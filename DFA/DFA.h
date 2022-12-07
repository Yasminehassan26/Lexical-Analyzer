//
// Created by blu-ray on 12/2/22.
//

#ifndef LEXICAL_ANALYZER_DFA_H
#define LEXICAL_ANALYZER_DFA_H


#include "../NFA/NFA.h"
#include "DfaState.h"

class DFA {

public:
    static vector<NFA::State*> getEclosure(NFA::State*);
    vector<DfaState*> convertNFAtoDFA(NFA::State*);
    static string getNfaStateName(vector<NFA::State*>);
    static string getDfaStateName(set<DfaState*>);

    static void isAcceptingStateNFA(DfaState*,vector<NFA::State*>);
    static void isAcceptingStateDFA(DfaState*,set<DfaState*>);
    set<DfaState*> minimize(vector<DfaState*>);
    static set<DfaState*> minimizeHelper(set<set<DfaState*>>);

};


#endif //LEXICAL_ANALYZER_DFA_H
