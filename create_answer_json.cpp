#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "json.hpp"
#include "utils.hpp"

using json = nlohmann::ordered_json;

int main() {
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
        Operation op = {x,y,n}
        data.push_back(op);
      }
    }
  }

  // JSONに変換して出力
  json j;
  j["ops"] = data;

  std::cout << j.dump(4) << std::endl;

  return 0;
}