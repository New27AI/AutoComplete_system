#include<iostream>
#include<algorithm>
#include<string>
#include<vector>
using namespace std;

struct Trienode{
    Trienode* children[26];
    bool isEndOfWord;
    int frequency;
    Trienode(){
        for(int i=0;i<26;i++)children[i] = nullptr;
        isEndOfWord = false;
        frequency = 0;
    }
    ~Trienode(){
        for(int i=0;i<26;i++){
            if(children[i]) delete children[i];
        }
    }
};

class Trie{
    public:
    Trienode* root;
    Trie(){
        root = new Trienode;
    }
    ~Trie(){
        delete root;
    }

//search
bool search(const string&word){
    Trienode* current = root;
    for(char ch:word){
        int index = ch - 'a';
        if(current->children[index] == nullptr){
            return false;
        }
        current = current->children[index];
    }
    return current->isEndOfWord;
}
//insert
void insert(const string&word){
    Trienode* current = root;
    for(char ch:word){
        int index = ch - 'a';
        if(current->children[index] == nullptr){
            current->children[index] = new Trienode();
        }
        current = current->children[index];
    }
    if(current->isEndOfWord){
        current->frequency++;
    }
    else{
        current->isEndOfWord = true;
        current->frequency = 1;
    }
}
//autocomplete
void autocomplete(Trienode* node,vector<pair<string,int>>&results,const string & currPrefix){
    if(node == nullptr)return;
    if(node->isEndOfWord){
        results.push_back({currPrefix,node->frequency});
    }
    for(int i=0;i<26;i++){
        if(node->children[i] != nullptr){
            char nextChar = 'a' + i;
            autocomplete(node->children[i],results,currPrefix+nextChar);
        }
    }
}
//getAllwithPrefix
vector<pair<string,int>>getAllwithPrefix(const string &prefix){
    Trienode* current = root;
    vector<pair<string,int>>results;
    for(char ch:prefix){
        int index = ch - 'a';
        if(current->children[index] == nullptr){
            return results;
        }
        current= current->children[index];
    }
    autocomplete(current,results,prefix);
    return results;
}
//sorting the results
void sortResults(vector<pair<string,int>>&results){
    sort(results.begin(),results.end(),[](const pair <string,int>&a , const pair<string,int>&b){
        if(a.second != b.second)return a.second > b.second;
        return a.first < b.first;
    });
}
//Final suggestions
vector<string>autoCompleteSystem(const string&prefix, int maxSuggestions=10){
    auto rawResults = getAllwithPrefix(prefix);
    sortResults(rawResults);
    vector<string>suggestions;
    for(int i=0;i<rawResults.size()&&i<maxSuggestions;++i){
        suggestions.push_back(rawResults[i].first);
    }
    return suggestions;
}
};
int main(){
    Trie t;
    string command,argument;
    cout << " Select any one:\n"
        << "insert\t" << "suggest<prefix>\t" << "exit\t";
        while(true){
            cout << "\n";
            cin >> command;
            if(command=="insert"){
                cout << "word:";
                cin >> argument;
                t.insert(argument);
                cout << "done!";
            }
            else if(command=="suggest"){
                cout << "word:";
                cin >> argument;
                auto suggestions = t.autoCompleteSystem(argument);
                for(auto& w:suggestions){
                   cout << w <<"\n";
                }
            }
            else if(command=="exit"){
                cout << "exiting...";
                break;
            }
            else{
                cout << "Unknown Request..";
            }
        }
        return 0;
}