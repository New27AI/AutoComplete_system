#include<bits/stdc++.h>
using namespace std;

struct TrieNode {
    TrieNode* children[26];  
    bool isEndOfWord;
    int frequency;

    TrieNode() {
    for(int i = 0; i < 26; i++) {
        children[i] = nullptr;  
    }
    isEndOfWord = false;
    frequency = 0;        
    }
};

class Trie{
    private:
    TrieNode* root;

    public:
    Trie(){
        root = new TrieNode();
    }

void insert(string word) {
    TrieNode* current = root; // Start at root
    
    for(char ch : word) {
        int index = ch - 'a';  // Your conversion logic!
        
        // What do we check here?
        if(current->children[index] == NULL) {
            // What do we do if child doesn't exist?
            current->children[index] = new TrieNode();
        }
        
        // Move to next node
        current = current->children[index];
    }
    
    // After processing all characters, what do we do?
    if(current->isEndOfWord) {
        // Your logic! Word already exists
        current->frequency++;
    } else {
        // Word is new
        current->isEndOfWord = true;
        current->frequency = 1;  // What should initial frequency be?
     }
}
bool search(string word) {
    TrieNode* current = root;
    
    for(char ch : word) {
        int index = ch - 'a';
        
        // What do we check here?
        if(current->children[index] == nullptr) {
            return false; // Word doesn't exist
        }
        
        current = current->children[index];
    }
    
    // We've traversed the full word, but is it complete?
    return current->isEndOfWord; // What should we return here?
}

};