//
// Created by blu-ray on 12/2/22.
//

#include "DFA.h"
#include "DfaState.h"

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
    }
    return visitedStates;
}

DfaState* convertNFAtoDFA(NFA::State* start){
    int id = 0;
    set<NFA::State*> epsilon = DFA::getEclosure(start);
    DfaState* inputState = new DfaState(id++,DFA::getStateName(epsilon));
    map<string,DfaState*> visitedStates;
    map<string,pair<DfaState*,set<NFA::State*>>> notVisitedStates;
    notVisitedStates.insert({inputState->getName(),pair<DfaState*,set<NFA::State*>>(inputState,epsilon)});
    while(!notVisitedStates.empty()){
        pair<DfaState*,set<NFA::State*>> temp = notVisitedStates.begin()->second;
        visitedStates.insert(pair<string,DfaState*>(temp.first->getName(),temp.first));
        notVisitedStates.erase(notVisitedStates.begin());
        map<char,set<NFA::State*>> DFAtransitions;
        for (auto itr : temp.second){
            multimap<char, NFA::State*> transitions = itr->transitions;
            for (auto itr1 : transitions){
                if(DFAtransitions.find(itr1.first)!=DFAtransitions.end()){
                    set<NFA::State*> ep = DFA::getEclosure(itr1.second);
                    DFAtransitions.at(itr1.first).insert(ep.begin(),ep.end());
                }else{
                    set<NFA::State*> trans = DFA::getEclosure(itr1.second);
                    DFAtransitions.insert({itr1.first,trans});
                }
            }
        }
        for(auto itr: DFAtransitions){
            if(visitedStates.count(DFA::getStateName(itr.second))==0){
                temp.first->setTransitions(visitedStates.at(DFA::getStateName(itr.second)),itr.first);
            }else if(notVisitedStates.count(DFA::getStateName(itr.second))==0){
                temp.first->setTransitions(notVisitedStates.at(DFA::getStateName(itr.second)).first,itr.first);
            }else{
                DfaState* newTran = new DfaState(id++,DFA::getStateName(itr.second));
                notVisitedStates.insert({newTran->getName(),pair<DfaState*,set<NFA::State*>>(newTran,itr.second)});
                temp.first->setTransitions(newTran,itr.first);
            }
        }
    }
    return inputState;
}

string getStateName(set<NFA::State*> epsilons){
    string name;
    for (auto itr : epsilons){
        name.append(to_string(itr->id));
    }
    return name;
}
