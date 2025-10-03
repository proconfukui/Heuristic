#!/bin/bash

# run_and_plot.sh - mainの実行からグラフ描画まで一貫実行

set -e  # エラーで停止

echo "=== ビルド中 ==="
g++ input_problem.cpp -o ./bin/input_problem.exe -std=c++17
g++ main.cpp base.cpp evalution.cpp -O3 -o ./bin/main.exe -std=c++17
g++ print_analysis.cpp base.cpp evalution.cpp -o ./bin/print_analysis.exe -std=c++17

# 変数定義（スペースなし）
WEIGHTS_FILE=./testcase/weights.txt
WEIGHT_LINE=1
PROBLEM_FILE=./testcase/problem.json

echo "=== 実行中 ==="
echo "1. 問題入力..."
./bin/input_problem.exe "$PROBLEM_FILE" "$WEIGHTS_FILE" "$WEIGHT_LINE" > ./testcase/problem.txt

echo "2. main実行..."
./bin/main.exe < ./testcase/problem.txt > ./testcase/answer.txt

echo "3. 分析データ生成..."
./bin/print_analysis.exe "$WEIGHTS_FILE" "$WEIGHT_LINE" < ./testcase/answer.txt > ./testcase/analysis_data.txt

echo "4. グラフ生成..."
./plt_analysis_graph.sh ./testcase/analysis_data.txt "$WIEGHT_LINE"

echo "=== 完了 ==="