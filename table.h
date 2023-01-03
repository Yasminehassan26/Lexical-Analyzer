//
// Created by Carnival on 1/3/2023.
//

#include <string>
#include <set>
#include <vector>
#include <map>

using namespace std;

class table {
public:
    set<string> getFirst(vector<string>, set<string>, map<string, set<string>>);
    map<string, map<string,vector<string>>> getParsingTable(map<string,vector<vector<string>>>,map<string,set<string>>, map<string,set<string>>);
    void printTable(map<string, map<string, vector<string>>> parsingTable);
};


