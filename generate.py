from random import *
from itertools import *
from math import *
import sys
from operator import itemgetter

try:
    n      = int(sys.argv[1])
    option = int(sys.argv[2])
    t      = int(sys.argv[3])

    if option not in {1, 2, 3}:
        raise Exception("Invalid option")

    if len(sys.argv) > 4:
        fraction = float(sys.argv[4])
    else:
        fraction = 0.5

    if len(sys.argv) > 5:
        m = int(sys.argv[5])
    else:
        #m = randint(1, n*(n-1)//2)
        m = n*(n-1)//2
except IndexError as e:
    print("\nUso:\n")
    print("\t$ python %s n option t [fracao] [m]" % sys.argv[0])
    print("")
    print("\t   n       eh o numero de vertices")
    print("\t   option  eh a forma de geracao de grupos:")
    print("\t           1 = t-particao aleatoria dos vertices")
    print("\t           2 = vertices em volta de centroides aleatorios")
    print("\t           3 = vertices em volta de centroides aleatorios (aleatorizado)")
    print("\t   t       eh o numero de grupos")
    print("\t   fracao  eh a fracao de vertices que fazem parte de grupos (opcional)")
    print("\t   m       eh o numero de arestas (opcional)")
    exit(1)


pos = [(uniform(0,1), uniform(0,1)) for i in range(n) ]
def dist(p1, p2):
    return sqrt( (p1[0]-p2[0])**2 + (p1[1]-p2[1])**2 )


print(n,m,t)
# print nodes
print("\n".join("{} {}".format(*p) for p in pos))

# print edges
edges = sample(list( (i,j,dist(pos[i],pos[j])) for i,j in product(range(n), range(n)) if i < j), m)
for edge in edges:
    print(*edge)

if option == 1:
    # select a random t-partition of ceil(n*fraction) random nodes.
    l = sample(range(n), int(n*fraction) )
    pts = [0] + sorted(sample(range(1,len(l)), t-1)) + [len(l)]
    for i in range(t):
        print(pts[i+1]-pts[i], *l[pts[i]:pts[i+1]])
elif option in {2,3}:
    centroids = [ (uniform(0,1), uniform(0,1)) for i in range(t) ]
    groups = [ [] for i in range(t) ]
    for i in range(n):
        if uniform(0,1) < fraction:
            # this vertex belongs to a group
            if option == 2:
                index, val = min(enumerate( dist(centroid, pos[i]) for centroid in centroids ), key=itemgetter(1))
            elif option == 3:
                dists = [dist(centroid, pos[i])**0.5 for centroid in centroids]
                acc = sum(dists)
                dec = uniform(0,1)
                for j in range(t):
                    p = dists[j] / acc
                    if p > dec:
                        index = j
                        break
                    else:
                        dec -= p
            groups[index].append(i)
    for group in groups:
        print(len(group), *group)
