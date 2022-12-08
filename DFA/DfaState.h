//
// Created by blu-ray on 12/2/22.
//

#ifndef LEXICAL_ANALYZER_DFASTATE_H
#define LEXICAL_ANALYZER_DFASTATE_H
#include <string>
#include <map>
#include <list>
#include <set>
#include <stack>
#include <deque>
#include <vector>
#include <algorithm>
#include <stdbool.h>
#include <iostream>

using namespace std;

class DfaState {
public:
    int id;
    string name;
    multimap<char, DfaState*> transitions;
    // list of states that this state goes to with epsilon transition
    // True if this state is accepting state (final state)
    bool acceptingState=false;
    string acceptingPattern;
    int patternPriority = -1;

    DfaState();
    DfaState(int,string);
    void setTransitions(DfaState*,char);
    multimap<char, DfaState*> getTransitions();
    DfaState* getTransition(char);
    void setAcceptingState();
    bool getIsAcceptingState();
    void setAcceptingPattern(string);
    string getAcceptingPattern();

    int getId();

    string getName();
    bool operator< (const DfaState &other) const {
        return id < other.id;
    }

};


#endif //LEXICAL_ANALYZER_DFASTATE_H
