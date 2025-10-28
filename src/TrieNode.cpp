#include "TrieNode.h"
#include <iostream>

TrieNode::TrieNode() : isEndOfWord(false), frequency(0) {
}

void TrieNode::insert(const string& word) {
    TrieNode* cur = this;
    for (char ch : word) {
        if (ch < 'a' || ch > 'z') continue;
        int index = ch - 'a';
        if (!cur->children[index])
            cur->children[index] = std::make_unique<TrieNode>();
        cur = cur->children[index].get();
    }
    
    if (cur->isEndOfWord) 
        cur->frequency++;
    else { 
        cur->isEndOfWord = true; 
        cur->frequency = 1;
    }
}

void TrieNode::insertUserWord(const string& word) {
    // Same as insert, but can be used for user-specific insertions
    insert(word);
}

bool TrieNode::search(const string& word) const {
    const TrieNode* cur = this;
    for (char ch : word) {
        if (ch < 'a' || ch > 'z') continue;
        int index = ch - 'a';
        if (!cur->children[index]) return false;
        cur = cur->children[index].get();
    }
    return cur->isEndOfWord;
}

void TrieNode::autoComplete(const TrieNode* node, 
                           std::priority_queue<Suggestion>& heap, 
                           int k, 
                           const string& currPrefix) const {
    if (!node) return;
    
    if (node->isEndOfWord) {
        Suggestion s{currPrefix, node->frequency};
        if ((int)heap.size() < k) {
            heap.push(s);
        } else if (heap.top() < s) {
            heap.pop();
            heap.push(s);
        }
    }
    
    for (int i = 0; i < 26; ++i) {
        if (node->children[i]) {
            char next = 'a' + i;
            autoComplete(node->children[i].get(), heap, k, currPrefix + next);
        }
    }
}

vector<pair<string, int>> TrieNode::getAllWithPrefix(const string& prefix, int k) const {
    const TrieNode* cur = this;
    for (char ch : prefix) {
        if (ch < 'a' || ch > 'z') return {};
        int index = ch - 'a';
        if (!cur->children[index]) return {};
        cur = cur->children[index].get();
    }
    
    std::priority_queue<Suggestion> heap;
    int maxSuggestions = k;
    autoComplete(cur, heap, maxSuggestions, prefix);
    
    vector<pair<string, int>> results;
    while (!heap.empty()) {
        results.emplace_back(heap.top().word, heap.top().freq);
        heap.pop();
    }
    
    std::reverse(results.begin(), results.end());
    return results;
}

void TrieNode::sortResults(vector<pair<string, int>>& results) const {
    std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first;
    });
}