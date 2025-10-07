#!/bin/bash

# analysis_data用のグラフ生成スクリプト

# 使用法チェック
if [ $# -lt 1 ] || [ $# -gt 2 ]; then
    echo "使用法: $0 <analysis_data_file> [weight_line_number]"
    echo "例: $0 ./testcase/analysis_data.txt"
    echo "例: $0 ./testcase/analysis_data.txt 2"
    echo "weight_line_numberを省略した場合は1行目の重みを使用します"
    exit 1
fi

ANALYSIS_FILE="$1"
WEIGHT_LINE_NUM="${2:-1}"  # 第2引数が指定されていない場合は1を使用

# ファイル存在チェック
if [ ! -f "$ANALYSIS_FILE" ]; then
    echo "エラー: ファイルが見つかりません: $ANALYSIS_FILE"
    exit 1
fi

echo "=== Analysis Data Graph Generator ==="
echo "入力ファイル: $ANALYSIS_FILE"
echo "使用する重みファイルの行番号: $WEIGHT_LINE_NUM"

# 重みの情報を取得
WEIGHTS_FILE="./testcase/weights.txt"
WEIGHT_SUFFIX=""
if [ -f "$WEIGHTS_FILE" ]; then
    # weights.txtから指定された行の重みの情報を読み取り
    TOTAL_LINES=$(wc -l < "$WEIGHTS_FILE") + 1
    if [ "$WEIGHT_LINE_NUM" -gt "$TOTAL_LINES" ]; then
        echo "エラー: 指定された行番号($WEIGHT_LINE_NUM)が重みファイルの行数($TOTAL_LINES)を超えています"
        exit 1
    fi
    
    WEIGHT_LINE=$(sed -n "${WEIGHT_LINE_NUM}p" "$WEIGHTS_FILE")
    
    # 重みの数を動的に取得し、すべての重みをファイル名に含める
    WEIGHTS_ARRAY=($WEIGHT_LINE)  # スペース区切りで配列に変換
    WEIGHT_COUNT=${#WEIGHTS_ARRAY[@]}
    
    # 重み情報を文字列として構築
    WEIGHT_INFO=""
    WEIGHT_SUFFIX_PARTS=""
    for i in "${!WEIGHTS_ARRAY[@]}"; do
        if [ $i -eq 0 ]; then
            WEIGHT_INFO="${WEIGHTS_ARRAY[$i]}"
            WEIGHT_SUFFIX_PARTS="w${WEIGHTS_ARRAY[$i]}"
        else
            WEIGHT_INFO="${WEIGHT_INFO}, ${WEIGHTS_ARRAY[$i]}"
            WEIGHT_SUFFIX_PARTS="${WEIGHT_SUFFIX_PARTS}_${WEIGHTS_ARRAY[$i]}"
        fi
    done
    
    WEIGHT_SUFFIX="_${WEIGHT_SUFFIX_PARTS}_line${WEIGHT_LINE_NUM}"
    echo "重み情報 (${WEIGHT_LINE_NUM}行目, ${WEIGHT_COUNT}個): $WEIGHT_INFO"
else
    echo "警告: weights.txtが見つかりません。重み情報なしでグラフを生成します。"
fi

# # 1. データ変換プログラムをビルド
# echo "1. データ変換プログラムをビルド中..."
# g++ convert_analysis_data.cpp -o ./bin/convert_analysis_data.exe -std=c++17
# if [ $? -ne 0 ]; then
#     echo "エラー: データ変換プログラムのビルドに失敗しました"
#     exit 1
# fi

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

# 一時的なgnuplotスクリプトを生成して重み情報を含むファイル名にする
TEMP_GB_DIR="./temp_gb"
mkdir -p "$TEMP_GB_DIR"

# # 評価値グラフ
# echo "  - 評価値グラフを生成中..."
# sed "s|analysis_value_per_step\.png|analysis_value_per_step${WEIGHT_SUFFIX}.png|g" ./gb/plt_analysis_value_graph.gb > "$TEMP_GB_DIR/plt_analysis_value_graph_temp.gb"
# gnuplot "$TEMP_GB_DIR/plt_analysis_value_graph_temp.gb"
# if [ $? -eq 0 ]; then
#     echo "    ✓ ./picture/analysis_value_per_step${WEIGHT_SUFFIX}.png"
# else
#     echo "    ✗ 評価値グラフの生成に失敗"
# fi

# # ペア割合グラフ
# echo "  - ペア割合グラフを生成中..."
# sed "s|analysis_pair_per_step\.png|analysis_pair_per_step${WEIGHT_SUFFIX}.png|g" ./gb/plt_analysis_pair_graph.gb > "$TEMP_GB_DIR/plt_analysis_pair_graph_temp.gb"
# gnuplot "$TEMP_GB_DIR/plt_analysis_pair_graph_temp.gb"
# if [ $? -eq 0 ]; then
#     echo "    ✓ ./picture/analysis_pair_per_step${WEIGHT_SUFFIX}.png"
# else
#     echo "    ✗ ペア割合グラフの生成に失敗"
# fi

# 複合グラフ
echo "  - 複合グラフを生成中..."
sed "s|analysis_combined\.png|analysis_combined${WEIGHT_SUFFIX}.png|g" ./gb/plt_analysis_combined_graph.gb > "$TEMP_GB_DIR/plt_analysis_combined_graph_temp.gb"
gnuplot "$TEMP_GB_DIR/plt_analysis_combined_graph_temp.gb"
if [ $? -eq 0 ]; then
    echo "    ✓ ./picture/analysis_combined${WEIGHT_SUFFIX}.png"
else
    echo "    ✗ 複合グラフの生成に失敗"
fi



echo ""
echo "=== 完了 ==="
echo "生成されたグラフ:"
ls -la ./picture/analysis_*${WEIGHT_SUFFIX}.png 2>/dev/null || echo "  (グラフファイルが見つかりません)"

# 一時ファイルをクリーンアップ
echo ""
echo "一時ファイルをクリーンアップ中..."
rm -f ./testcase/analysis_value_data.txt ./testcase/analysis_pair_data.txt
rm -rf "$TEMP_GB_DIR"
echo "完了！"