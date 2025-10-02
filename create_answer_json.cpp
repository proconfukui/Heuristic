#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "json.hpp"
#include "utils.hpp"

using json = nlohmann::ordered_json;

// コマンドライン引数は1つ。この引数で指定したパスにjsonファイルを出力する
int main(int argc, char* argv[]) {
  if (argc != 2){
    std::cerr << "引数が足りません" << std::endl;
    return 1;
  }

  std::vector<Operation> data;

  // 標準入力から読み取り
  std::string line;
  
  // 1行目をスキップ
  if (std::getline(std::cin, line)) {
    // 1行目は使用しない（必要に応じて処理）
  }
  
  // 2行目からops数を読み取り
  int ops_count = 0;
  if (std::getline(std::cin, line)) {
    std::istringstream iss(line);
    iss >> ops_count;
  }
  
  // 指定された数だけOperationデータを読み取り
  for (int i = 0; i < ops_count; i++) {
    if (std::getline(std::cin, line)) {
      std::istringstream iss(line);
      int x, y, n;
      
      if (iss >> n >> x >> y) {
        Operation op = {x,y,n};
        data.push_back(op);
      }
    }
  }

  // JSONに変換して出力
  json j;
  for (int i = 0; i < ops_count; i++)
  {
    j["ops"][i]["x"] = data[i].x;
    j["ops"][i]["y"] = data[i].y;
    j["ops"][i]["n"] = data[i].n;
  }

  std::ofstream ofs(argv[1]);
  ofs << j.dump(4) << std::endl;
  ofs.close();

  return 0;
}