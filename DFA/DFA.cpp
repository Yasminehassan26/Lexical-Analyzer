//
// Created by blu-ray on 12/2/22.
//

#include "DFA.h"
#include "DfaState.h"
#include <limits.h>
#include <queue>
#include<algorithm>

set<NFA::State *> DFA::getEclosure(NFA::State *startState) {
    set<NFA::State *> visitedStates;
    stack<NFA::State *> states;

    states.push(startState);
    visitedStates.insert(startState);

    while (!states.empty()) {
        NFA::State *temp = states.top();
        states.pop();
        list<NFA::State*> closureStates=temp->epsilonTransitions;

        for (NFA::State* s : closureStates) {

            if (visitedStates.find(s) == visitedStates.end()) {
                visitedStates.insert(s);
                states.push(s);
            }

        }
    }
    return visitedStates;
}

void DFA::isAcceptingState(DfaState *dfaState, set<NFA::State *> nfaStates) {
    int maxPriority = INT_MAX;
    string token = "";
    for (auto itr: nfaStates) {
        if (itr->acceptingState) {
            int tempPriority = itr->patternPriority;
            if (tempPriority < maxPriority) {
                maxPriority = tempPriority;
                token = itr->acceptingPattern;
            }
        }
    }
    //after the for I have the max priority and its token

    if (!token.empty()) {
        dfaState->setAcceptingPattern(token);
        dfaState->setAcceptingState();
    }
}

vector<DfaState *> DFA::convertNFAtoDFA(NFA::State *start) {
    int id = 0;
    vector<DfaState *> dfaStates;
    set<NFA::State *> epsilon = DFA::getEclosure(start);
    DfaState *inputState = new DfaState(id++, DFA::getNfaStateName(epsilon));
    map<string, DfaState *> visitedStates;
    map<string, pair<DfaState *, set<NFA::State *>>> notVisitedStates;
    notVisitedStates.insert({inputState->getName(), pair<DfaState *, set<NFA::State *>>(inputState, epsilon)});
    while (!notVisitedStates.empty()) {
        pair<DfaState *, set<NFA::State *>> temp = notVisitedStates.begin()->second;
        visitedStates.insert(pair<string, DfaState *>(temp.first->getName(), temp.first));
        dfaStates.push_back(temp.first);
        notVisitedStates.erase(notVisitedStates.begin());
        DFA::isAcceptingState(temp.first, temp.second);
        map<char, set<NFA::State *>> DFATransitions;
        for (auto itr: temp.second) {
            multimap<char, NFA::State *> transitions = itr->transitions;
            for (auto itr1: transitions) {
                if (DFATransitions.find(itr1.first) != DFATransitions.end()) {
                    set<NFA::State *> ep = DFA::getEclosure(itr1.second);
                    DFATransitions.at(itr1.first).insert(ep.begin(), ep.end());
                } else {
                    set<NFA::State *> trans = DFA::getEclosure(itr1.second);
                    DFATransitions.insert({itr1.first, trans});
                }
            }
        }
        for (auto itr: DFATransitions) {
            if (visitedStates.count(DFA::getNfaStateName(itr.second)) != 0) {
                temp.first->setTransitions(visitedStates.at(DFA::getNfaStateName(itr.second)), itr.first);
            } else if (notVisitedStates.count(DFA::getNfaStateName(itr.second)) != 0) {
                temp.first->setTransitions(notVisitedStates.at(DFA::getNfaStateName(itr.second)).first, itr.first);
            } else {
                DfaState *newTran = new DfaState(id++, DFA::getNfaStateName(itr.second));
                notVisitedStates.insert({newTran->getName(), pair<DfaState *, set<NFA::State *>>(newTran, itr.second)});
                temp.first->setTransitions(newTran, itr.first);
            }
        }
    }
    return dfaStates;
}

string DFA::getNfaStateName(set<NFA::State *> epsilons) {
    string name;
    for (auto itr: epsilons) {
        name.append(to_string(itr->id));
    }
    return name;
}

string DFA::getDfaStateName(set<DfaState*> epsilons) {
    string name;
    for (auto itr: epsilons) {
        name.append(itr->getName());
    }
    return name;
}

set<DfaState *> DFA::minimize(vector<DfaState *> dfaStates) {
    vector<DfaState *> sortedDfaStates = dfaStates;
    std::sort(sortedDfaStates.begin(), sortedDfaStates.end());
    bool pairsArray[sortedDfaStates.size()][sortedDfaStates.size()] = { {false} };
    bool findPairs = true;

    //mark the pairs
    for (int i = 0; i < sortedDfaStates.size(); i++) {
        DfaState *rowState = sortedDfaStates.at(i);
        for (int j = 0; j < i; j++) {
            DfaState *colState = sortedDfaStates.at(j);
            if ((rowState->acceptingState && !colState->acceptingState) ||
                (!rowState->acceptingState && colState->acceptingState)) {
                pairsArray[i][j] = true;
            }
            if (rowState->acceptingState && colState->acceptingState && (rowState->getAcceptingPattern()!=colState->getAcceptingPattern())) {
                pairsArray[i][j] = true;
            }
        }
    }
    while (findPairs) {
        findPairs = false;
        for (int i = 0; i < sortedDfaStates.size(); i++) {
            DfaState *rowState = sortedDfaStates.at(i);
            multimap<char, DfaState *> firstStateTransitions = rowState->getTransitions();

            for (int j = 0; j < i; j++) {
                DfaState *colState = sortedDfaStates.at(j);
                multimap<char, DfaState *> secondStateTransitions = colState->getTransitions();
                if (!pairsArray[i][j]) {
                    if (firstStateTransitions.size() != secondStateTransitions.size()) {
                        pairsArray[i][j] = true;
                        findPairs = true;
                    } else {
                        for (auto itr = firstStateTransitions.begin(); itr != firstStateTransitions.end(); itr++) {
                            char key = itr->first;
                            if (secondStateTransitions.find(key) != secondStateTransitions.end()) {
                                int index1 = itr->second->getId();
                                int index2 = secondStateTransitions.find(key)->second->getId();
                                int row = max(index1, index2);
                                int col = min(index1, index2);
                                if (pairsArray[row][col]) {
                                    pairsArray[i][j] = true;
                                    findPairs = true;
                                }
                            } else {
                                pairsArray[i][j] = true;
                                findPairs = true;
                            }
                        }
                    }
                }
            }
        }
    }

    set<set<DfaState *>> minimizedStates;
    queue<set<DfaState *>> notMarked;
    set<DfaState*> used;
    for (int i = 0; i < sortedDfaStates.size(); i++) {
        for (int j = 0; j < i; j++) {
           if(!pairsArray[i][j]){
               set<DfaState* >temp;
               temp.insert(sortedDfaStates.at(i));
               temp.insert(sortedDfaStates.at(j));
               notMarked.push(temp);
               used.insert(sortedDfaStates.at(i));
               used.insert(sortedDfaStates.at(j));
           }
        }
    }

    for(auto itr:sortedDfaStates){
        if(used.find(itr)==used.end()){
            set<DfaState*> temp;
            temp.insert(itr);
            minimizedStates.insert(temp);
        }
    }

    while(!notMarked.empty()){
        set<DfaState*> states=notMarked.front();
        notMarked.pop();
        std::set<DfaState*> combinedStates(states);
        int size=notMarked.size();
        bool found=false;
        for(int i=0;i<size;i++){
            set<DfaState*> temp=notMarked.front() ;
            for (auto itr : temp)
            {
                if(combinedStates.find(itr)!=combinedStates.end()){
                    combinedStates.insert(temp.begin(), temp.end());
                    found=true;
                    break;
                }
            }
            notMarked.pop();
            if(!found)notMarked.push(temp);
            else found=false;
        }
        minimizedStates.insert(combinedStates);// ab cde
    }

    return minimizeHelper(minimizedStates);
}

set<DfaState *> DFA::minimizeHelper(set<set<DfaState *>> minimizedStates) {

    int ind=0;
    set<pair<set<DfaState*>,DfaState*>> combinedStates;
    for (auto itr : minimizedStates)
    {
        DfaState* newState=new DfaState(ind, getDfaStateName(itr));
        combinedStates.insert({itr,newState});
        ind++;
    }
    set<DfaState*> result;
    for (auto combinedState : combinedStates) {
        DfaState *beg = *(combinedState.first.begin());
        multimap<char, DfaState *> mainTrans = beg->getTransitions();
        for (auto itr: mainTrans) {
            for (auto loopState: combinedStates) {
                if (loopState.first.find(itr.second) != loopState.first.end()) {
                    combinedState.second->setTransitions(loopState.second, itr.first);
                    break;
                }
            }
        }
        result.insert(combinedState.second);
    }

    return result;
}








