#ifndef BFS_H
#define BFS_H

// Bread First Search 

#include "graph.h"
#include "sllist.h"

typedef struct {
  void     *data;
  t_VState state;
  int      hops;
} t_BfsVertex;


int bfs(t_Graph *graph, t_BfsVertex *startv, t_sllist *plhops);

#endif
