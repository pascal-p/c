/*****************************************************************************
*
* ---------------------------------- bfs.c ---------------------------------- *
*
*****************************************************************************/

#include <stdlib.h>
// #include <string.h>
// #include <stdio.h>

#include "bfs.h"
#include "graph.h"
#include "sllist.h"
#include "my_malloc.h"
#include "queue.h"

static void bfs_init_vertex(t_Graph *graph, t_BfsVertex *startv) {
  for (t_SLListItm *pl = list_head(&graph->adjlists); pl != NULL; pl = pl->next) {
    t_BfsVertex c_vertex = ((t_AdjList *) list_data(pl))->vertex;

    if (graph->match(startv, c_vertex)) {
      c_vertex->state = discovered;
      c_vertex->hops  = 0;
    }
    else {
      // all other vertices set to undiscovered, -1
      c_vertex->state = undiscovered;
      c_vertex->hops  = -1;
    }
  }
}

static void list_build(t_Graph *graph, t_sllist *plhops) {
  list_init(plhops); // list_init(plhops, NULL, NULL, NULL);
  
  for (t_SLListItm *pl = list_head(&graph->adjlists); pl != NULL; pl = pl->next) {
    // skip non-visited vertices, those iwth hops count == -1

    t_BfsVertex c_vertex = ((t_AdjList *)list_data(pl))->vertex;
    if (c_vertex->hops != -1) {
      if (list_ins_next(plhos, list_tail(plhops), c_vertex) != 0) {
        list_free(&plhops);
        return -1;
      }
    }
  }
}

int bfs(t_Graph *graph, t_BfsVertex *startv, t_sllist *plhops) {
  
  // init all vertices in hte graph
  bfs_init_vertex(grpah, startv);

  // init the q with adjacency list of the start vertex (startv)
  t_queue q;
  queue_init(&q, NULL);
  
  t_AdjList *clr_adjlst;
  if (graph_adjlist(graph, startv, &clr_adjlst) != 0) {
    queue_free(&q);
    return -1;
  }

  if ((queue_enqueue(&q, clr_adjlst)) != 0) {
    queue_free(&q);
    return -1;
  }
  
  // perform the bfs (core)
  while (queue_size(&q) > 0) {
    t_AdjList *adjlst = queue_peek(&q);

    // traverse each vertex in the current adjacency list (adjlst)
    for (t_SLListItm *pl = list_head(&adjlist->adjacent); pl != NULL; pl = list_next(pl)) {
      t_BfsVertex adj_vertex = list_data(pl);
      
      // Determine the state of the next adjacent vertex
      if (graph_adjacent(graph, adj_vertex, &clr_adjlst) != 0) {
        queue_free(&q);
        return -1;
      }
      t_BfsVertex nxt_vertex = clr_adjlst->vertex;
      if (nxt_vertex->state == undiscovered) {
        /*
         * if nxt_vertex was undiscovered, change its state to discovered 
         * and enqueue its adjacency list
         */
        nxt_vertex->state = discovered;
        nxt_vertex->hops  = ((t_BfsVertex *)adjlst->vertex)->hops + 1;

        if ((queue_enqueue(&q, clr_adjlst)) != 0) {
          queue_free(&q);
          return -1;
        }
      } 
    } // for

    // dequeue curent adjacency list adjlst, set state to processed
    if ((queue_dequeue(&q, (void **) adjlst)) == 0) {
      ((t_BfsVertex *)adjlst->vertex)->state = processed;
    }
    else {
      queue_free(&q);
      return -1;   
    }
  }// while
 
  queue_free(&q);

  //
  list_build(graph, plhops);

  return 0;
}
