#!/bin/bash

# analysis_data用のグラフ生成スクリプト

# 使用法チェック
if [ $# -ne 1 ]; then
    echo "使用法: $0 <analysis_data_file>"
    echo "例: $0 ./testcase/analysis_data.txt"
    exit 1
fi

ANALYSIS_FILE="$1"

# ファイル存在チェック
if [ ! -f "$ANALYSIS_FILE" ]; then
    echo "エラー: ファイルが見つかりません: $ANALYSIS_FILE"
    exit 1
fi

echo "=== Analysis Data Graph Generator ==="
echo "入力ファイル: $ANALYSIS_FILE"

# 1. データ変換プログラムをビルド
echo "1. データ変換プログラムをビルド中..."
g++ convert_analysis_data.cpp -o ./bin/convert_analysis_data.exe -std=c++17
if [ $? -ne 0 ]; then
    echo "エラー: データ変換プログラムのビルドに失敗しました"
    exit 1
fi

# 2. データを変換
echo "2. データを変換中..."
./bin/convert_analysis_data.exe "$ANALYSIS_FILE"
if [ $? -ne 0 ]; then
    echo "エラー: データ変換に失敗しました"
    exit 1
fi

# 3. pictureディレクトリを作成
mkdir -p ./picture

# 4. gnuplotでグラフを生成
echo "3. グラフを生成中..."

# 評価値グラフ
echo "  - 評価値グラフを生成中..."
gnuplot ./gb/plt_analysis_value_graph.gb
if [ $? -eq 0 ]; then
    echo "    ✓ ./picture/analysis_value_per_step.png"
else
    echo "    ✗ 評価値グラフの生成に失敗"
fi

# ペア割合グラフ
echo "  - ペア割合グラフを生成中..."
gnuplot ./gb/plt_analysis_pair_graph.gb
if [ $? -eq 0 ]; then
    echo "    ✓ ./picture/analysis_pair_per_step.png"
else
    echo "    ✗ ペア割合グラフの生成に失敗"
fi

# 複合グラフ
echo "  - 複合グラフを生成中..."
gnuplot ./gb/plt_analysis_combined_graph.gb
if [ $? -eq 0 ]; then
    echo "    ✓ ./picture/analysis_combined.png"
else
    echo "    ✗ 複合グラフの生成に失敗"
fi

echo ""
echo "=== 完了 ==="
echo "生成されたグラフ:"
ls -la ./picture/analysis_*.png 2>/dev/null || echo "  (グラフファイルが見つかりません)"

# 一時ファイルをクリーンアップ
echo ""
echo "一時ファイルをクリーンアップ中..."
rm -f ./testcase/analysis_value_data.txt ./testcase/analysis_pair_data.txt
echo "完了！"