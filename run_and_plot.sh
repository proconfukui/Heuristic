#!/bin/bash

# run_and_plot.sh - mainの実行からグラフ描画まで一貫実行

set -e  # エラーで停止

echo "=== ビルド中 ==="
g++ input_problem.cpp -o ./bin/input_problem.exe -std=c++17
g++ main.cpp base.cpp evalution.cpp solver.cpp -O3 -o ./bin/main.exe -std=c++17
g++ print_analysis.cpp base.cpp evalution.cpp -o ./bin/print_analysis.exe -std=c++17
g++ create_answer_json.cpp base.cpp -o "./bin/create_answer_json.exe" -std=c++17

# 変数定義（スペースなし）
WEIGHTS_FILE=./testcase/weights.txt
PROBLEM_FILE=./testcase/problem.json

echo "=== 実行中 ==="
LINES=$(wc -l < "$WEIGHTS_FILE")
for ((WEIGHT_LINE=1; WEIGHT_LINE<=LINES; WEIGHT_LINE++)); do
  echo "--- WEIGHT_LINE=$WEIGHT_LINE ---"
  echo "1. 問題入力..."
  ./bin/input_problem.exe "$PROBLEM_FILE" "$WEIGHTS_FILE" "$WEIGHT_LINE" > "./testcase/problem_${WEIGHT_LINE}.txt"

  echo "2. main実行..."
  ./bin/main.exe < "./testcase/problem_${WEIGHT_LINE}.txt" > "./testcase/answer_${WEIGHT_LINE}.txt"

  echo "3. 分析データ生成..."
  ./bin/print_analysis.exe "$WEIGHTS_FILE" "$WEIGHT_LINE" < "./testcase/answer_${WEIGHT_LINE}.txt" > "./testcase/analysis_data_${WEIGHT_LINE}.txt"

  echo "4. グラフ生成..."
  ./plt_analysis_graph.sh "./testcase/analysis_data_${WEIGHT_LINE}.txt" "$WEIGHT_LINE"

  echo "5. jsonファイル生成"
  ./bin/create_answer_json.exe "./testcase/answer_${WEIGHT_LINE}.json" < "./testcase/answer_${WEIGHT_LINE}.txt"
done

echo "=== 完了 ==="