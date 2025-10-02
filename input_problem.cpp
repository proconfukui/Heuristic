#include <fstream>
#include <iostream>
#include <cstdlib>
#include <sstream>

#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// コマンドライン引数は3つ
// 1つ目の引数はjsonのパス,2つ目のパスは重みの情報が書いてあるファイルのパス
// 3つ目はそのファイルの何行目を参照するか
int main(int argc, char* argv[]) {
  if(argc !=4){
    cerr << "引数が足りません" << endl;
    return 1;
  }

  // JSONファイル読み込み
  ifstream ifs(argv[1]);
  if (!ifs) {
    cerr << "JSONファイルが開けませんでした。" << endl;
    return 1;
  }

  json data;
  ifs >> data;

  // startsAtの出力
  if (data.contains("startsAt")) {
    cout << data["startsAt"] << endl;
  } else {
    cout << "startsAt: 未定義" << endl;
  }

  // sizeの出力
  if (data.contains("problem") && data["problem"].contains("field") &&
      data["problem"]["field"].contains("size")) {
    cout << data["problem"]["field"]["size"] << endl;
  } else {
    cout << "size: 未定義" << endl;
  }

  // entitiesの出力
  if (data.contains("problem") && data["problem"].contains("field") &&
      data["problem"]["field"].contains("entities")) {
    for (const auto& row : data["problem"]["field"]["entities"]) {
      for (const auto& val : row) {
        cout << val << " ";
      }
      cout << endl;
    }
  } else {
    cout << "entities: 未定義" << endl;
  }

  // 重みファイルから指定された行を読み取り
  ifstream weight_file(argv[2]);
  if (!weight_file) {
    cerr << "重みファイルが開けませんでした。" << endl;
    return 1;
  }

  int target_line = atoi(argv[3]);
  if (target_line <= 0) {
    cerr << "行番号は1以上の整数を指定してください。" << endl;
    return 1;
  }

  string line;
  int current_line = 0;
  bool found = false;
  
  while (getline(weight_file, line)) {
    current_line++;
    if (current_line == target_line) {
      
      
      // 重みの数をカウント
      istringstream iss(line);
      string token;
      int weight_count = 0;
      
      while (iss >> token) {
        weight_count++;
      }
      
      cout << weight_count << endl;
      cout << line << endl;

      found = true;
      break;
    }
  }

  if (!found) {
    cerr << "指定された行が見つかりませんでした。" << endl;
    return 1;
  }

  weight_file.close();

  return 0;
}