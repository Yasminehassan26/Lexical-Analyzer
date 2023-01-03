#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include<vector>
#include "table.h"
#include "Parsing.h"
#include "LexicalAnalyzerSimulator/Simulator.h"

using namespace std;
string startSympol = "";
vector<string> nonTerminals;

map<string,set<string>> followSet(map<string,vector<vector<string>>> rules,map<string,set<string>>firstSet) {
    map<string, set<string>> followSetMap;
    followSetMap[startSympol].insert("$");
    bool converge = false;
    int iteration = 0 ;
    // for (iteration = 0 ; iteration< 10 ;iteration++) {
    while (!converge || iteration < 4){
        iteration++;
        map<string, set<string>> prevFollowSetMap = followSetMap;
        for (auto const &x: rules) {
            string nonTerminal = x.first;
            //setProductionFollow(rules,firstSet,followSetMap,nonTerminal);
            for (auto element: x.second) {
                int i = 0;
                while (i < element.size()) {
                    auto t = element[i];
                    //skip non terminals
                    while (i < element.size() - 1 && t[0] == '\'') {
                        i++;
                        t = element[i];
                    }
                    //first non terminal
                    i++;
                    if (i < element.size()) {
                        t = element[i];
                        if (t[0] == '\'') followSetMap[element[i - 1]].insert(t.substr(1, t.size() - 2));
                            //non terminal followed by non terminal ex A B
                            // follow A is first of B
                        else {
                            set<string> first = firstSet[t];
                            followSetMap[element[i - 1]].insert(first.begin(), first.end());
                            //if first of B has epsilon ex A B C
                            // follow A is first of C also
                            int index = i + 1;
                            while (followSetMap[element[i - 1]].find("\\L") != followSetMap[element[i - 1]].end() &&
                                   index < element.size()) {
                                followSetMap[element[i - 1]].erase("\\L");
                                // terminal A B c  and B fist has epsilon
                                if (element[index][0] == '\'')
                                    followSetMap[element[i - 1]].insert(
                                            element[index].substr(1, element[index].size() - 2));
                                    //non terminal
                                else {
                                    set<string> nextFollow = firstSet[element[index]];
                                    followSetMap[element[i - 1]].insert(nextFollow.begin(), nextFollow.end());
                                }
                                index++;
                            }
                            //last symbol then add the header of the production
                            if (followSetMap[element[i - 1]].find("\\L") != followSetMap[element[i - 1]].end()) {
                                followSetMap[element[i - 1]].erase("\\L");
                                set<string> headerFollow = followSetMap[nonTerminal];
                                followSetMap[element[i - 1]].insert(headerFollow.begin(), headerFollow.end());
                            }
                        }
                    }
                }
                // last sympol then add header of production
                if (element[element.size() - 1][0] != '\'' && element[element.size() - 1] != "\\L") {
                    set<string> headerFollow = followSetMap[nonTerminal];
                    followSetMap[element[element.size() - 1]].insert(headerFollow.begin(), headerFollow.end());
                }
            }
        }
        //convergence update
        int nTCount = 0 ;
        for(auto nT : prevFollowSetMap){
            int prevSize = nT.second.size();
            int currentSize = followSetMap[nT.first].size();
            if(prevSize != currentSize) break;
            nTCount++;
        }
        if(nTCount == prevFollowSetMap.size()) converge = true;
    }

    cout << "num of iterations to converger " << iteration <<endl;
    return followSetMap;
}


set<string> setProductionFirst(map<string,vector<vector<string>>> rules,map<string,set<string>> &firstSet,string nonTerminal) {
    if (firstSet[nonTerminal].size() == 0) {
        vector<vector<string>> productions = rules[nonTerminal];
        set<string> first;
        for (auto element: productions) {
            auto t = element[0];
            if (t[0] == '\''){
                first.insert(t.substr(1, t.size() - 2));
            }

            else if (t == "\\L") first.insert(t);
            else {
                set<string> nT_first = setProductionFirst(rules,firstSet,t);
                first.insert(nT_first.begin(), nT_first.end());
                // contains epsilon
                int index = 1;
                while (first.find("\\L")!= first.end() && index < element.size()) {
                    first.erase("\\L");
                    nT_first.clear();
                    // terminal
                    if (element[index][0] == '\'') {
                        first.insert(element[index].substr(1, element[index].size() - 2));
                    }
                        //non terminal
                    else {
                        nT_first = setProductionFirst(rules,firstSet,element[index]);
                        first.insert(nT_first.begin(), nT_first.end());}
                    index++;
                }
            }
            firstSet[nonTerminal].insert(first.begin(),first.end());
            first.clear();
        }
    }
    return firstSet[nonTerminal];
}

map<string,set<string>> firstSet(map<string,vector<vector<string>>> rules){
    map<string,set<string>> firstSetMap;
    for (auto const& x : rules)
    {
        string nonTerminal = x.first;
        setProductionFirst(rules,firstSetMap,nonTerminal);
    }

    return firstSetMap;
}


void Print(map<string, vector<vector<string>>> rules,string message) {
    std::cout << message << endl;
    for (auto const &x: rules) {
        std::cout << x.first  // string (key)
                  << ':';
        int l = x.second.size();
        int i = 0;
        for (auto element: x.second) {
            for (auto t: element) {
                cout << t << " ";
            }
            i++;
            if (i < l)
                cout << '|' << " ";
        }
        cout << "" << endl;

    }
}

map<string,vector<vector<string>>> readRules(){

    map<string,vector<vector<string>>> rules;
    //stmt = if stmt then stmt|while stmt
    fstream newfile;
    newfile.open("D:\\Lexical-Analyzer\\CFG_Rules.txt",ios::in);
    bool firstLine = true;

    if (newfile.is_open()){
        string line;
        string nonTerminal;
        string temp = "";
        vector<string> production ;

        while(getline(newfile, line)) {
            int index = 0 ;

            if(line[0] == '#'){
                if(!firstLine) {
                    production.push_back(temp);
                    rules[nonTerminal].push_back(production);
                }
                int s = 1 ;
                while(s<line.size() && line[s]==' ') s++;
                nonTerminal = line.substr(s, line.find("::=")-s);
                if(nonTerminal[nonTerminal.size()-1]==' ')nonTerminal=nonTerminal.substr(0,nonTerminal.size()-1);
                nonTerminals.push_back(nonTerminal);
                if(firstLine){
                    firstLine = false;
                    startSympol = nonTerminal;
                }
                index = line.find("::=")+3 ;
                while(index<line.size() && line[index] ==' ')index++;
                temp = "";
                production.clear();
            }

            int i = index;
            while(i<line.size()){
                if ((line[i] == ' ' && line[i+1] == '|') || (line[i]=='|') ) {
                    if(temp != "") production.push_back(temp);
                    rules[nonTerminal].push_back(production);
                    production.clear();
                    temp = "";
                    if(line[i] == ' ') i+=2;
                    else if (line[i] == '|') i+=1;
                    while(i<line.size() && line[i]==' ') i++;
                }
                if (line[i] == ' ') {
                    production.push_back(temp);
                    temp = "";
                }
                else {
                    temp += line[i];
                }
                i++;
            }
        }
        production.push_back(temp);
        rules[nonTerminal].push_back(production);
        newfile.close(); //close the file object.
    }
    cout<<"num of rules " <<rules.size()<<endl;

    //printing

    for (auto const& x : rules)
    {
        std::cout << x.first  // string (key)
                  << ':';
        int l = x.second.size();
        int i = 0;
        for (auto element : x.second) {
            for (auto t : element) {
                cout << t << " ";
            }
            i++;
            if(i < l)
                cout << '|' << " ";
        }
        cout<<""<<endl;

    }
    return rules;
}


map<string, vector<vector<string>>> eliminateLeftRecursion(map<string, vector<vector<string>>> rules,vector<string> nonTerminals) {
    map<string, vector<vector<string>>> newRules;
    for (int s=0;s<nonTerminals.size();s++) {
        string nonTerminal=nonTerminals.at(s);
        bool check=true;
        int counter;
        while(check){
            counter=0;
            vector<vector<string>> modifiedVector;
            vector<vector<string>> wanted;
            if(newRules.find(nonTerminal) != newRules.end()){
                 wanted=newRules[nonTerminal];
            }
            else wanted=rules[nonTerminal];
            for (int k = 0; k < wanted.size(); k++) {
                vector<string> ruleVector;

                ruleVector = wanted.at(k);

                string nonTerminal2 = ruleVector.at(0);
                // in case of substitute
                //check if terminal or not terminal
                if (nonTerminal != nonTerminal2 && newRules.find(nonTerminal2) != newRules.end()) {
                    //loop on map till finding the matching rule
                    vector<vector<string>> tempRule = newRules.find(nonTerminal2)->second;
                    vector<string> newVector = ruleVector;
                    newVector.erase(newVector.begin());
                    for (auto replacedVector: tempRule) {
                        vector<string> temp;
                        temp.reserve(replacedVector.size() + newVector.size());
                        temp.insert(temp.end(), replacedVector.begin(), replacedVector.end());
                        temp.insert(temp.end(), newVector.begin(), newVector.end());
                        modifiedVector.push_back(temp);
                    }


                } else {
                    //in case I didn't find a matching i add the vector it-self
                    modifiedVector.push_back(ruleVector);
                    counter++;
                }
            }
            newRules[nonTerminal] = modifiedVector;
            if(counter==newRules.find(nonTerminal)->second.size()){
                check=false;
            }
        }
        //begin eliminating
        vector<vector<string>> eliminatedVector = newRules.find(nonTerminal)->second;
        vector<vector<string>> restEliminatedVector;

        string newNonTerminal=nonTerminal+"'";
        while (rules.find(newNonTerminal) != rules.end() || newRules.find(newNonTerminal) != newRules.end()){
            newNonTerminal=newNonTerminal+"'";
        }
        for (auto vec: eliminatedVector) {
            if (nonTerminal == vec[0]) {
                vector<string> newNonTerminalVector = vec;
                newNonTerminalVector.erase(newNonTerminalVector.begin());
                newNonTerminalVector.push_back(newNonTerminal);
                newRules[newNonTerminal].push_back(newNonTerminalVector);
            } else {
                if (vec[0] == "\\L") {
                    vec.erase(vec.begin());
                }
                vec.push_back(newNonTerminal);
                restEliminatedVector.push_back(vec);
            }
        }


        if (newRules.find(newNonTerminal) != newRules.end()) {
            newRules[nonTerminal] = restEliminatedVector;
            vector<string> eps;
            eps.push_back("\\L");
            newRules[newNonTerminal].push_back(eps);
        }

    }
    Print(newRules,"Printing the rules after left recursion elimination");
    return newRules;
}

map<string, vector<vector<string>>> eliminateLeftFactoring(map<string, vector<vector<string>>> rules) {

    vector<string> nonTerminals;
    for(auto rule:rules){
        nonTerminals.push_back(rule.first);
    }

    for (int s=0;s<nonTerminals.size();s++) {
        string nonTerminal=nonTerminals.at(s);

        bool check=true;
        while(check){
            int counter=0;
            vector<vector<string>> wanted=rules[nonTerminal];

            for(int i=0;i< wanted.size();i++){
                vector<vector<string>> eliminatedVector ;
                vector<vector<string>> restEliminatedVector;
                vector<string> vec1=wanted.at(i);
                string first1=vec1.at(0);
                for(int j=i;j<wanted.size();j++){
                    vector<string> vec2=wanted.at(j);
                    string first2=vec2.at(0);
                    if(first1==first2){
                        vec2.erase(vec2.begin());
                        if(vec2.empty()){
                            vec2.push_back("\\L");
                        }
                        eliminatedVector.push_back(vec2);
                    }
                    else restEliminatedVector.push_back(vec2);
                }
                if(eliminatedVector.size()>1){
                    string newNonTerminal=nonTerminal+"'";
                    while(rules.find(newNonTerminal) != rules.end()){
                        newNonTerminal=newNonTerminal+"'";
                    }
                    nonTerminals.push_back(newNonTerminal);
                    rules[newNonTerminal]=eliminatedVector;
                    vector<string> temp;
                    temp.push_back(first1);
                    temp.push_back(newNonTerminal);
                    restEliminatedVector.push_back(temp);
                    rules[nonTerminal]=restEliminatedVector;
                    break;
                }
                else{
                    counter++;
                }
                }
            if(counter==wanted.size()){
                check= false;
            }
            }

    }


    Print(rules,"printing the rules after left factoring elimination ");
    return rules;
}
map<string, vector<vector<string>>> refactorRules(map<string, vector<vector<string>>> rules) {
    map<string, vector<vector<string>>> eliminatedLeftRecursion = eliminateLeftRecursion(rules);
    map<string,vector<vector<string>>> eliminatedLeftFactoring = eliminateLeftFactoring(eliminatedLeftRecursion);
    return eliminatedLeftFactoring;
}

void printFirst(map<string, set<string>> firstSet){
    cout << "=============== FIRST SET =================" << endl;
    for(auto i : firstSet){
        int count = 0;
        cout << "FIRST( " <<i.first << " ) = { " ;
        for(string j : i.second){
            cout << j ;
            count++;
            if(count == i.second.size())
                cout << " }" << endl;
            else
                cout << ", ";
        }
    }
}

void printFollow(map<string, set<string>> followSet){
    cout << "=============== FOLLOW SET =================" << endl;
    for(auto i : followSet){
        int count = 0;
        cout << "FOLLOW( " <<i.first << " ) = { " ;
        for(string j : i.second){
            cout << j ;
            count++;
            if(count == i.second.size())
                cout << " }" << endl;
            else
                cout << ", ";
        }
    }
}

int main() {
//    map<string, vector<vector<string>>> rules = readRules();
//    map<string, vector<vector<string>>> refactoredRules = refactorRules(rules);
//
//    map<string, set<string>> firstSetMap = firstSet(refactoredRules);
//    map<string, set<string>> followSetMap = followSet(refactoredRules, firstSetMap);

    map<string, vector<vector<string>>> rules = readRules();
    map<string, vector<vector<string>>> refactoredRules = refactorRules(rules);

//    map<string, set<string>> firstSetMap = firstSet(rules);
//    map<string, set<string>> followSetMap = followSet(rules, firstSetMap);

    //printFirst(firstSetMap);
    //printFollow(followSetMap);

//    table *t = new table();
//    map<string, map<string, vector<string>>> parsingTable = t->getParsingTable(rules, firstSetMap, followSetMap);
//    t->printTable(parsingTable);
//
//    Simulator *sim = new Simulator();
//    vector<pair<string,string>> input = sim -> lexicalAnalyzer();
//
//    Parsing *p = new Parsing();
//    p->parse(startSympol,parsingTable,input);

    return 0;
}