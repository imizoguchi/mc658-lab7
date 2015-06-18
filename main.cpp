#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <set>
#include <cerrno>
#include <cstring>
#include <sstream>
#include <lemon/list_graph.h>
#include <cassert>
#include "group_tsp.h"
#include "mygraphlib.h"
#include "myutils.h"
#include <limits>
#include <unistd.h>
#include <signal.h>
using namespace std;
using namespace lemon;

void
read_instance(ListGraph &g, ListGraph::EdgeMap<double> &weights, vector<
              set<ListGraph::Node> > &S, ListGraph::NodeMap<double> &px,
              ListGraph::NodeMap <double> &py, ListGraph::NodeMap<string>
              &vname, ListGraph::EdgeMap<string> &ename);

void
check_solution(ListGraph &g, ListGraph::EdgeMap<double> &weights, vector<
               set<ListGraph::Node> > &S, vector<ListGraph::Node> &sol , double
               &cost);

void
usage(char *argv[]);


  int
main(int argc, char *argv[])
{
  ListGraph g;
  ListGraph::EdgeMap<double> weights(g);
  vector< set<ListGraph::Node > > S;
  ListGraph::NodeMap<double> px(g);
  ListGraph::NodeMap<double> py(g);
  ListGraph::NodeMap<string> vname(g);
  ListGraph::EdgeMap<string> ename(g);
  ListGraph::NodeMap<int> vcolor(g);
  ListGraph::EdgeMap<int> ecolor(g);

  if (argc != 3) {
    usage(argv);
  } else {
    stdin = freopen(argv[1], "r", stdin);
    if (stdin == NULL) {
      puts(strerror(errno));
      usage(argv);
    }
  }

  read_instance(g, weights, S, px, py, vname, ename);

  time_t start, end;
  vector<ListGraph::Node> sol;
  char *argptr;
  const long max_time = strtol(argv[2], &argptr, 10);
  double best_time;
  double LB;
  double cost;
  string alg_info;
  int ret;

  if (argptr == argv[2]) {
    usage(argv);
    exit(EXIT_FAILURE);
  }

  start = time(NULL);

  ret = HeuristicGroupTSP(g, weights, S, sol, max_time, best_time, LB, alg_info);

  end = time(NULL);

  if (ret >= 1) check_solution(g, weights, S, sol, cost);

  cout << "Duracao:\t" << end - start << "\n";
  cout << "Resultado:\t" << ret << " ";
  switch(ret) {
  case 0:
    cout << "(nao foi possivel encontrar solucao).\n";
    break;
  case 1:
    cout << "(solucao encontrada, mas nao necessariamente otima).\n";
    break;
  case 2:
    cout << "(solucao otima encontrada).\n";
    break;
  }
  if (ret >= 1) {
    cout << "Custo da solucao:\t" << cost << "\n";
    cout << "Momento que melhor solucao foi encontrada:\t" << best_time << "\n";
  }
  cout << "Limitante inferior:\t" << LB << "\n";
  if (ret >= 1) {
    cout << "Solucao:\t";
    for(size_t i=0; i<sol.size(); i++) {
      cout << g.id(sol[i]) << " \n"[i==sol.size()-1];
    }
  }
  cout << "Informacoes do algoritmo:\t" << alg_info << "\n";

  /* print graph */
  for(ListGraph::NodeIt v(g); v != INVALID; ++v) {
    vcolor[v] = BLUE;
  }
  for(ListGraph::EdgeIt e(g); e != INVALID; ++e) {
    ecolor[e] = NOCOLOR;
  }
  for(int i=0, sz=(int)sol.size(); i<sz; i++) {
    ListGraph::Edge e = findEdge(g, sol[i], sol[(i+1)%sz]);
    if (e != INVALID) {
      ecolor[e] = RED;
    }
  }
  ViewListGraph(g, vname, ename, px, py, vcolor, ecolor, "");

  return EXIT_SUCCESS;
}


void read_instance(ListGraph &g, ListGraph::EdgeMap<double> &weights, vector<
                   set<ListGraph::Node> > &S, ListGraph::NodeMap<double> &px,
                   ListGraph::NodeMap <double> &py, ListGraph::NodeMap<string>
                   &vname, ListGraph::EdgeMap<string> &ename)
{
  int N, M, T;
  int lineno = 0;
  char aux[200];
  cerr << "Lendo instancia...";

  GET_INPUT(INTEGER_REG " " INTEGER_REG " " INTEGER_REG, "%d%d%d", &N, &M, &T);

  vector<ListGraph::Node> nodes(N);
  vector<ListGraph::Edge> edges(M);
  for(int i=0; i<N; i++) {
    double x, y;
    nodes[i] = g.addNode();
    GET_INPUT(DOUBLE_REG " " DOUBLE_REG, "%lf%lf", &x, &y);
    px[nodes[i]] = x/100.;
    py[nodes[i]] = y/100.;
    sprintf(aux, "%d", i);
    vname[nodes[i]] = aux;
  }

  for(int i=0; i<M; i++) {
    int x,y;
    double w;
    GET_INPUT(INTEGER_REG " " INTEGER_REG " " DOUBLE_REG, "%d%d%lf", &x, &y, &w);
    if (x >= N || y >= N) {
      SPECIFICATION_ERROR("Identificador incorreto para vertice de aresta.");
    }
    edges[i] = g.addEdge(nodes[x], nodes[y]);
    weights[edges[i]] = w;
    sprintf(aux, "%d", i);
    ename[edges[i]] = aux;
  }

  S = vector< set<ListGraph::Node> >(T);
  for(int i=0; i<T; i++) {
    vector<int> s;
    GET_INPUT_SERIES(INTEGER_REG, s);
    for(vector<int>::iterator _x = s.begin(); _x != s.end(); _x++) {
      int x = *_x;
      if (x >= N) {
        SPECIFICATION_ERROR("Identificador incorreto para vertice de grupo.");
      } else {
        S[i].insert(nodes[x]);
      }
    }
  }
  cerr << " OK!" << endl;
}

void
check_solution(ListGraph &g, ListGraph::EdgeMap<double> &weights, vector<
               set<ListGraph::Node> > &S, vector<ListGraph::Node> &sol ,
               double &cost)
{
  int sz = (int) sol.size();

  if (sz <= 2) {
    DISPLAY_ERROR("A solucao precisa ter pelo menos 3 vertices!");
  }

  set<ListGraph::Node> seen;
  for(vector<ListGraph::Node>::iterator _node = sol.begin(); _node != sol.end(); _node++) {
    ListGraph::Node node = *_node;
    if (seen.find(node) != seen.end()) {
      DISPLAY_ERROR("A solucao repete vertices!");
    } else {
      seen.insert(node);
    }
  }

  for(vector<set<ListGraph::Node> >::iterator _s = S.begin(); _s != S.end(); _s++) {
    const set<ListGraph::Node> &s = *_s;
    bool ok = false;
    for(set<ListGraph::Node>::iterator _node = s.begin(); _node != s.end(); _node++) {
      ListGraph::Node node = *_node;
      if (seen.find(node) != seen.end()) {
        ok = true;
        break;
      }
    }
    if (!ok) {
      DISPLAY_ERROR("Existe grupo sem representante na solucao!");
    }
  }

  /* compute cost */
  for(int i=0; i<sz; i++) {
    double mn = numeric_limits<double>::infinity();
    bool ok = false;
    for(ListGraph::Edge e = findEdge(g,sol[i],sol[(i+1)%sz]); e != INVALID; e =
        findEdge(g,sol[i],sol[(i+1)%sz],e)) {
      mn = min(mn, weights[e]);
      ok = true;
    }
    if (!ok) {
      DISPLAY_ERROR("Existe adjacentes sem aresta!");
    }
    cost += mn;
  }

}

void
usage(char *argv[]) {
  cout << "\n";
  cout << "  O programa requer os seguintes parametros:\n";
  cout << "\n";
  cout << "   $ " << argv[0] << " <caminho para arquivo> <tempo max de exec. em seg>\n";
  cout << "\n\n";
  exit(EXIT_FAILURE);
}
