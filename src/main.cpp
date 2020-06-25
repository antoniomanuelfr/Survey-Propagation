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

    int solved_clauses = 0;
    out_file << "alpha,";
    for (auto i : alphas) {
        out_file << i << ",";
    }
    out_file << endl;

    for (auto f : fractions) {
        out_file << f << ",";
        for (auto a : alphas) {
            solved_clauses = 0;
            std::stringstream p;
            p << "/testCNF/" << std::setprecision(3) << a ;
            initializeCnfFolder(p.str());
            for(const auto &path : cnf_folder) {
            //    cout << "Executing SP over " << path << " with alpha = " << a << " f = " << f << endl;
                FactorGraph orig(path, 0), fg(orig);
                SurveyPropagation SP (fg);
                if (SP.SIDF(assignment, f) == SAT) {
                    if (orig.CheckAssignment(assignment)) {
                        solved_clauses++;
                    }
                }
            }
            cout << "Total resueltas con f = " << f << " y alfa = " << a << " es: " << (solved_clauses / N);
            out_file << (solved_clauses / N) << ",";
        }
        out_file << endl;
    }
}

int main() {
    Experiment(100);
}
