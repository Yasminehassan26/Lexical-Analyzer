cmake_minimum_required(VERSION 3.24)
project(Lexical_Analyzer)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lexical_Analyzer main.cpp DFA/DfaState.cpp DFA/DfaState.h DFA/DFA.cpp DFA/DFA.h NFA/NFA.cpp NFA/NFA.h LexicalAnalyzerSimulator/Simulator.cpp LexicalAnalyzerSimulator/Simulator.h Parsing.h Parsing.cpp table.cpp table.h)