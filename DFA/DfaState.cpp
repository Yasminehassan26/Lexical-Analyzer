//
// Created by blu-ray on 12/2/22.
//

#include "DfaState.h"

DfaState::DfaState() {}
DfaState::DfaState(int id,string name) {
    this->id=id;
    this->name=name;
}

int DfaState::getId()
{
    return id;
}
string DfaState::getName()
{
    return name;
}
bool DfaState::getIsAcceptingState()
{
    return acceptingState;
}
string DfaState::getAcceptingPattern()
{
    return acceptingPattern;
}
void DfaState::setAcceptingState()
{
    this->acceptingState=true;
}
void DfaState::setAcceptingPattern(string pattern)
{
    this->acceptingPattern=pattern;
}
void DfaState::setTransitions(DfaState * state, char target) {
    this->transitions.insert(make_pair(target,state));
}
multimap<char, DfaState*> DfaState::getTransitions()
{
    return transitions;
}
DfaState* DfaState::getTransition(char target)
{
    return transitions.find(target)->second;
}


