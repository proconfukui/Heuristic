#!/bin/sh

# 使い方：
# ./main.sh 問題ファイルのパス 重みファイルのパス 使用する重みの開始行　メイン実行ファイルのパス
# 「使用する重みの開始行」から「PCのコア数」行分の重みを、コアごとに割り当てる。

# 試合でのコマンド使用例：
# ./main.sh testcase/problem.json testcase/weights.txt 1 ./bin/main.exe | ./bin/create_answer_json.exe testcase/answer.json
# cat output_*.txtで出てきた複数の標準出力の中から最も好ましい解をcreate_answer.jsonで出力する。

CORE_NUM=0
case $(uname -s) in
  Darwin)
    # macOS
    CORE_NUM=$(sysctl -n hw.ncpu)
    ;;
  *)
    # Linux or Windows (WSL/Git Bash)
    CORE_NUM=$(nproc)
    ;;
esac

# main.exeをコアごとに実行し、一時ファイルに出力

#for((i = 1;i <= CORE_NUM;i++));do
for i in `seq 1 $CORE_NUM`
do
  ./bin/input_problem.exe $1 $2 $((i+$3-1)) | \
    ./bin/main.exe  > "output_$i.txt" &
done

wait

# 一時ファイルの出力を合成
echo $CORE_NUM
cat output_*.txt

# 一時ファイルを削除
rm -f output_*.txt
