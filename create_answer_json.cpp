#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "json.hpp"
#include "base.hpp"
#include "utils.hpp"

using json = nlohmann::ordered_json;
using namespace std;

void cin_field(vector<vector<int>> &field);

// コマンドライン引数は1つ。この引数で指定したパスにjsonファイルを出力する
int main(int argc, char* argv[]) {
  if (argc != 2){
    std::cerr << "引数が足りません" << std::endl;
    return 1;
  }

  int answer_count;
  cin >> answer_count;

  vector<int> pair_count(answer_count);
  vector<vector<Operation>> ops_data(answer_count);
  for (int answer_index = 0; answer_index < answer_count; answer_index++)
  {
    // 1行目をスキップ
    int time;
    cin >> time;
    // 2行目からops数を読み取り
    int ops_count;
    cin >> ops_count;

    // 指定された数だけOperationデータを読み取り
    ops_data[answer_index] = vector<Operation>(ops_count);
    for (int ops_index = 0; ops_index < ops_count; ops_index++)
    {
      int n, x, y;
      cin >> n >> x >> y;
      Operation op = {x, y, n};
      ops_data[answer_index][ops_index] = op;
    }
    vector<vector<int>> field;
    cin_field(field);
    for (const auto &op : ops_data[answer_index])
    {
      rotate(field,op);
    }
    pair_count[answer_index] = count_pair(field);
  }

  // 複数の解がある場合は、その中から最も良いものを選ぶ
  int best_answer_index = 0;
  for (int index = 1; index < answer_count; index++)
  {
    if (pair_count[index] > pair_count[best_answer_index])
    {
      best_answer_index = index;
    }
    else if (pair_count[index] == pair_count[best_answer_index] && ops_data[index].size() < ops_data[best_answer_index].size())
    {
      best_answer_index = index;
    }
  }

  // // JSONに変換して出力
  json j;
  for (int ops_index = 0; ops_index < ops_data[best_answer_index].size(); ops_index++)
  {
    j["ops"][ops_index]["x"] = ops_data[best_answer_index][ops_index].x;
    j["ops"][ops_index]["y"] = ops_data[best_answer_index][ops_index].y;
    j["ops"][ops_index]["n"] = ops_data[best_answer_index][ops_index].n;
  }
  j["pair_count"] = pair_count[best_answer_index];

  ofstream ofs(argv[1]);
  ofs << j.dump(4) << endl;
  ofs.close();

  return 0;
}

void cin_field(vector<vector<int>> &field)
{
  int field_size;
  cin >> field_size;
  for (int i = 0; i < field_size; i++)
  {
    vector<int> row(field_size);
    for (int j = 0; j < field_size; j++)
    {
      cin >> row[j];
    }
    field.push_back(row);
  }
}