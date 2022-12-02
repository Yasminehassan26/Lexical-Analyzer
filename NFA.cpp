#include "NFA.h"
#include <fstream>
#include <sstream>
#include <string>
#include <stdbool.h>
#include <iostream>
#include <algorithm>
#include<vector>

using namespace std;


NFA::NFA(){}


void NFA::push(string chInput)
{
	// Create 2 new states on the heap
	State *s0 = new State();
	State *s1 = new State();

	// Add the transition from s0->s1 on input character
	if(chInput == "\L")
	  s0->addEpsilonTransition(s1);
	// add /reserved
	else if (chInput[0] == '\\' && chInput.size()>1){
      s0->addTransition(chInput[1],s1);
	  // Add this character to the input character set
	  alphabet.insert(chInput[1]);
	}
	else{
	   s0->addTransition(chInput[0], s1);
	   // Add this character to the input character set
	   alphabet.insert(chInput[0]);
	}

	// Create a NFA from these 2 states
	FSATABLE NFATable;
	NFATable.push_back(s0);
	NFATable.push_back(s1);

	// push it onto the operand stack
	operandStack.push(NFATable);

}

bool NFA::pop(FSATABLE &NFATable)
{
	// If the stack is empty we cannot pop anything
	if(operandStack.size()>0)
	{
		NFATable = operandStack.top();
		operandStack.pop();
		return true;
	}

	return false;
}


// operator methods
bool NFA::OR (){
    // Pop 2 elements
	FSATABLE A, B;
	if(!pop(B) || !pop(A))
		return false;


	State *pStartState	= new State();
	State *pEndState	= new State();
	pStartState->addEpsilonTransition(A[0]);
	pStartState->addEpsilonTransition(B[0]);
	A[A.size()-1]->addEpsilonTransition(pEndState);
	B[B.size()-1]->addEpsilonTransition(pEndState);

	// Create new NFA from A
	B.push_back(pEndState);
	A.push_front(pStartState);
	A.insert(A.end(), B.begin(), B.end());

	// Push the result onto the stack
	operandStack.push(A);

	return true;
}

bool NFA::concat()
{
	// Pop 2 elements
	FSATABLE A, B;
	if(!pop(B) || !pop(A))
		return false;


	A[A.size()-1]->addEpsilonTransition(B[0]);
	A.insert(A.end(), B.begin(), B.end());

	// Push the result onto the stack
	operandStack.push(A);
	return true;
}

bool NFA::range(){
    // Pop 2 elements
	FSATABLE A, B;
	if(!pop(B) || !pop(A))
		return false;

    char rangeStart = ((A.front())->transitions.begin())->first;
    char rangeEnd = ((B.front())->transitions.begin())->first;
     for(int i = rangeStart; i < rangeEnd; i++){
		push(string(1,i));
		operatorStack.push('|');
	 }
    push(string(1,rangeEnd));

}

bool NFA::star()
{
	// Pop 1 element
	FSATABLE A;
	if(!pop(A))
		return false;

	State *pStartState	= new State();
	State *pEndState	= new State();

	pStartState->addEpsilonTransition(pEndState);

	// add epsilon transition from start state to the first state of A
	pStartState->addEpsilonTransition( A[0]);

	// add epsilon transition from A last state to end state
	A[A.size()-1]->addEpsilonTransition( pEndState);

	// From A last to A first state
	A[A.size()-1]->addEpsilonTransition(A[0]);

	// construct new NFA and store it onto the stack
	A.push_back(pEndState);
	A.push_front(pStartState);

	// Push the result onto the stack
	operandStack.push(A);

	return true;
}

bool NFA::plus()
{
	FSATABLE A;
	if(!pop(A))
		return false;


	State *pStartState	= new State();
	State *pEndState	= new State();


	// add epsilon transition from start state to the first state of A
	pStartState->addEpsilonTransition( A[0]);

	// add epsilon transition from A last state to end state
	A[A.size()-1]->addEpsilonTransition( pEndState);

	// From A last to A first state
	A[A.size()-1]->addEpsilonTransition(A[0]);

	// construct new NFA and store it onto the stack
	A.push_back(pEndState);
	A.push_front(pStartState);

	// Push the result onto the stack
	operandStack.push(A);

	return true;
};

string NFA:: reconstructRE (string regular){
	if(regular[0] == '{' ){
		regular = regular.substr(1,regular.size()-2);
		if(regular[0]==' ') regular=regular.substr(1);
		if(regular[regular.size()-1]==' ')regular=regular.substr(0,regular.size()-1);
		replace( regular.begin(), regular.end(), ' ', '|');
		regular =  "keyWords:" + regular;
	}else if(regular[0] == '['){
		regular = regular.substr(1,regular.size()-2);
		cout <<regular[0]<< int(regular[0])<<" "<<regular[3]<< int(regular[3])<< endl;
		if(regular[0]==' ') regular=regular.substr(1);
		if(regular[regular.size()-1]==' ')regular=regular.substr(0,regular.size()-1);
		cout << regular << endl;
		replace( regular.begin(), regular.end(), ' ', '|');
		cout << regular << endl;
		regular =  "punc:" + regular;
	}
	return regular;
};

vector<string> NFA:: preprocessing(string regular)
{
// for punct and keywords to follow RE rules

//for splitting on space , and add concate
//letterdigit*digit
    vector<string> tokens ;
	string temp;
    int i = 0;

	while(i < regular.size()){
	if(regular[i] == ' ' ){
	//	cout << regular[i] << "space" <<endl ;
		i++;
	}
	if(isBackSlash(string(1,regular[i]))){
	//	cout << "back slash" <<endl ;
		if(isReserved(string(1,regular[i+1]))){
		//	cout << "reserved" << endl;
			string token = regular.substr(i,2);
			tokens.push_back(token);
			i+=2;
		}
	}
	if(isOperator(string(1, regular[i]))) {
       //cout << regular[i] << "Operator" <<endl ;

		string token ;
		if(regular[i]=='.'){
			 token = string(1,'\\') + regular[i];
		}else{
			 token =  string(1,regular[i]);
		}

		tokens.push_back(token);
		i++;
		}
     //cout << i << endl ;
     while(!isOperator(string(1, regular[i])) && regular[i] != ' '&& i< regular.size()&& !(isBackSlash(string(1,regular[i]))&&isReserved(string(1,regular[i+1])))) {
		temp += regular[i];
		i++;
	 }
   // cout << temp << endl;
	 while(temp.size()>1){
		//found
		if(definitions.find(temp) != definitions.end()){
		 tokens.push_back(temp);
		 temp = "";
		}else{
			temp.resize (temp.size () - 1);
			i--;
		}
	 }
        //cout << "out of second loop" << endl;
	 	if(temp.size()==1) {
	 		//cout << temp << "singleChar"<< int(temp[0]) <<endl ;

		 tokens.push_back(temp);
		 temp = "";
		}

	};
	vector <string> preprocessedRE;
	//cout << tokens.size() <<endl;

	for(int i=0; i<tokens.size()-1; i++)
	{

		string cLeft = tokens[i];
		string cRight = tokens[i+1];
	//	cout << cLeft << endl ;
		preprocessedRE.push_back(cLeft) ;

		if((isInput(cLeft)) || (isRightParanthesis(cLeft)) || (cLeft=="*") ||(cLeft=="+")){
				if((isInput(cRight)) || (isLeftParanthesis(cRight)))
			    	//preprocessedRE += ". ";
			    	preprocessedRE.push_back(".");
		}

		//cout << preprocessedRE;
	}
	preprocessedRE.push_back(tokens[tokens.size()-1]) ;

    //cout << preprocessedRE << endl ;
    int k = 0 ;
    for(k = 0 ; k<preprocessedRE.size();k++){
    	cout << preprocessedRE[k] << " ";
	}
	cout << endl;

	return preprocessedRE;

}
bool NFA:: readRules(){

    fstream newfile;
    newfile.open("input.txt",ios::in);

   if (newfile.is_open()){
      string regular;
      while(getline(newfile, regular)){
		  regular = reconstructRE(regular);
		  //cout << regular << endl;
			if (regular.find(':') == string::npos){
				string name = regular.substr(0, regular.find('='));
				string RE = regular.substr(regular.find('=')+1);
				if(name[name.size()-1]==' ')
				  name = name.substr(0,name.size()-1);
                cout  << "definition " << name <<"\n";
				vector<string> tokens = preprocessing(RE);

				//cout << RE<< "\n";
				definitions.insert(make_pair(name,createNFA(tokens,name)));
			}else{
				string name = regular.substr(0, regular.find(':'));
				string RE = regular.substr(regular.find(':')+1);
				if(name[name.size()-1]==' ')
				  name = name.substr(0,name.size()-1);
			//	if(name != "keyWords" && name != "punc")
			     cout  << "expression " << name <<"\n";
				 vector<string> tokens = preprocessing(RE);

				//cout << RE<< "\n";
				expressions.insert(make_pair(name,createNFA(tokens,name)));
			}
      }
      newfile.close(); //close the file object.
   }
   //cout << "out of create" << endl;
     /* FSATABLE finalNFA = expressions.find("letter")->second;
    int i = 0;
    for (i = 0; i < finalNFA.size(); i++)
    {
        NFA::State *temp = finalNFA.front();
        finalNFA.pop_front();
        finalNFA.push_back(temp);
        temp->print();
    }*/

   return createFinalNFA();
};

bool NFA:: createFinalNFA(){
    multimap<string,FSATABLE>::iterator itr;
    for ( itr = expressions.begin(); itr != expressions.end(); itr++){
		operandStack.push(clone(itr->second));
	}

    State* s0 = new State();
    NFATable.push_back(s0);
    while(!operandStack.empty()){
        FSATABLE currentNFA = operandStack.top();
        s0->addEpsilonTransition(currentNFA[0]);
        NFATable.insert(NFATable.end(),currentNFA.begin(),currentNFA.end());
        operandStack.pop();
    }

    assignID();
	return true;

};

NFA:: FSATABLE NFA:: createNFA(vector<string> tokens , string patternMatch){
//cout << tokens.size() << endl;

	for(int i=0; i<tokens.size(); i++)
	{
		// get the charcter
		string token = tokens[i];
		//cout << token << endl;
		//found
        if(definitions.find(token)!= definitions.end()){
         operandStack.push(clone(definitions.find(token)->second));
		}
		// not found single char or /reserved
		else if(isInput(token) || (isBackSlash(string(1,token[0]))&& isReserved(string(1,token[1])))){
			push(token);
		}
		else if(operatorStack.empty() || isLeftParanthesis(token)){
           operatorStack.push(token[0]);
		}
		else if(isRightParanthesis(token))
		{
			// Evaluate everyting in paranthesis
			while(!isLeftParanthesis(string(1,operatorStack.top())))
                Eval();
			/*	if(!Eval())
					return ;*/
			// Remove left paranthesis after the evaluation
			operatorStack.pop();
		}
		else
		{
			while(!operatorStack.empty() && presedence(token[0], operatorStack.top()))
				Eval();
				/*if(!Eval())
					return nullptr;*/
			operatorStack.push(token[0]);
		}
	}

	// Evaluate the rest of operators
	while(!operatorStack.empty())
        Eval();
		/*if(!Eval())
			return nullptr;*/

	// Pop the result from the stack
	FSATABLE NFATable;
	pop(NFATable);
	//if(!pop(NFATable))
		//return nullptr;

	// Last NFA state is always accepting state
	NFATable[NFATable.size()-1]->acceptingState = true;
	NFATable[NFATable.size()-1]->acceptingPattern = patternMatch ;
	//expressions.insert(make_pair(patternMatch,NFATable));

	return NFATable;
}
bool NFA::Eval()
{
	// First pop the operator from the stack
	if(operatorStack.size()>0)
	{
		char chOperator = operatorStack.top();
		operatorStack.pop();

		// Check which operator it is
		switch(chOperator)
		{
		case  42:
			return star();
			break;
		case 124:
			return OR();
			break;
		case   46:
			return concat();
			break;
		case 43:
			return plus();
			break;
		case 45:
			return range();
			break;
		}

		return false;
	}

	return false;
}
