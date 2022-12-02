//
// Created by blu-ray on 12/2/22.
//

#include "DFA.h"

set<NFA::State*>  DFA::getEclosure(NFA::State * startState) {
    set<NFA::State*> visitedStates;
    stack<NFA::State*> states;

    states.push(startState);
    visitedStates.insert(startState);

    while(!states.empty()){
        NFA::State* temp=states.top();
        states.pop();
        list<NFA::State*> closureStates=temp->epsilonTransitions;

        for (NFA::State* s : closureStates) {

            if(visitedStates.find(s) == visitedStates.end()){
                visitedStates.insert(s);
                states.push(s);
            }

        }
        return visitedStates;

    }

//    void convertNFAtoDFA(NFA::State*){
//
//    }





}
