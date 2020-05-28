#include <iostream>
#include "SurveyPropagation.h"
#include "FactorGraph.h"
#include <filesystem>
using namespace std;
namespace fs = filesystem;

#define CNF_PATH "../../cnf"
static vector<string> cnf_folder;

int main() {
    int sel = 0;
    // Get the files of the folder and print the files inside.
    cout << "Choose the desired formula: " << endl;
    for (const auto &entry : fs::directory_iterator(CNF_PATH)) {
        cnf_folder.push_back(entry.path().string());
        cout << sel << ": " << entry.path().filename() << endl;
        sel++;
    }

    do {
        cin >> sel;
    } while (sel < 0 && sel > cnf_folder.size());

    FactorGraph my_graph(cnf_folder[sel]);
    FactorGraph res = my_graph.PartialAssignment(0, true);
    std::cout << res <<  std::endl;
    auto i = res.getUnitVars();
    for (auto it : i)
        cout << "unit var " << it << endl;
    auto b = res.UnitPropagation();
    cout << b << endl;
}
