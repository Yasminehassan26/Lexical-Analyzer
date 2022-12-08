#pragma once
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include <string>
#include <stack>
#include <map>
#include <deque>
#include <vector>
#include <set>
#include <algorithm>
#include <list>
#include <stdbool.h>
#include <iostream>

using namespace std;
class NFA{
	public:
		class State{
			public:
			    int id ;
				// Transitions from this state to other
				multimap<char, State*> transitions;

				// list of states that this state goes to with epsilon transition
				list<State*> epsilonTransitions;

				// True if this state is accepting state (final state)
				bool acceptingState = false;

				string acceptingPattern = "" ;

                int patternPriority = -1;

				//! Default constructor
				State() :acceptingState(false) {

				};

				State( set<State*> NFAState)
				{

					// DFA state is accepting state if it is constructed from
					// an accepting NFA state
					acceptingState	= false;
					set<State*>::iterator iter;
					for(iter=NFAState.begin(); iter!=NFAState.end(); ++iter)
						if((*iter)->acceptingState){
							acceptingState = true;
							acceptingPattern = (*iter)->acceptingPattern;
						}
				};

				//! Adds a transition from this state to the other
				void addTransition(char chInput, State *pState){
					if(pState != NULL)
						transitions.insert(make_pair(chInput, pState));
				};

				void addEpsilonTransition(State *pState){
					if(pState != NULL)
						epsilonTransitions.push_back(pState);
				};

				State* clone(multimap<State*, State*> &visited){

				   multimap<State*,State*>::iterator it;

					State* cloned = new State();
                    if(visited.find(this) == visited.end())
                      visited.insert({this, cloned});
                    else return visited.find(this)->second;

					cloned->acceptingState = acceptingState;
					cloned->acceptingPattern = acceptingPattern;
					cloned->patternPriority = patternPriority;
					// coping transitions
					multimap<char,State*>::iterator itr;
					for ( itr = transitions.begin(); itr != transitions.end(); itr++){
						if(visited.find((itr->second)) != visited.end()){

							cloned->addTransition(itr->first, visited.find(itr->second)->second);
						}else{
							cloned->addTransition(itr->first, (itr->second)->clone(visited));
					}}

					// coping epsilon transitions
					list<State*>::iterator itr1;
					for(itr1 = epsilonTransitions.begin(); itr1 != epsilonTransitions.end(); itr1++){
						if(visited.find((*itr1)) != visited.end()){
							cloned->addEpsilonTransition(visited.find((*itr1))->second);
						}else{
							cloned->addEpsilonTransition((*itr1)->clone(visited));
					}}

					return cloned;
				};

				void print(){
					// print state identifier
					cout << "----------------------------------------------------" << endl;
					cout << "State name: " << id <<"\t ";

					// print boolean is final
					cout << "Is Final: " << acceptingState ;
					// print accepting pattern
					cout << "  Accepting pattern: " << acceptingPattern << " priority level : " << patternPriority << endl;

					// print transitions
					cout << "Transitions: " << "\t";
					multimap<char,State*>::iterator itr;
					for ( itr = transitions.begin(); itr != transitions.end(); itr++)
						cout << "( " << itr->first << " -> " << itr->second->id <<") ";

					// print epsilon transitions
					cout <<endl << "Epsilon Transitions: " << "\t" ;
					list<State*>::iterator itr1;
					for(itr1 = epsilonTransitions.begin(); itr1 != epsilonTransitions.end(); itr1++)
						cout << "(epsilon -> " << (*itr1)->id << ") ";

					cout << endl;
				}
		};

	// constructor

		NFA();
		typedef deque<State*> FSATABLE;



		FSATABLE NFATable;

		FSATABLE clone(FSATABLE obj){
			FSATABLE cloned ;
			int i = 0 ;
			multimap<State*, State*> visited;
			for (i = 0 ;i<obj.size();i++){
				State* temp = obj.front();
				obj.pop_front();
				obj.push_back(temp);
				cloned.push_back(temp->clone(visited));
			}
			return cloned;
		}

		//! Operand Stack
		stack<FSATABLE> operandStack;

		//! Operator Stack
		// Operators are simple characters like "*" & "|" & "+" & "-" & "(" & ")"
		stack<char> operatorStack;

		//! Set of language alphabet
		set<char> alphabet;

		multimap <string,FSATABLE> definitions;
		multimap <string,FSATABLE> expressions;

		void push(string chInput);


		bool pop(FSATABLE &NFATable);

		//! Checks is a specific character and operator
		//                                       \            *               |              (             )             -             +
		bool isOperator(string ch) {
			if(ch.size()>1)
				return false;
			return((ch[0] == 46) || (ch[0] == 42) || (ch[0] == 124) || (ch[0] == 40) || (ch[0] == 41) || (ch[0] == 45) || (ch[0] == 43));
		};

		bool isReserved (string ch){
			if(ch.size()>1)
				return false;
			return((ch[0] == 46) || (ch[0] == 42) || (ch[0] == 124) || (ch[0] == 40) || (ch[0] == 41) || (ch[0] == 45) || (ch[0] == 43) || (ch[0]== 61) ||(ch[0]==76));
		};

		bool isBackSlash(string ch){
			if(ch.size()>1)
				return false;
			return(ch[0] == 92) ;
		};

		int computePriority(char op) {
			if(op == '*' || op == '+' || op == '-')
				return 3;
			else if (op == '.')
				return 2;
			else if (op == '|')
				return 1;
			return 0;
		};

		bool presedence(char opLeft, char opRight){
			int left = computePriority(opLeft);
			int right = computePriority(opRight);

			return right >= left;
		};

		bool isInput(string ch) {
			return(!isOperator(ch));
		};

		bool isLeftParanthesis(string ch) {
			if(ch.size()>1)
				return false;
			return(ch[0] == 40);
		};

		bool isRightParanthesis(string ch) {
			if(ch.size()>1)
				return false;
			return(ch[0] == 41);
		};

		bool eval();

		bool OR();

		bool concat();

		bool range();

		bool star();
		bool plus();


		string keywordsHandler(string regular);
		vector<string> preprocessing(string strRegEx);

		FSATABLE createNFA(vector<string> tokens,string name ,bool isExpr,int priority);
		bool readRules();
		bool Eval();
		bool createFinalNFA();
		void getTokens(string line,char spliter,int priority);
		void assignID(){
			int i = 0 ;
			for (i = 0 ;i<NFATable.size();i++){
				NFA::State* temp = NFATable.front();
				NFATable.pop_front();
				temp->id = i;
				NFATable.push_back(temp);
			}
		}
	};
