#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>
#include <unordered_set>

using namespace std;

class SATSolver {
public:
    SATSolver(int numVariables, const vector<vector<int>>& clauses) 
        : numVariables(numVariables), originalClauses(clauses), decisionLevel(0), conflict(false) {
        assignment.resize(numVariables, false);
        levels.resize(numVariables, -1);
    }

    bool solve() {
        return cdcl();
    }

    void printAssignment() {
        for (int i = 0; i < numVariables; ++i) {
            cout << "Variable " << i + 1 << ": " << (assignment[i] ? "True" : "False") << endl;
        }
    }

private:
    int numVariables;
    vector<vector<int>> originalClauses;
    vector<vector<int>> learnedClauses;
    vector<bool> assignment;
    vector<int> levels;
    stack<int> decisionStack;
    int decisionLevel;
    bool conflict;

    bool isSatisfied() {
        for (const auto& clause : originalClauses) {
            bool satisfied = false;
            for (int var : clause) {
                if (assignment[abs(var) - 1] == (var > 0)) {
                    satisfied = true;
                    break;
                }
            }
            if (!satisfied) return false;
        }
        return true;
    }

    void analyzeConflict(const vector<int>& conflictingClause) {
        learnedClauses.push_back(conflictingClause);
        conflict = true; // Indicate a conflict has occurred
    }

    void backtrack() {
        // Reset the levels and assignment for backtracking
        while (!decisionStack.empty()) {
            int var = decisionStack.top();
            decisionStack.pop();
            assignment[var - 1] = false; // Reset assignment
            levels[var - 1] = -1; // Reset level
        }
        decisionLevel--; // Decrease decision level
    }

    bool cdcl() {
        while (true) {
            if (isSatisfied()) {
                return true; // Satisfiable
            }

            int variable = -1;
            for (int i = 0; i < numVariables; ++i) {
                if (levels[i] == -1) {
                    variable = i + 1; // Variable numbering starts from 1
                    break;
                }
            }
            if (variable == -1) {
                return false; 
            }

            // Assign true to the variable
            decisionStack.push(variable);
            assignment[variable - 1] = true;
            levels[variable - 1] = decisionLevel;
            decisionLevel++;

            // Check for conflicts
            for (const auto& clause : originalClauses) {
                bool satisfied = false;
                for (int var : clause) {
                    if (assignment[abs(var) - 1] == (var > 0)) {
                        satisfied = true;
                        break;
                    }
                }
                if (!satisfied) {
                    analyzeConflict(clause);
                    break; 
                }
            }

            // If conflict occurs, backtrack
            if (conflict) {
                backtrack(); // Backtrack to the previous decision point
                if (decisionStack.empty()) {
                    return false; // UNSAT
                }
                // Flip the last decision variable
                int lastVar = decisionStack.top();
                decisionStack.pop();
                assignment[lastVar - 1] = false; 
                decisionStack.push(lastVar); 
                assignment[lastVar - 1] = false; // Try assigning it to false
                levels[lastVar - 1] = decisionLevel;
                decisionLevel++;
                conflict = false; // Reset conflict state
            }
        }
    }
};

int main() {
    int numVariables = 3;
    vector<vector<int>> clauses = {
        {1, 2, 3},
        {-1},
        {-3},
        {-2}
    };

    SATSolver solver(numVariables, clauses);

    if (solver.solve()) {
        cout << "Satisfiable" << endl;
        solver.printAssignment();
    } else {
        cout << "Unsatisfiable" << endl;
    }

    return 0;
}