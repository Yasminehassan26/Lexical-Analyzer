cmake_minimum_required(VERSION 3.24)
project(Lexical_Analyzer)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lexical_Analyzer main.cpp DFA/DfaState.cpp DFA/DfaState.h DFA/DFA.cpp DFA/DFA.h Minimization/DfaMinimization.cpp Minimization/DfaMinimization.h Minimization/DfaMinimization.h DFA/DfaMinimization.cpp)
e