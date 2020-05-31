#include <iostream>
#include "SurveyPropagation.h"
#include <filesystem>

using namespace std;

static vector<string> cnf_folder;

/**
 * @brief Function that initialize cnf_folder vector. This function will iterate over the path defined in
 * CNF_PATH (this macro is defined using cmake) and save the path of each file in the cnf_folder.
 */
void initializeCnfFolder() {
    // Get the files of the folder and save the path in the folder vector.
    for (const auto &entry : filesystem::directory_iterator(CNF_PATH)) {
        cnf_folder.push_back(entry.path().string());
    }
}

/**
 * @brief Function that implements a simple menu in order to select a cnf file.
 * @return Index of the cnf_folder vector.
 */
int selectFormula() {
    int sel = 0;
    // Get the files of the folder and print the files inside.
    for (const auto& path : cnf_folder) {
        cout << sel << ": " << SplitString(path, '/').back() << endl;
        sel++;
    }
    sel = -1;
    while (sel < 0 && sel > cnf_folder.size()) {
        cout << "Choose a number between 0 and " << cnf_folder.size() << "." << endl;
        cin >> sel;
    }
    return sel;
}

/**
 * @brief Get a verbose output for SurveyPropagation.
 * @param sol: Result of the SID process.
 * @return String with the verbose output for SurveyPropagation.
 */
string PrintSurveyPropagationResults(int sol) {
    string res;
    switch (sol) {
        case SAT:
            res = "The formula is satisfied with the assignment: ";
            break;
        case CONTRADICTION:
            res = "A contradiction has been founded";
            break;
        case PROB_UNSAT:
            res = "Probably, the formula is unsatisfiable";
            break;
        default:
            res = "";
    }
    return res;
}

int main() {
    // First, initialize the cnf_folder vector.
    initializeCnfFolder();
    FactorGraph my_graph(cnf_folder[selectFormula()], 0);
    SurveyPropagation sp(my_graph);
    std::vector<bool> assignment;

    cout << PrintSurveyPropagationResults(sp.SID(assignment, my_graph.getNVariables())) << endl;
    for (auto it : assignment)
        cout << it << " ";

    cout << endl;
}
