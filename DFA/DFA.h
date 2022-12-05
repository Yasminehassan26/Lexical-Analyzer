//
// Created by blu-ray on 12/2/22.
//

#ifndef LEXICAL_ANALYZER_DFA_H
#define LEXICAL_ANALYZER_DFA_H


#include "../NFA/NFA.h"
#include "DfaState.h"

class DFA {

public:
    static set<NFA::State*> getEclosure(NFA::State*);
    vector<DfaState*> convertNFAtoDFA(NFA::State*);
    static string getNfaStateName(set<NFA::State*>);
    static string getDfaStateName(set<DfaState*>);

    static void isAcceptingStateNFA(DfaState*,set<NFA::State*>);
    static void isAcceptingStateDFA(DfaState*,set<DfaState*>);
    set<DfaState*> minimize(vector<DfaState*>);
    static set<DfaState*> minimizeHelper(set<set<DfaState*>>);

};


#endif //LEXICAL_ANALYZER_DFA_H
