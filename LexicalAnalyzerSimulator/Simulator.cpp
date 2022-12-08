
#include <string>
#include <stack>
#include <map>
#include <deque>
#include <vector>
#include <set>
#include <algorithm>
#include <list>
#include <stdbool.h>
#include <iostream>
#include <fstream>
#include "Simulator.h"
#include "../NFA/NFA.h"
#include "../DFA/DFA.h"
#include "../DFA/DfaState.h"

using namespace std;

Simulator::Simulator(){
}

DfaState* Simulator::lexicalGenerator()
{
    NFA *nfa = new NFA();
    DFA *dfa = new DFA();

    nfa->readRules();

    // code to print nfa
    NFA::FSATABLE finalNFA = nfa->NFATable;
    vector<DfaState*> conv = dfa->convertNFAtoDFA(finalNFA.front());
    set<DfaState*> minDFA = dfa->minimize(conv);
//    fstream newfile;
//    newfile.open("C:\\Users\\Carnival\\CLionProjects\\Lexical-Analyzer\\LexicalAnalyzerSimulator\\TransitionTable.txt", ios::out);
//    for(auto itr: minDFA){
//        if(itr->acceptingState){
//            newfile << "---------- State (" << itr->id <<") (Final State: " << itr->acceptingPattern << ") Transitions ----------" << endl;
//        }else{
//            newfile << "---------- State (" << itr->id <<") Transitions ----------" << endl;
//        }
//
//        // print transitions
//        multimap<char,DfaState*>::iterator itr1;
//        for ( itr1 = itr->transitions.begin(); itr1 != itr->transitions.end(); itr1++)
//            newfile << itr1->first << " -> " << itr1->second->id << endl;
//    }
//    newfile.close();
    return *(conv.begin());
}

multimap<string, string> Simulator::lexicalAnalyzer()
{
    DfaState* s0 = lexicalGenerator();
    return patternMatch(s0);
}

multimap<string, string> Simulator::patternMatch(DfaState* s0)
{
    fstream newfile;
    newfile.open("C:\\Users\\Carnival\\CLionProjects\\Lexical-Analyzer\\LexicalAnalyzerSimulator\\code.txt", ios::in);
    fstream outputfile;
    outputfile.open("C:\\Users\\Carnival\\CLionProjects\\Lexical-Analyzer\\LexicalAnalyzerSimulator\\out.txt", ios::out);
    multimap<string, string> result;
    if (newfile.is_open())
    {
        string inputLine;
        while (getline(newfile, inputLine))
        {
            DfaState*s = s0;
            string temp = "";
            string lastAcceptedPattern = s0->acceptingPattern;
            int lastIndex = 0;
            int i = 0;
            while (i < inputLine.size())
            {
                char ch = inputLine[i];
                if (s->transitions.find(ch)==s->transitions.end())
                {
                    i = lastIndex+1;
                    s = s0;
                    result.insert({temp, lastAcceptedPattern});

                    // write to output file here
                    outputfile << lastAcceptedPattern << endl;

                    lastAcceptedPattern = "";
                    temp = "";
                    if (inputLine[i] == ' ')
                        i++;
                }
                else
                {
                    s = s->transitions.find(ch)->second;
                    temp += ch;
                    i++;
                }
                if (s->acceptingState)
                {
                    lastAcceptedPattern = s->acceptingPattern;
                    lastIndex = i-1;
                }
            }
            if(lastAcceptedPattern!=""){
                i = lastIndex+1;
                s = s0;
                result.insert({temp, lastAcceptedPattern});

                // write to output file here
                outputfile << lastAcceptedPattern << endl;
                lastAcceptedPattern = "";
                temp = "";
            }
        }
        outputfile.close();
        newfile.close();
    }

    newfile.close();
    return result;
}

//bool compareDFA(DfaState* i1, DfaState* i2){
//    return i1->id < i2->id;
//}
