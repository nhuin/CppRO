#include <ilcplex/ilocplex.h>

static void CPLEX_getDual(benchmark::State& state) {
    while (state.KeepRunning()) {
        IloEnv env;
        IloModel model(env);
        IloNumVarArray x(env, state.range(0), 0.0, IloInfinity, ILOINT);
        IloAdd(model, x);
        IloCplex solver;
        solver.solve();
        for (int i = 0; i < state.range(0); ++i) {
            IloNum val = solve.getDual(x[i]);
        }
    }
}
