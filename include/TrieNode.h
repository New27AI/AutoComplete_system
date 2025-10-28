//lego brick and for node level operations
#ifndef TRIENODE_H
#define TRIENODE_H

#include <array>
#include <memory>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <iostream>

using std::string;
using std::vector;
using std::pair;
using std::unique_ptr;

struct Suggestion {
    string word;
    int freq;
    
    bool operator<(const Suggestion& other) const {
        if (freq != other.freq) return freq > other.freq;
        return word < other.word;
    }
};

struct TrieNode {
    std::array<unique_ptr<TrieNode>, 26> children;
    bool isEndOfWord;
    int frequency;
    
    TrieNode();
    ~TrieNode() = default;
    
    void insert(const string& word);
    void insertUserWord(const string& word);
    bool search(const string& word) const;
    
    void autoComplete(const TrieNode* node, std::priority_queue<Suggestion>& heap, 
                     int k, const string& currPrefix) const;
    
    vector<pair<string, int>> getAllWithPrefix(const string& prefix, int k = 10) const;
    void sortResults(vector<pair<string, int>>& results) const;
};

#endif