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

void Experiment(int N) {
    std::vector<bool> assignment;
    std::ofstream out_file("results.csv");
    int n_formulas = 0;
    vector<double> fractions = {0.04, 0.02, 0.01, 0.005, 0.0025, 0.00125};
    vector<double> alphas = {4.21, 4.22, 4.23, 4.24};
    vector<vector<double>> table (fractions.size(), vector<double>(alphas.size(), -1.0));
    bool solved;
    int solved_formulas, unconverged, false_positives, unsat;
    out_file << "fractions/alphas,";
    for (auto i : alphas) {
        out_file << i << ",";
    }
    out_file << endl;

    for (int alpha = 0; alpha < alphas.size(); alpha++) {
        for (int frac = 0; frac < fractions.size(); frac++) {
            solved_formulas = unconverged = unsat = false_positives = 0;
            std::stringstream p;
            p << "/testCNF/" << N << "/" << std::setprecision(3) << alphas[alpha];
            //cout << p.str() << endl;
            initializeCnfFolder(p.str());
            for (const auto &path : cnf_folder) {

                FactorGraph orig(path);
                SurveyPropagation SP(orig);
                int res = SP.SIDF(assignment, fractions[frac]);
                switch (res) {
                    case SAT:
                        if (orig.CheckAssignment(assignment)) {
                            solved_formulas++;
                        } else {
                            cout << "falso positivo path: " << path << std::endl;
                            false_positives++;
                        }
                        break;
                    case SP_UNCONVERGED:
                        unconverged++;
                        break;
                    case PROB_UNSAT:
                        unsat++;
                        break;
                    case CONTRADICTION:
                        cerr << "Contraduction founded in " << path << endl;
                        break;
                    default:
                        break;
                }
            }
            cout << "Results using f = " << fractions[frac] << " and alpha = " << alphas[alpha] << ": " << endl;
            cout << "SAT = " << solved_formulas << ", SP UNCONVERGED = " << unconverged << ", UNSAT = " << unsat
                 << ", false positives = " << false_positives << endl;
            table[frac][alpha] = solved_formulas / N;

        }
    }
    for (int f = 0; f < fractions.size(); f++) {
        out_file << fractions[f] << ",";
        for (int a = 0; a < alphas.size(); a++) {
            out_file << table[f][a] << ",";
        }
        out_file << endl;
    }
}

void TestCNF() {
    initializeCnfFolder();
    FactorGraph my_graph(cnf_folder[selectFormula()]);
    SurveyPropagation sp(my_graph);
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
    Experiment(100);
    //TestCNF();
}