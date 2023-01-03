#include <iostream>
#include <bits/stdc++.h>
#include "Parsing.h"

 vector<string> mariem;

 void Parsing::parse(string starting,map<string,map<string,vector<string>>> table,vector<pair<string, string>> input){
    input.push_back({"$","$"});
    vector<string> states;
    states.push_back("$");
    states.push_back(starting);
    string output = "";
    string latestNT;
    while(!states.empty()){
        cout << "begin loop" << endl;
        printRes(states,output);
        cout << "-------------" << endl;
        string top = states.back();
        string pattern = input.begin()->second;
        string secondcheck = input.begin()->first;
        string chosen;
        if(top==pattern || top==secondcheck){
            top==pattern ? chosen = pattern : chosen=secondcheck;
            //top is terminal that matches the current token
            if(top=="$"){
                printRes(states,output);
                cout << "accepted" << endl;
            }else{
                output = output + " "+ chosen;
                cout <<"matched " << "'" << chosen << "'" << endl;
            }
            input.erase(input.begin());
            states.pop_back();
        }else if(table.find(top)!=table.end()){
            //top is non-terminal
            map<string,vector<string>> inputs = table.at(top);
            if(inputs.find(pattern)!=inputs.end() || inputs.find(secondcheck)!=inputs.end()){
                latestNT = top;
                inputs.find(pattern)!=inputs.end() ? chosen = pattern : chosen = secondcheck;
                vector<string> prods = inputs.at(chosen);
                if(prods.at(0)!="sync"){
                    if(prods.at(0)!="\\L"){
                        printRes(states,output);
                        states.pop_back();
                        for(int i=prods.size()-1; i>=0; i--){
                            if(prods[i].at(0)=='\''){
                                prods[i] = prods[i].substr(1,prods[i].length()-2);
                            }
                            states.push_back(prods[i]);
                        }
                    }else{
                        printRes(states,output);
                        cout << top << "->" << "epsilon" << endl;
                        states.pop_back();
                    }
                }else{
                    printRes(states,output);
                    cout <<" error: missing " << top << endl;
                    states.pop_back();
                }
            }else{
                cout <<"error: discard " << pattern << endl;
                input.erase(input.begin());
            }
        }else{
            //top is terminal but doesn't match current token
            if(top=="$"){
                cout << "not accepted" << endl;
                states.pop_back();
            }else{
                map<string,vector<string>> inputs = table.at(latestNT);
                if(inputs.find(top)==inputs.end()){
                    cout << "error: discard " << pattern << endl;
                    input.erase(input.begin());
                }else if(inputs.at(top).at(0)=="sync"){
                    printRes(states,output);
                    cout <<" error: missing " << top << endl;
                    states.pop_back();
                }
            }
        }
    }
    cout << "---------------------------------------" << endl;
    for(string p:mariem){
        cout << p << endl;
    }
}

void Parsing::printRes(vector<string> stack, string matched){
    string res = matched;
    string st = "";
    for(int i=stack.size()-1; i>0; i--){
        res = res+ " " + stack.at(i);
        st = st + stack.at(i) + " ";
    }
    cout << "hi " << res << endl;
    cout << "stack: " << st << endl;
    mariem.push_back(res);
}