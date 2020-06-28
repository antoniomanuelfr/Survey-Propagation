#include <iostream>
#include "SurveyPropagation.h"
#include <filesystem>

using namespace std;

static vector<string> cnf_folder;

/**
 * @brief Function that initialize cnf_folder vector. This function will iterate over the path defined in
 * CNF_PATH (this macro is defined using cmake) and save the path of each file in the cnf_folder.
 */
void initializeCnfFolder(const string& folder = "") {
    cnf_folder.clear();
    // Get the files of the folder and save the path in the folder vector.
    for (const auto &entry : filesystem::directory_iterator(CNF_PATH + folder)) {
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

void Experiment(int N) {
    std::vector<bool> assignment;
    std::ofstream out_file("results.csv");

    vector<double> fractions = {0.04, 0.02, 0.01, 0.005, 0.0025, 0.00125};
    vector<double> alphas = {4.21, 4.22, 4.23, 4.24};

    int solved_formulas, unconverged, false_positives, unsat;
    out_file << "alpha,";
    for (auto i : alphas) {
        out_file << i << ",";
    }
    out_file << endl;

    for (auto f : fractions) {
        out_file << f << ",";
        for (auto a : alphas) {
            solved_formulas = unconverged = unsat = false_positives =0;
            std::stringstream p;
            p << "/testCNF/" << N << "/" << std::setprecision(3) << a ;
            cout << p.str() << endl;
            initializeCnfFolder(p.str());
            for(const auto &path : cnf_folder) {
                FactorGraph orig(path, 0), fg(orig);
                SurveyPropagation SP (fg);
                int res = SP.SIDF(assignment, f);
                switch (res) {
                    case SAT:
                        if (orig.CheckAssignment(assignment)) {
                            solved_formulas++;
                        } else {
                            false_positives++;
                        }
                    break;
                    case SP_UNCONVERGED:
                        unconverged++;
                        break;
                    case PROB_UNSAT:
                        unsat++;
                        break;
                    default:
                        break;
                }
            }
            cout << "Results using f = " << f << " and alpha = " << a << ": " << endl;
            cout << "SAT = " << solved_formulas << ", SP UNCONVERGED = " << unconverged << ", UNSAT = " << unsat
                 << ", false positives = " << false_positives << endl;

            out_file << (solved_formulas / N) << ",";
        }
        out_file << endl;
    }
}

void TestCNF() {
    initializeCnfFolder();
    FactorGraph my_graph(cnf_folder[selectFormula()], 0);
    SurveyPropagation sp(my_graph);
    std::vector<bool> assignment;

    cout << PrintSurveyPropagationResults(sp.SID(assignment, 10)) << endl;
    if (assignment.size() != 0) {
        for (auto it : assignment)
            cout << it << " ";
        cout << endl;
        cout << my_graph.CheckAssignment(assignment);
    }
}
int main() {
//Experiment(10000);}
    TestCNF();
}