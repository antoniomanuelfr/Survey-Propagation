#include <iostream>
#include "SurveyPropagation.h"
#include "FactorGraph.h"
#include <filesystem>

#define CNF_PATH "../../cnf"
using namespace std;

static vector<string> cnf_folder;

/**
 * @brief Function that initialize cnf_folder vector. This function will iterate over the path defined in
 * CNF_PATH and save the path of each file in the cnf_folder.
 */
void initialize_cnf_folder() {
    // Get the files of the folder and save the path in the folder vector.
    for (const auto &entry : filesystem::directory_iterator(CNF_PATH)) {
        cnf_folder.push_back(entry.path().string());
    }
}

/**
 * @brief Function that implements a simple menu in order to select a cnf file.
 * @return Index of the cnf_folder vector.
 */
int select_formula() {
    int sel = 0;
    // Get the files of the folder and print the files inside.
    for (const auto& path : cnf_folder) {
        cout << sel << ": " << path << endl;
        sel++;
    }
    sel = -1;
    while (sel < 0 && sel > cnf_folder.size()) {
        cout << "Choose a number between 0 and " << cnf_folder.size() << "." << endl;
        cin >> sel;
    }
    return sel;
}
int main() {
    // First, initialize the cnf_folder vector.
    initialize_cnf_folder();
    FactorGraph my_graph(cnf_folder[select_formula()]);
    my_graph.PartialAssignment(0, true);
    std::cout << my_graph <<  std::endl;
    auto i = my_graph.getUnitVars();
    for (auto it : i)
        cout << "unit var " << it << endl;
    my_graph.UnitPropagation();
    cout << my_graph << endl;
}
