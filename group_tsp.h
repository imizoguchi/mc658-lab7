#ifndef EXACTGROPUTSP_H
#define EXACTGROPUTSP_H

#include <lemon/list_graph.h>
#include <vector>
#include <set>

using namespace std;
using namespace lemon;

int
HeuristicGroupTSP(ListGraph &g,  ListGraph::EdgeMap<double>& weights, vector<
                  set<ListGraph::Node> > &S, vector<ListGraph::Node> &sol, long
                  max_time, double &best_time, double &LB, string &alg_info);


#endif /* end of include guard: EXACTGROPUTSP_H */

