#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "使用法: " << argv[0] << " <analysis_data_file>" << endl;
        return 1;
    }
    
    string input_file = argv[1];
    ifstream file(input_file);
    
    if (!file) {
        cerr << "ファイルが開けません: " << input_file << endl;
        return 1;
    }
    
    vector<string> lines;
    string line;
    
    // 3行を読み取り
    for (int i = 0; i < 3 && getline(file, line); i++) {
        lines.push_back(line);
    }
    
    if (lines.size() != 3) {
        cerr << "データは3行である必要があります" << endl;
        return 1;
    }
    
    // 各行をパース
    vector<int> steps;
    vector<float> values;
    vector<float> pair_ratios;
    
    // 1行目: ステップ数
    istringstream step_stream(lines[0]);
    int step;
    while (step_stream >> step) {
        steps.push_back(step);
    }
    
    // 2行目: 評価値
    istringstream value_stream(lines[1]);
    float value;
    while (value_stream >> value) {
        values.push_back(value);
    }
    
    // 3行目: ペア割合
    istringstream pair_stream(lines[2]);
    float pair_ratio;
    while (pair_stream >> pair_ratio) {
        pair_ratios.push_back(pair_ratio);
    }
    
    // データ数の確認
    if (steps.size() != values.size() || values.size() != pair_ratios.size()) {
        cerr << "データの数が一致しません" << endl;
        cerr << "ステップ数: " << steps.size() << ", 評価値: " << values.size() << ", ペア割合: " << pair_ratios.size() << endl;
        return 1;
    }
    
    // 評価値データを出力
    ofstream value_file("./testcase/analysis_value_data.txt");
    for (size_t i = 0; i < steps.size(); i++) {
        value_file << steps[i] << " " << values[i] << endl;
    }
    value_file.close();
    
    // ペア割合データを出力
    ofstream pair_file("./testcase/analysis_pair_data.txt");
    for (size_t i = 0; i < steps.size(); i++) {
        pair_file << steps[i] << " " << pair_ratios[i] << endl;
    }
    pair_file.close();
    
    cout << "データ変換完了:" << endl;
    cout << "  評価値データ: ./testcase/analysis_value_data.txt" << endl;
    cout << "  ペア割合データ: ./testcase/analysis_pair_data.txt" << endl;
    cout << "  データ点数: " << steps.size() << endl;
    
    return 0;
}