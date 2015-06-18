#include "group_tsp.h"
#include <limits>
#include <cmath>
#include <queue>
#include <lemon/preflow.h>
#include "group_tsp_lp_solver.h"
#include <random>

#define EPS 1e-7

int calculateIntegralWithProportionalProbability(float prob);

int
HeuristicGroupTSP(ListGraph &g,  ListGraph::EdgeMap<double>& weights, vector<
                  set<ListGraph::Node> > &S, vector<ListGraph::Node> &sol, long
                  max_time,  double &best_time,  double &LB, string &alg_info)
{
  /**
   * Computa solucao heuristica para o Group TSP.
   *
   * Entrada:
   * @param   g           grafo simples utilizado
   * @param   weights     pesos das arestas
   * @param   S           vetor de grupos de vertices (ver def. do problema)
   * @param   max_time    tempo maximo (em seg) que o procedimento deve ocorrer
   *
   * Saida:
   * @param   sol         sequencia de vertices que representa ciclo
   * @param   best_time   momento em que solucao atual foi encontrada
   * @param   LB          limite inferior encontrado para custo otimo
   * @param   alg_info    informacoes de execucao do algoritmo, ex: cadeia de
   *                      heuristicas utilizadas
   *
   * @return          0 = nao foi possivel encontrar solucao
   *                  1 = solucao encontrada, mas nao necessariamente otima
   *                  2 = solucao otima encontrada
   */

  // Sample Algorithm
  // Until solution is not integral, sets highest variable to its closest
  // integer value.

  // Variables
  GroupTSPLPSolver::ReturnType rettype = GroupTSPLPSolver::OPTIMAL_FRACTIONARY;
  GroupTSPLPSolver solver(g, weights, S);
  ListGraph::NodeMap<double> lpsol_vertex(g);
  ListGraph::EdgeMap<double> lpsol_edge(g);
  ListGraph::NodeMap<bool> already_set(g, false);
  double objVal = 0;
  time_t start = time(NULL);

  LB = -1;

  // main loop
  cout << "STARTING\n";
  while (rettype == GroupTSPLPSolver::OPTIMAL_FRACTIONARY) {
    cout << "Solving..\n";
    rettype = solver.getSolution(lpsol_vertex, lpsol_edge, objVal);

    if (fabsl(LB - (-1)) < EPS) {
      LB = objVal;
    }


    if (rettype == GroupTSPLPSolver::OPTIMAL_FRACTIONARY) {
      // Round highest variable to closest value.
      double mx = -1;
      Node mx_idx=INVALID;

      for(NodeIt v(g); v!=INVALID; ++v) {
        if (!already_set[v]) {
          if (lpsol_vertex[v] > mx) {
            mx = lpsol_vertex[v];
            mx_idx = v;
          }
        }
      }

      if (mx_idx != INVALID) {
        // int val = (lpsol_vertex[mx_idx] > 0.5 ? 1 : 0);
        int val = calculateIntegralWithProportionalProbability(lpsol_vertex[mx_idx]);
        cout << "Fixing " << g.id(mx_idx) << " to " << val << "\n";
        solver.fixNodeVariable(mx_idx, val);
        already_set[mx_idx] = true;
      } else {
        // it seems we were unable to obtain a solution with integral x[e]
        break;
      }
    }
  }

  // OBTAIN SOLUTION
  best_time = time(NULL) - start;
  if (rettype != GroupTSPLPSolver::OPTIMAL_INTEGRAL) {
    return 0;
  } else {
    /* do dfs to find out the answer */
    ListGraph::NodeMap<bool> vis(g, false);

    for(ListGraph::NodeIt v(g); v != INVALID; ++v) {
      if ( lpsol_vertex[v] > 1.0 - EPS ) {
        queue<ListGraph::Node> q;
        vis[v] = true;
        q.push(v);

        while (!q.empty()) {
          ListGraph::Node u = q.front(); q.pop();
          sol.push_back(u);

          for(ListGraph::IncEdgeIt e(g, u); e != INVALID; ++e) {
            ListGraph::Node w = g.runningNode(e);
            if (lpsol_edge[e] > 1.0 - EPS && !vis[w]) {
              vis[w] = true;
              q.push(w);
              /* only pick one! */
              break;
            }
          }
        }

        break;
      }
    }

    // check if there are vertices outside the cycle
    for(NodeIt v(g); v != INVALID; ++v) {
      if (lpsol_vertex[v] > 1-EPS && !vis[v]) {
        sol.clear();
        return 0;
      }
    }
    return fabsl(objVal - LB) < EPS ? 2 : 1;
  }
}

int calculateIntegralWithProportionalProbability(float prob) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<double> dist(0, 1);
  if(dist(mt) >= prob) {
    return 0;
  } else {
    return 1;
  }
}