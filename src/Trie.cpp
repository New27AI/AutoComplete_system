#include "Trie.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>

Trie::Trie() : root(std::make_unique<TrieNode>()),
               userRoot(std::make_unique<TrieNode>()) {}

void Trie::insert(const string& word) {
    root->insert(word);
}

void Trie::insertUserWord(const string& word) {
    userRoot->insert(word);
    userHistory[word]++;
}

bool Trie::search(const string& word) const {
    return root->search(word);
}

void Trie::recordSearchQuery(const string& query) {
    if (query.empty()) return;
    
    // Track partial search queries
    searchHistory[query]++;
    
    // Insert into user trie for future suggestions
    userRoot->insert(query);
    
    std::cout << "Recorded search query: '" << query << "' (count: " << searchHistory[query] << ")\n";
}

void Trie::recordCompleteSearch(const string& query) {
    if (query.empty()) return;
    
    // Give extra weight to complete searches
    searchHistory[query] += 10;
    userHistory[query] += 10;
    
    // Insert into user trie
    userRoot->insert(query);
    
    std::cout << "Recorded complete search: '" << query << "' (total count: " << searchHistory[query] << ")\n";
}

// FIXED: Remove const and record search queries for prefixes length > 2
vector<string> Trie::autoCompleteSystem(const string& prefix, int maxSuggestions) {
    if (prefix.empty()) {
        return {};
    }
    
    std::cout << "\n=== AutoComplete Debug for '" << prefix << "' ===\n";
    
    // Show current search history for debugging
    std::cout << "Current search history:\n";
    for (const auto& entry : searchHistory) {
        std::cout << "  '" << entry.first << "': " << entry.second << "\n";
    }
    
    // Record search query for prefixes longer than 1 character (reduced threshold)
    if (prefix.length() > 1) {
        searchHistory[prefix]++;
        std::cout << "Auto-recorded search query: '" << prefix << "' (count: " << searchHistory[prefix] << ")\n";
    }
    
    // Collect pairs (word, combinedFrequency)
    vector<std::pair<string, int>> allResults;
    
    // Get from user history trie and boost frequencies for searched terms
    auto userResults = userRoot->getAllWithPrefix(prefix, maxSuggestions * 3); // Increased multiplier
    
    std::cout << "User results found: " << userResults.size() << "\n";
    
    for (auto& result : userResults) {
        int boostedFreq = result.second;
        std::cout << "Processing user result: '" << result.first << "' (base freq: " << boostedFreq << ")\n";
        
        // Check if this word was searched as a complete query
        if (searchHistory.find(result.first) != searchHistory.end()) {
            int boost = searchHistory.at(result.first) * 1000;
            boostedFreq += boost;
            std::cout << "  Boosting '" << result.first << "' by " << boost << " (search history)\n";
        }
        
        // Additional boost if it was added as user word
        if (userHistory.find(result.first) != userHistory.end()) {
            int boost = userHistory.at(result.first) * 100;
            boostedFreq += boost;
            std::cout << "  Boosting '" << result.first << "' by " << boost << " (user history)\n";
        }
        
        // MEGA boost if it starts with the prefix and was searched
        for (const auto& searchEntry : searchHistory) {
            if (searchEntry.first.find(prefix) == 0 && searchEntry.first == result.first) {
                int megaBoost = searchEntry.second * 5000;
                boostedFreq += megaBoost;
                std::cout << "  MEGA BOOST for '" << result.first << "' by " << megaBoost << "\n";
            }
        }
        
        allResults.emplace_back(result.first, boostedFreq);
        std::cout << "  Final boosted freq for '" << result.first << "': " << boostedFreq << "\n";
    }
    
    // If underfilled, get from main dictionary trie
    if ((int)allResults.size() < maxSuggestions) {
        auto dictResults = root->getAllWithPrefix(prefix, maxSuggestions);
        std::cout << "Dictionary results found: " << dictResults.size() << "\n";
        
        for (auto& p : dictResults) {
            // Check if word already exists in user results
            auto it = find_if(allResults.begin(), allResults.end(),
                [&](const auto& u) { return u.first == p.first; });
            
            if (it == allResults.end()) {
                // Check if this dictionary word was ever searched
                int dictFreq = p.second;
                if (searchHistory.find(p.first) != searchHistory.end()) {
                    int boost = searchHistory.at(p.first) * 500;
                    dictFreq += boost;
                    std::cout << "Boosting dict word '" << p.first << "' by " << boost << "\n";
                }
                allResults.emplace_back(p.first, dictFreq);
                
                if ((int)allResults.size() >= maxSuggestions) break;
            }
        }
    }
    
    // Sort by frequency descending, then lexicographically
    sort(allResults.begin(), allResults.end(),
        [](const auto& a, const auto& b) {
            if (a.second != b.second) return a.second > b.second;
            return a.first < b.first;
        });
    
    // Trim to maxSuggestions
    if ((int)allResults.size() > maxSuggestions) {
        allResults.resize(maxSuggestions);
    }
    
    // Extract words and log for debugging
    vector<string> suggestions;
    std::cout << "=== Final suggestions for '" << prefix << "' ===\n";
    for (const auto& p : allResults) {
        suggestions.push_back(p.first);
        std::cout << "Suggestion: '" << p.first << "' (freq: " << p.second << ")\n";
    }
    std::cout << "===========================================\n\n";
    
    return suggestions;
}

void Trie::saveToFile(const string& filename) const {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Cannot open " << filename << " for writing\n";
        return;
    }
    
    auto entries = root->getAllWithPrefix("");
    for (auto& p : entries) {
        out << p.first << "," << p.second << "\n";
    }
}

void Trie::loadFromFile(const string& filename) {
    std::ifstream in(filename);
    if (!in) return;
    
    string line;
    while (getline(in, line)) {
        std::istringstream iss(line);
        string word;
        char comma;
        int freq;
        if (iss >> word >> comma >> freq) {
            for (int i = 0; i < freq; ++i) {
                root->insert(word);
            }
        }
    }
}

void Trie::saveUserHistory(const string& filename) const {
    std::ofstream out(filename);
    if (!out) {
        std::cerr << "Cannot save user history to " << filename << "\n";
        return;
    }
    
    // Save user word history
    out << "[USER_WORDS]\n";
    for (const auto& entry : userHistory) {
        out << entry.first << " " << entry.second << "\n";
    }
    
    // Save search history
    out << "[SEARCH_HISTORY]\n";
    for (const auto& entry : searchHistory) {
        out << entry.first << " " << entry.second << "\n";
    }
    
    std::cout << "Saved user history with " << searchHistory.size() << " search entries\n";
}

void Trie::loadUserHistory(const string& filename) {
    std::ifstream in(filename);
    if (!in) {
        std::cout << "No existing user history file found. Starting fresh.\n";
        return;
    }
    
    string line;
    bool inSearchHistory = false;
    
    while (getline(in, line)) {
        if (line == "[SEARCH_HISTORY]") {
            inSearchHistory = true;
            continue;
        }
        if (line == "[USER_WORDS]") {
            inSearchHistory = false;
            continue;
        }
        
        if (line.empty()) continue;
        
        std::istringstream iss(line);
        string word;
        int freq;
        
        if (iss >> word >> freq) {
            if (inSearchHistory) {
                searchHistory[word] = freq;
                std::cout << "Loaded search history: '" << word << "' = " << freq << "\n";
            } else {
                userHistory[word] = freq;
                std::cout << "Loaded user word: '" << word << "' = " << freq << "\n";
            }
            
            // Rebuild user trie
            for (int i = 0; i < freq; ++i) {
                userRoot->insert(word);
            }
        }
    }
    
    std::cout << "Loaded " << searchHistory.size() << " search history entries\n";
}