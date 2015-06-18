#ifndef GROUP_TSP_LP_SOLVER_H_LNPSQND8
#define GROUP_TSP_LP_SOLVER_H_LNPSQND8

#include <lemon/list_graph.h>
#include "mygraphlib.h"
#include <set>
#include <vector>
#include "gurobi_c++.h"

using namespace std;

class SubTourElim: public GRBCallback {
public:

  SubTourElim( ListGraph &origG, ListGraph::EdgeMap<GRBVar> &_x,
               ListGraph::NodeMap<GRBVar> &_y);
  ~SubTourElim();

protected:
  ListGraph::EdgeMap<GRBVar> &x;
  ListGraph::NodeMap<GRBVar> &y;
  ListGraph &g;
  ListDigraph dg;

  ListGraph::NodeMap<ListDigraph::Node> corresponding_vertex;
  ListDigraph::NodeMap<ListGraph::Node> corresponding_vertex_rev;
  ListDigraph::ArcMap<ListGraph::Edge > corresponding_edge;

  void callback();
};

class GroupTSPLPSolver
{
public:
  GroupTSPLPSolver (ListGraph &g,  ListGraph::EdgeMap<double>& weights, vector<
                    set<ListGraph::Node> > &S);
  virtual ~GroupTSPLPSolver ();


  // fixa o valor da variável y[v] um valor inteiro específico (0 ou 1)
  void fixNodeVariable(Node v, int val);

  // permite que variável y[v] possua valor racional qualquer
  void unfixNodeVariable(Node v);

  // fixa o valor da variável x[e] um valor inteiro específico (0 ou 1)
  void fixEdgeVariable(Edge e, int val);
  //
  // permite que variável x[e] possua valor racional qualquer
  void unfixEdgeVariable(Edge e);

  enum ReturnType {OPTIMAL_INTEGRAL, OPTIMAL_FRACTIONARY, INFEASIBLE};
  enum StateType {FREE, MUST_USE, MUST_NOT_USE};

  ReturnType getSolution(ListGraph::NodeMap<double>
                         &lpsol_vertex,
                         ListGraph::EdgeMap<double>
                         &lpsol_edge, double &obj);
private:
  ListGraph &g;
  GRBEnv env;
  SubTourElim ste;
  GRBModel model;
  ListGraph::EdgeMap<GRBVar> x;
  ListGraph::NodeMap<GRBVar> y;
  ListGraph::NodeMap<StateType> state_v;
  ListGraph::EdgeMap<StateType> state_e;
};


#endif /* end of include guard: GROUP_TSP_LP_SOLVER_H_LNPSQND8 */

