//
// Created by blu-ray on 12/2/22.
//

#ifndef LEXICAL_ANALYZER_DFA_H
#define LEXICAL_ANALYZER_DFA_H


#include "../NFA.h"

class DFA {

public:
    static set<NFA::State*> getEclosure(NFA::State*);
    void convertNFAtoDFA(NFA::State*);
    static string getStateName(set<NFA::State*>);
};


#endif //LEXICAL_ANALYZER_DFA_H
