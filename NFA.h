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

				// Set of NFA state from which this state is constructed
				set<State*> parentStates;

				// True if this state is accepting state (final state)
				bool acceptingState;

				string acceptingPattern;

				//! Default constructor
				State() :acceptingState(false) {

				};

				//! Constructs new state from the set of other states
				/*! This is necessary for subset construction algorithm
					because there a new DFA state is constructed from
					one or more NFA states
				*/
				State( set<State*> NFAState)
				{
					parentStates	= NFAState;


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

				void addParent(State *pState){
					parentStates.insert(pState);
				}

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
				    //cout << visited.size()<< "visited size" << endl;
				   multimap<State*,State*>::iterator it;
//					for ( it = visited.begin(); it != visited.end(); it++){
//					    cout << it->first << " "<< it->second << endl;
//					}
					//cout << "end of for loop _______________________________________" <<endl;
					State* cloned = new State();
                    if(visited.find(this) == visited.end())
                      visited.insert({this, cloned});
                    else return visited.find(this)->second;

					//cout << this <<" " << cloned << endl ;
					cloned->acceptingState = acceptingState;
					cloned->acceptingPattern = acceptingPattern;
					// coping transitions
					multimap<char,State*>::iterator itr;
					for ( itr = transitions.begin(); itr != transitions.end(); itr++){
                        // cout << itr->second << " key for search" <<endl;
						if(visited.find((itr->second)) != visited.end()){

                                //cout << "found in visited" << endl ;
							cloned->addTransition(itr->first, visited.find(itr->second)->second);
						}else{
						    //cout << "not found in transitions" << endl;
							cloned->addTransition(itr->first, (itr->second)->clone(visited));
					}}
					//cout << "finished coping transitions" << endl;

					// coping epsilon transitions

					list<State*>::iterator itr1;
					for(itr1 = epsilonTransitions.begin(); itr1 != epsilonTransitions.end(); itr1++){
                           // cout << (*itr1) << " key for search epsilon" <<endl;
						if(visited.find((*itr1)) != visited.end()){
                            //cout << "found in visited" << endl ;
							cloned->addEpsilonTransition(visited.find((*itr1))->second);
						}else{
						    //cout << "not found in epsilon" << endl;
							cloned->addEpsilonTransition((*itr1)->clone(visited));
					}}
					// coping parent states
					/*set<State*>::iterator iter;
					for(iter=parentStates.begin(); iter!=parentStates.end(); iter++){
						if(visited.find((*iter)) != visited.end())
							cloned->addParent(visited.find((*iter))->second);
						else
							cloned->addParent((*iter)->clone(visited));
					}*/

					return cloned;
				};

				void print(){
					// print state identifier
					cout << "----------------------------------------------------" << endl;
					cout << "State name: " << id <<"\t ";



					// print boolean is final
					cout << "Is Final: " << acceptingState ;

					// print accepting pattern
					cout << "  Accepting pattern: " << acceptingPattern << endl;

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

					// // print parent states
					// cout << "Parent states: " << endl << "-----------------------------------------------------------------" << endl;
					// set<State*>::iterator iter;
					// for(iter=parentStates.begin(); iter!=parentStates.end(); iter++)
					// 	cout << (*iter) << endl;

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
		/*! If we use the Thompson's Algorithm then we realize
			that each operand is a NFA automata on its own!
		*/
		stack<FSATABLE> operandStack;

		//! Operator Stack
		// Operators are simple characters like "*" & "|" & "+" & "-" & "(" & ")"
		stack<char> operatorStack;

		//! Set of language alphabet
		set<char> alphabet;

		multimap <string,FSATABLE> definitions;
		multimap <string,FSATABLE> expressions;

		void push(string chInput);

		//! Pops an element from the operand stack
		/*! The return value is TRUE if an element
			was poped successfully, otherwise it is
			FALSE (syntax error)
		*/
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

		// returns true if opRight highest priority
		bool presedence(char opLeft, char opRight){
			int left = computePriority(opLeft);
			int right = computePriority(opRight);

			return right >= left;
		};

		//! Checks if the specific character is input character
		bool isInput(string ch) {
			return(!isOperator(ch));
		};

		//! Checks is a character left parantheses
		bool isLeftParanthesis(string ch) {
			if(ch.size()>1)
				return false;
			return(ch[0] == 40);
		};

		//! Checks is a character right parantheses
		bool isRightParanthesis(string ch) {
			if(ch.size()>1)
				return false;
			return(ch[0] == 41);
		};

		//! Evaluates the next operator from the operator stack
		bool eval();

		//! Evaluates the concatenation operator
		/*! This function pops two operands from the stack
			and evaluates the concatenation on them, pushing
			the result back on the stack.
		*/
		bool OR();

		bool concat();

		bool range();
		//! Evaluates the Kleen's closure - star operator
		/*! Pops one operator from the stack and evaluates
			the star operator on it. It pushes the result
			on the operand stack again.
		*/
		bool star();

		//! Evaluates the union operator
		/*! Pops 2 operands from the stack and evaluates
			the union operator pushing the result on the
			operand stack.
		*/
		bool plus();

		//! Inserts char 8 where the concatenation needs to occur
		/*! The method used to parse regular expression here is
			similar to method of evaluating the arithmetic expressions.
			A difference here is that each arithmetic expression is
			denoted by a sign. In regular expressions the concatenation
			is not denoted by ny sign so I will detect concatenation
			and insert a character 8 between operands.
		*/
		string  reconstructRE (string regular);
		vector<string> preprocessing(string strRegEx);

		//! Creates Nondeterministic Finite Automata from a Regular Expression
		FSATABLE createNFA(vector<string> tokens,string name);
		bool readRules();
		bool Eval();
		bool createFinalNFA();
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
