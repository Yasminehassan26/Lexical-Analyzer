#pragma once

#include <string>
#include <stack>
#include <map>
#include <vector>
#include <stdbool.h>

using namespace std;

class Parsing {

public:
    void parse(string,map<string,map<string,vector<string>>>, vector<pair<string, string>>);
    void printRes(vector<string>, string);
};
