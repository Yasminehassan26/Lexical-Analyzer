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
	if(chInput == "\\L")
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

	// From A last to A first state
	A[A.size()-1]->addEpsilonTransition(A[0]);

	// Push the result onto the stack
	operandStack.push(A);

	return true;
};

string NFA:: keywordsHandler (string regular){
	if(regular[0] == '{' || regular[0] == '['){
        //remove brackets and spaces
		regular = regular.substr(1,regular.size()-2);
		if(regular[0]==' ') regular=regular.substr(1);
		if(regular[regular.size()-1]==' ')regular=regular.substr(0,regular.size()-1);

		int i = 0 ;
		while(i<regular.size()){
            string temp = "";
            while(regular[i] != ' ' && i<regular.size()){
                temp += regular[i];
                i++;
            }
            i++;
            string name = temp;
            if (name[0] == '\\') name = name.substr(1);
            string keyword = name + ":" + temp;
            getTokens(keyword,':',0);
		}

        return "";
	}

	return regular;
};

vector<string> NFA:: preprocessing(string regular)
{
// for punct and keywords to follow RE rules

//for splitting on space , and add concate
    vector<string> tokens ;
	string temp;
    int i = 0;

	while(i < regular.size()){
	if(regular[i] == ' ' ){
		i++;
	}
	if(isBackSlash(string(1,regular[i]))){
		if(isReserved(string(1,regular[i+1]))){
			string token = regular.substr(i,2);
			tokens.push_back(token);
			i+=2;
		}
	}
	if(isOperator(string(1, regular[i]))) {

		string token ;
		if(regular[i]=='.'){
			 token = string(1,'\\') + regular[i];
		}else{
			 token =  string(1,regular[i]);
		}

		tokens.push_back(token);
		i++;
		}
     while(!isOperator(string(1, regular[i])) && regular[i] != ' '&& i< regular.size()&& !(isBackSlash(string(1,regular[i]))&&isReserved(string(1,regular[i+1])))) {
		temp += regular[i];
		i++;
	 }
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
	 	if(temp.size()==1) {

		 tokens.push_back(temp);
		 temp = "";
		}

	};
	vector <string> preprocessedRE;

	for(int i=0; i<tokens.size()-1; i++)
	{

		string cLeft = tokens[i];
		string cRight = tokens[i+1];
		preprocessedRE.push_back(cLeft) ;

		if((isInput(cLeft)) || (isRightParanthesis(cLeft)) || (cLeft=="*") ||(cLeft=="+")){
				if((isInput(cRight)) || (isLeftParanthesis(cRight)))
			    	preprocessedRE.push_back(".");
		}

	}
	preprocessedRE.push_back(tokens[tokens.size()-1]) ;

	return preprocessedRE;

}


bool NFA:: readRules(){

    fstream newfile;
    newfile.open("C:\\Users\\Carnival\\CLionProjects\\Lexical-Analyzer\\input.txt",ios_base::in);
    int priority = 1;

   if (newfile.is_open()){
      string regular;
      while(getline(newfile, regular)){
		  regular = keywordsHandler(regular);
          if(regular.size()!= 0){
			if (regular.find(':') == string::npos){
                 getTokens(regular,'=',-1);
			}else{
                getTokens(regular,':',priority++);
			}
      }}
      newfile.close(); //close the file object.
   }

   return createFinalNFA();
};

void NFA :: getTokens(string line,char spliter,int priority){
    string name = line.substr(0, line.find(spliter));
    string RE = line.substr(line.find(spliter)+1);
    if(name[name.size()-1]==' ')
        name = name.substr(0,name.size()-1);
    vector<string> tokens = preprocessing(RE);
    if(spliter == '='){
        definitions.insert(make_pair(name,createNFA(tokens,name,false,priority)));
    }else if(spliter == ':'){
        expressions.insert(make_pair(name,createNFA(tokens,name,true,priority)));
    }
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

NFA:: FSATABLE NFA:: createNFA(vector<string> tokens , string patternMatch ,bool isExpr,int priority){

	for(int i=0; i<tokens.size(); i++)
	{
		// get the charcter
		string token = tokens[i];
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
			// Remove left paranthesis after the evaluation
			operatorStack.pop();
		}
		else
		{
			while(!operatorStack.empty() && presedence(token[0], operatorStack.top()))
				Eval();
			operatorStack.push(token[0]);
		}
	}

	// Evaluate the rest of operators
	while(!operatorStack.empty())
        Eval();

	// Pop the result from the stack
	FSATABLE NFATable;
	pop(NFATable);

	// Last NFA state is always accepting state
	if(isExpr){
	NFATable[NFATable.size()-1]->acceptingState = true;
	NFATable[NFATable.size()-1]->acceptingPattern = patternMatch ;
	NFATable[NFATable.size()-1]->patternPriority = priority ;
	}

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
