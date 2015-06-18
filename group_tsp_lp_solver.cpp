#include "group_tsp_lp_solver.h"
#include <limits>
#include <cmath>
#include <queue>
#include <lemon/preflow.h>

#define EPS 1e-7

#define all(a) (a).begin(), (a).end()
#define inf 0x3f3f3f3f
#define pb(x) push_back(x)
#define forall(x, a, b) for(let(x, a), _##x = (b); x <= _##x; x++)
#define inf 0x3f3f3f3f
#define let(x, a) __typeof(a) x = (a)
#define printA(a,L,R) do { cerr << #a << "[" << #L << ".." << #R << "] = "; forall(asldf,L,R) cerr << (a)[asldf] << (asldf==(R)?'\n':' '); } while(0)
#define repi(i,a) for(__typeof((a).begin()) i=(a).begin(), _##i=(a).end(); i!=_##i; ++i)
#define in(a,b) ( (b).find(a) != (b).end())
#define infl 0x3f3f3f3f3f3f3f3fLL
#define mfill(x, a) memset(x, a, sizeof(x))
#define bitcount(x) __builtin_popcountll(x)
#define sz(a) ((int)(a.size()))
#define fi first
#define se second
#define mp(a,b) make_pair(a,b)

SubTourElim::SubTourElim( ListGraph &origG, ListGraph::EdgeMap<GRBVar> &_x,
                          ListGraph::NodeMap<GRBVar> &_y)
: x(_x), y(_y), g(origG), corresponding_vertex(origG),
    corresponding_edge(dg), corresponding_vertex_rev(dg)
{

  for(ListGraph::NodeIt v(origG); v != INVALID; ++v) {
    corresponding_vertex[v] = dg.addNode();
    corresponding_vertex_rev[corresponding_vertex[v]] = v;
  }

  for(ListGraph::EdgeIt e(origG); e != INVALID; ++e) {
    ListDigraph::Arc e1 = dg.addArc( corresponding_vertex[ g.u(e) ], corresponding_vertex[ g.v(e) ]);
    ListDigraph::Arc e2 = dg.addArc( corresponding_vertex[ g.v(e) ], corresponding_vertex[ g.u(e) ]);

    corresponding_edge[e1] = e;
    corresponding_edge[e2] = e;
  }
}

SubTourElim::~SubTourElim() {
  // do nothing
}

void
SubTourElim::callback() {
  try {
    if (where == GRB_CB_MIPSOL) {
      ListDigraph::ArcMap<double> capacities(dg);
      for(ListDigraph::ArcIt e(dg); e != INVALID; ++e) {
        capacities[e] = getSolution( x[ corresponding_edge[e] ] );
      }


      for(ListDigraph::NodeIt v(dg); v != INVALID; ++v) {
        for(ListDigraph::NodeIt u(dg, v); u != INVALID; ++u) {
          ListGraph::Node vg = corresponding_vertex_rev[v];
          ListGraph::Node ug = corresponding_vertex_rev[u];
          double rhv;
          if (v != u && (rhv = getSolution(y[vg]) + getSolution(y[ug]) - 1.0) > EPS) {
            Preflow<ListDigraph, ListDigraph::ArcMap<double> > flow(dg, capacities, v, u);
            flow.runMinCut();
            if (flow.flowValue() + EPS < 2*rhv) {
              GRBLinExpr expr = 0;
              for(ListDigraph::NodeIt w(dg); w!= INVALID; ++w) {
                if (flow.minCut(w)) {
                  for(ListDigraph::OutArcIt oa(dg, w); oa != INVALID; ++oa) {
                    if (!flow.minCut(dg.runningNode(oa))) {
                      expr += x[corresponding_edge[oa]];
                    }
                  }
                }
              }
              addLazy(expr >= 2 * (y[vg] + y[ug] - 1));
            }
          }
        }
      }
    }
  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  }
}

GroupTSPLPSolver::GroupTSPLPSolver (ListGraph &g,  ListGraph::EdgeMap<double>&
                                    weights, vector< set<ListGraph::Node> >
                                    &S)
: g(g), env(), model(env), x(g), y(g), ste(g,x,y), state_v(g, FREE),
    state_e(g, FREE)
{
  try {
    // Comment the next line to display Gurobi log
    // model.getEnv().set(GRB_IntParam_OutputFlag, 0);

    model.getEnv().set(GRB_IntParam_Method, 2);
    model.getEnv().set(GRB_IntParam_LazyConstraints, 1);
    model.set(GRB_IntAttr_ModelSense, +1); //minimization
    for(ListGraph::EdgeIt e(g); e != INVALID; ++e) {
      x[e] = model.addVar(0.0, 1.0, weights[e], GRB_SEMICONT);
    }
    for(ListGraph::NodeIt v(g); v != INVALID; ++v) {
      y[v] = model.addVar(0.0, 1.0, 0.0, GRB_SEMICONT);
    }
    model.update();

    for(ListGraph::NodeIt v(g); v != INVALID; ++v) {
      GRBLinExpr expr = 0;
      for(ListGraph::IncEdgeIt e(g, v); e != INVALID; ++e) {
        expr += x[e];
      }
      model.addConstr(expr == 2 * y[v]);
    }

    repi(s, S) {
      GRBLinExpr expr = 0;
      repi(v, *s) {
        expr += y[*v];
      }
      model.addConstr(expr >= 1);
    }

    model.setCallback(&ste);
    model.update();
  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  }
}

GroupTSPLPSolver::~GroupTSPLPSolver()
{
  // do nothing
}

void
GroupTSPLPSolver::fixNodeVariable(Node v, int val)
{
  if (state_v[v] != FREE) {
    unfixNodeVariable(v);
  }

  if (val == 0) {
    state_v[v] = MUST_NOT_USE;
    y[v].set(GRB_DoubleAttr_UB, 0.0);
  } else if(val == 1) {
    y[v].set(GRB_DoubleAttr_LB, 1.0);
    state_v[v] = MUST_USE;
  } else {
    cerr << "ERROR: Invalid node state!" << endl;
  }
  model.update();
}

void
GroupTSPLPSolver::unfixNodeVariable(Node v)
{
  if (state_v[v] != FREE) {
    y[v].set(GRB_DoubleAttr_LB, 0.0);
    y[v].set(GRB_DoubleAttr_UB, 1.0);
    model.update();
  }
}

void
GroupTSPLPSolver::fixEdgeVariable(Edge e, int val)
{
  if (state_e[e] != FREE) {
    unfixEdgeVariable(e);
  }

  if (val == 0) {
    state_e[e] = MUST_NOT_USE;
    x[e].set(GRB_DoubleAttr_UB, 0.0);
  } else if(val == 1) {
    x[e].set(GRB_DoubleAttr_LB, 1.0);
    state_e[e] = MUST_USE;
  } else {
    cerr << "ERROR: Invalid edge state!" << endl;
  }
  model.update();
}

  void
GroupTSPLPSolver::unfixEdgeVariable(Edge e)
{
  if (state_e[e] != FREE) {
    x[e].set(GRB_DoubleAttr_LB, 0.0);
    x[e].set(GRB_DoubleAttr_UB, 1.0);
    model.update();
  }
}

GroupTSPLPSolver::ReturnType
GroupTSPLPSolver::getSolution(ListGraph::NodeMap<double> &lpsol_vertex,
                              ListGraph::EdgeMap<double> &lpsol_edge, double
                              &obj)
{
  try {
    //model.reset();
    model.optimize();
    if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
      for(NodeIt v(g); v != INVALID; ++v) {
        lpsol_vertex[v] = y[v].get(GRB_DoubleAttr_X);
      }
      for(EdgeIt e(g); e != INVALID; ++e) {
        lpsol_edge[e] = x[e].get(GRB_DoubleAttr_X);
      }
      obj = model.get(GRB_DoubleAttr_ObjVal);
      for(NodeIt v(g); v != INVALID; ++v) {
        if (lpsol_vertex[v] > EPS && lpsol_vertex[v] < 1-EPS)
          return OPTIMAL_FRACTIONARY;
      }
      for(EdgeIt e(g); e != INVALID; ++e) {
        if (lpsol_edge[e] > EPS && lpsol_edge[e] < 1-EPS)
          return OPTIMAL_FRACTIONARY;
      }
      return OPTIMAL_INTEGRAL;
    } else {
      return INFEASIBLE;
    }
  } catch(GRBException e) {
    cout << "Error code = " << e.getErrorCode() << endl;
    cout << e.getMessage() << endl;
  }

}
