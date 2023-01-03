//
// Created by Carnival on 1/3/2023.
//
#include <iostream>
#include "table.h"

set<string> table::getFirst(vector<string> subRule, set<string> nonTerminals, map<string, set<string>> firstSet){
    set<string> first;
    for(auto i : subRule){
        // if terminal
        if(nonTerminals.find(i) == nonTerminals.end()){
            if (i[0] == '\''){
                first.insert(i.substr(1, i.size() - 2));
            }else{
                first.insert(i);
            }
            break;
        }else{
            set<string> temp = firstSet[i];
            first.insert(temp.begin(), temp.end());
            // remove epsilon
            first.erase("\\L");
            // doesn't contain epsilon don't continue
            if(temp.find("\\L") == temp.end())
                break;
        }
    }
    bool allContainsEpsilon = true;
    for(auto i : subRule){
        if(nonTerminals.find(i) == nonTerminals.end()){
            allContainsEpsilon = false;
            break;
        }
        // didn't find epsilon
        if(firstSet[i].find("\\L") == firstSet[i].end()){
            allContainsEpsilon = false;
            break;
        }
    }
    if(allContainsEpsilon)
        first.insert("\\L");
    return first;
}

map<string, map<string, vector<string>>> table::getParsingTable(map<string,vector<vector<string>>> rules,
                                                                map<string,set<string> > firstSet, map<string,set<string> > followSet){

    set<string> nonTerminals;
    for(auto rule : rules)
        nonTerminals.insert(rule.first);
    map<string, map<string, vector<string>>> parsingTable;
    // parsing table
    for(auto rule : rules){
        map<string, vector<string>> tableEntry;
        string nonTerminal = rule.first;
        for(auto subRule : rule.second){
            // for each sub-rule
            set<string> subRuleFirst = getFirst(subRule, nonTerminals, firstSet);
            // insert into table
            for(auto terminal : subRuleFirst){
                if(terminal != "\\L"){
                    if(tableEntry.find(terminal) == tableEntry.end())
                        tableEntry.insert({terminal, subRule});
                    else{
                        cout << "NOT LL(1) GRAMMAR !!" << endl;
                        exit(0);
                    }
                }else{
                    // Epsilon found -> insert terminals in FOLLOW(non-terminal)
                    set<string> follow = followSet[nonTerminal];
                    for(auto f : follow){
                        if(tableEntry.find(f) == tableEntry.end())
                            tableEntry.insert({f, subRule});
                        else{
                            cout << "NOT LL(1) GRAMMAR !!" << endl;
                            exit(0);
                        }
                    }
                }
            }
        }
        // adding sync
        bool addSync = firstSet[nonTerminal].find("\\L") == firstSet[nonTerminal].end();
        if(addSync){
            for(auto i : followSet[nonTerminal])
                if(tableEntry.find(i) == tableEntry.end()){
                    tableEntry.insert({i, {"sync"}});
                }
        }
        parsingTable.insert({nonTerminal, tableEntry});
    }
    return parsingTable;
}

void table::printTable(map<string, map<string, vector<string>>> parsingTable){
    cout << "------------------------------------------------------------------------" << endl;
    for(auto row : parsingTable){
        cout << row.first << ": ";
        int count = 0;
        for(auto terminal : row.second){
            count++;
            cout << "[ " << terminal.first << " ] => ";
            for(auto symbol : terminal.second){
                cout << symbol << " ";
            }
            if(count < row.second.size()){
                if(count % 3 == 0) {
                    cout << "\n ";
                }
                else
                    cout << "\t ";
            }
        }
        cout << endl << "==================================================================================================" << endl;
    }
}