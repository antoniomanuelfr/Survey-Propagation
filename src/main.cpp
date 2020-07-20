#include <iostream>
#include "SurveyPropagation.h"
#include <filesystem>
#include <omp.h>
using namespace std;

static vector<string> cnf_folder;

/**
 * @brief Function that initialize cnf_folder vector. This function will iterate over the path defined in
 * CNF_PATH (this macro is defined using cmake) and save the path of each file in the cnf_folder.
 */
void initializeCnfFolder(const string& folder = "") {
    cnf_folder.clear();
    // Get the files of the folder and save the path in the folder vector.
    try {

        for (const auto &entry : filesystem::directory_iterator(CNF_PATH + folder)) {
            cnf_folder.push_back(entry.path().string());
        }

    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        exit(1);

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
        cout << "Choose a number between 0 and " << cnf_folder.size() - 1 << "." << endl;
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

void Experiment(int N, const string& result = "/bin/results.csv") {
    std::vector<bool> assignment;
    std::ofstream out_file(BIN_PATH + result);
    vector<double> fractions = {0.04, 0.02, 0.01, 0.005, 0.0025, 0.00125};
    vector<double> alphas = {4.21, 4.22, 4.23, 4.24};
    vector<vector<double>> table (fractions.size(), vector<double>(alphas.size(), 0.0));
    int solved_formulas, unconverged, false_positives, unsat;
    out_file << "fractions/alphas,";
    for (auto i : alphas) {
        out_file << i << ",";
    }
    out_file << endl;
    int n_files;
    bool not_solved;
    for (int alpha = 0; alpha < alphas.size(); alpha++) {
        not_solved = true;
        for (int frac = 0; frac < fractions.size() && not_solved; frac++) {
            solved_formulas = unconverged = unsat = false_positives = 0;
            std::stringstream p;
            p << "/testCNF/" << N << "/" << std::setprecision(3) << alphas[alpha];
            //cout << p.str() << endl;
            initializeCnfFolder(p.str());
            n_files = 0;
            for (const auto &path : cnf_folder) {
                n_files++;
                FactorGraph orig(path, 1);
                SurveyPropagation SP(path, 2);
                int res = SP.SIDF(assignment, fractions[frac]);
                switch (res) {
                    case SAT:
                        if (orig.CheckAssignment(assignment)) {
                            solved_formulas++;
                            table[frac][alpha]++;
                        } else {
                            cout << "False positive in path: " << path << std::endl;
                            false_positives++;
                        }
                        break;
                    case SP_UNCONVERGED:
                        unconverged++;
                        cout << "SP didn't converged using the formula " << path << endl;
                        break;
                    case PROB_UNSAT:
                        unsat++;
                        cout << "Formula " << path << " is unsatisfiable" << endl;
                        break;
                    case CONTRADICTION:
                        cerr << "Contradiction founded in " << path << endl;
                        break;
                    default:
                        break;
                }
            }
            not_solved = table[frac][alpha] != n_files;
        }
        cout << "alpha = " << alphas[alpha] << endl;
    }
    for (int f = 0; f < fractions.size(); f++) {
        out_file << fractions[f] << ",";
        for (int a = 0; a < alphas.size(); a++) {
            out_file << (table[f][a] / n_files) << ",";
        }
        out_file << endl;
    }
}

void TestCNF() {
    initializeCnfFolder();
    int index = selectFormula();
    FactorGraph my_graph(cnf_folder[index]);
    SurveyPropagation sp(cnf_folder[index]);
    std::vector<bool> assignment;

    cout << PrintSurveyPropagationResults(sp.SID(assignment, 10)) << endl;
    if (!assignment.empty()) {
        for (auto it : assignment)
            cout << it << " ";
        cout << endl;
        cout << my_graph.CheckAssignment(assignment);
    }
}
int main() {
    Experiment(50);
    //TestCNF();
}
