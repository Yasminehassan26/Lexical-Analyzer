#include <iostream>
#include "NFA.cpp"

using namespace std;

int main()
{

    NFA *nfa = new NFA();
    nfa->readRules();

    // code to print nfa
    NFA::FSATABLE finalNFA = nfa->NFATable;
    cout << "Total number of states " << finalNFA.size() << endl ;

    int i = 0;
    for (i = 0; i < finalNFA.size(); i++)
    {
        NFA::State *temp = finalNFA.front();
        finalNFA.pop_front();
        finalNFA.push_back(temp);
        temp->print();
    }
    cout << "Total number of states " << finalNFA.size() << endl ;
}
