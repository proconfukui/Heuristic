#!/bin/bash

g++ input_problem.cpp -o ./bin/input_problem.exe -std=c++17
g++ main.cpp base.cpp evalution.cpp -o -O3 ./bin/main.exe -std=c++17
g++ print_analysis.cpp -o ./bin/print_analysis.exe -std=c++17

WEIGHTS_FILE = ./testcase/weights.txt
WEIGHTS_FILE_TEARGET_LINE = 1

./bin/input_problem.exe ./testcase/problem.json $WEIGHT_LINE $WEIGHTS_FILE_TEARGET_LINE
    | ./bin/main.exe 
    | ./bin/print_analysis.exe $WEIGHT_LINE $WEIGHTS_FILE_TEARGET_LINE


./plt_analysis_combined_graph