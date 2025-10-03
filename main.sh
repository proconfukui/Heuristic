#!/bin/zsh

# 試合での運用時は、次のコマンドを使うこと：
# ./main.sh | ./bin/create_answer_json.exe testcase/answer.json

CORE_NUM=$(sysctl -n hw.ncpu)

# main.exeをコアごとに実行し、一時ファイルに出力
for i in {1..$CORE_NUM}; do
  ./bin/input_problem.exe testcase/problem.json testcase/weights.txt $i | ./bin/main.exe > output_$i.txt &
done

wait

# 一時ファイルの出力を合成
echo $CORE_NUM
cat output_*.txt

# 一時ファイルを削除
rm -f output_*.txt
