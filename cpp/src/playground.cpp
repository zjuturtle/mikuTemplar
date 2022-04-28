#include "utils/queue_with_max_min.h"
#include <string>
#include "ortools/linear_solver/linear_solver.h"
#include <iostream>

using namespace std;
using namespace MikuTemplar;
int main(int argc, char *argv[]){
    unique_ptr<operations_research::MPSolver> solver(operations_research::MPSolver::CreateSolver("SCIP"));
    const double infinity = solver->infinity();
    auto a=solver->MakeNumVar(0.0, 1.0, "a");
    auto b=solver->MakeNumVar(0.0, 1.0, "b");
    operations_research::MPConstraint* const sumToOne = solver->MakeRowConstraint(1.0, 1.0);
    sumToOne->SetCoefficient(a, 1);
    sumToOne->SetCoefficient(b, 1);

    auto objective = solver->MutableObjective();
    objective->SetCoefficient(a, 3);
    objective->SetCoefficient(b, 4);
    objective->SetMaximization();

    auto result_status = solver->Solve();
  // Check that the problem has an optimal solution.
  if (result_status != operations_research::MPSolver::OPTIMAL) {
    cout<< "The problem does not have an optimal solution!";
  }
  // [END solve]

  // [START print_solution]
  cout << "Solution:" << endl;
  cout << "Optimal objective value = " << objective->Value() << endl;
  cout << a->name() << " = " << a->solution_value() << endl;
  cout << b->name() << " = " << b->solution_value() << endl;
}