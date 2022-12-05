#include <iostream>
#include "NFA/NFA.h"
#include "DFA/DFA.h"
#include "LexicalAnalyzerSimulator/Simulator.h"

using namespace std;

int main()
{
    Simulator *sim = new Simulator();
    sim -> lexicalAnalyzer();

    //TEST MINIMIZATION
//    DFA *dfa = new DFA();
//    vector<DfaState*> input;
//    DfaState *a = new DfaState(0,"a");
//    DfaState *b = new DfaState(1,"b");
//    DfaState *c = new DfaState(2,"c");
//    c->setAcceptingState();
//    c->setAcceptingPattern("C");
//    DfaState *d = new DfaState(3,"d");
//    d->setAcceptingState();
//    d->setAcceptingPattern("C");
//    DfaState *e = new DfaState(4,"e");
//    e->setAcceptingState();
//    e->setAcceptingPattern("C");
//    DfaState *f = new DfaState(5,"f");
//
//    a->setTransitions(b,'0');
//    a->setTransitions(c,'1');
//    b->setTransitions(a,'0');
//    b->setTransitions(d,'1');
//    c->setTransitions(e,'0');
//    c->setTransitions(f,'1');
//    d->setTransitions(e,'0');
//    d->setTransitions(f,'1');
//    e->setTransitions(e,'0');
//    e->setTransitions(f,'1');
//    f->setTransitions(f,'0');
//    f->setTransitions(f,'1');
//
//    input.push_back(a);
//    input.push_back(b);
//    input.push_back(c);
//    input.push_back(d);
//    input.push_back(e);
//    input.push_back(f);
//
//    set<DfaState*> res = dfa->minimize(input);
//    for(auto itr:res){
//        multimap<char,DfaState*> trans = itr->getTransitions();
//        for(auto tr: trans){
//            cout << itr->getName() << ":" << tr.first << "->" << tr.second->getName() << endl;
//        }
//        cout << "----------" << endl;
//    }

    //TEST DFA
//    NFA *nfa = new NFA();
//    DFA *dfa = new DFA();
//    NFA::State* A = new NFA::State();
//    A->id=0;
//    NFA::State* B = new NFA::State();
//    B->id = 1;
//    NFA::State* C = new NFA::State();
//    C->id=2;
//    C->acceptingState=true;
//    C->acceptingPattern="C";
//
//    A->addTransition('0',B);
//    A->addTransition('0',C);
//    A->addTransition('1',A);
//    A->addEpsilonTransition(B);
//
//    B->addTransition('1',B);
//    B->addEpsilonTransition(C);
//
//    C->addTransition('0',C);
//    C->addTransition('1',C);
//
//    vector<DfaState*> conv = dfa->convertNFAtoDFA(A);
//    for(auto itr: conv){
//        multimap<char,DfaState*> trans = itr->getTransitions();
//        for(auto tr:trans){
//            cout << itr->getName() << ":" << tr.first << "->" << tr.second->getName() << endl;
//        }
//        cout << "--------" << endl;
//    }

    //TEST NFA
//    NFA *nfa = new NFA();
//    nfa->readRules();
//
//    // code to print nfa
//    NFA::FSATABLE finalNFA = nfa->NFATable;
//    cout << "Total number of states " << finalNFA.size() << endl ;
//
//    int i = 0;
//    for (i = 0; i < finalNFA.size(); i++)
//    {
//        NFA::State *temp = finalNFA.front();
//        finalNFA.pop_front();
//        finalNFA.push_back(temp);
//        temp->print();
//    }
//    cout << "Total number of states " << finalNFA.size() << endl ;
}
