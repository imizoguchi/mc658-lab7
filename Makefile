CPP     = g++
#CC_ARGS = -m64 -std=c++11 -g -O2
CC_ARGS = -m64 -g -O2

# Modifique os caminhos de instalacao do Gurobi e do lemon abaixo abaixo
GUROBI_HOME=/opt/gurobi602/linux64/
#LEMON_HOME=

LIB=-lemon -lgurobi_c++ -lgurobi60
INC=-I`pwd`

ifdef LEMON_HOME
  LIB+= -L$(LEMON_HOME)/lib
  INC+= -I$(LEMON_HOME)/include
endif

ifdef GUROBI_HOME
  LIB+= -L$(GUROBI_HOME)/lib
  INC+= -I$(GUROBI_HOME)/include
endif

SOURCES = group_tsp.cpp geompack.cpp mygraphlib.cpp myutils.cpp group_tsp_lp_solver.cpp main.cpp
OBJS    = $(SOURCES:.cpp=.o)

all: $(OBJS)
	$(CPP) $(CC_ARGS) $^ -o group_tsp $(LIB)

%.o: %.cpp
	$(CPP) $(CC_ARGS) -c $^ $(INC) -o $@

clean:
	rm -f group_tsp $(OBJS)
