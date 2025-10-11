./main.sh testcase/problem.json testcase/weights.txt 1 ./bin/main.exe > output.txt
./bin/create_answer_json.exe testcase/answer.json < output.txt