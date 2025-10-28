#ifndef TRIE_H
#define TRIE_H

#include "TrieNode.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

using std::string;
using std::vector;
using std::unique_ptr;

class Trie {
public:
    Trie();
    ~Trie() = default;
    
    void insert(const string& word);
    void insertUserWord(const string& word);
    bool search(const string& word) const;
    
    // CHANGED: Remove const to allow internal recording
    vector<string> autoCompleteSystem(const string& prefix, int maxSuggestion = 10);
    
    // New methods for search query tracking
    void recordSearchQuery(const string& query);
    void recordCompleteSearch(const string& query);
    
    void saveToFile(const string& filename) const;
    void loadFromFile(const string& filename);
    void saveUserHistory(const string& filename) const;
    void loadUserHistory(const string& filename);

private:
    unique_ptr<TrieNode> root;
    unique_ptr<TrieNode> userRoot;
    std::unordered_map<string, int> userHistory;
    std::unordered_map<string, int> searchHistory;  // New: tracks search queries
};

#endif