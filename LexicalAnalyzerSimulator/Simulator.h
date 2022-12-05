
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
#include <algorithm>
#include <stdbool.h>
#include <iostream>
#include "../NFA/NFA.h"
#include "../DFA/DFA.h"
#include "../DFA/DfaState.h"

using namespace std;
class Simulator
{
public:
    Simulator();
    DfaState* lexicalGenerator();
    multimap<string, string> lexicalAnalyzer();
    multimap<string, string> patternMatch(DfaState* s0);
};
