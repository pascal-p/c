/*****************************************************************************
*                                                                            *
* ----------------------- Graph Abstract Datatype -------------------------- *
*                                                                            *
*****************************************************************************/

#ifndef GRAPH_H
#define GRAPH_H

#include <stdlib.h>
#include <stdbool.h>

#include "sllist.h"

// adjacency list
typedef struct AdjList_ {
  void      *vertex;
  int       weight;      // for futur usage on weighted graph
  sllist_t adjacent;
} adjlist_t;


// graph structure
typedef struct Graph_ {
  int       vcount;
  int       ecount;  
  int       (*match)(const void *k1, const void *k2);
  void      (*destroy)(void *data);
  void      (*cb)(const void *pdata);
  void      (*lcb)(const void *pdata);
  sllist_t  adjlists;
  bool      directed;   // make the graph directed or not, default is true
} graph_t;

// define state for graph traversal 
typedef enum {undiscovered, discovered, processed} t_VState;

/*
 * to handle default values in graph_new, graph_init
 * + wr_graph_new, wr_graph_init + macros
 */
typedef struct {
  graph_t **graph;
  int  (*match)(const void *key1, const void *key2);
  void (*destroy)(void *data);
  void (*cb)(const void *pdata);
  void (*lcb)(const void *pdata);
  bool directed;
} args_t;



int wr_graph_new(args_t in);

void wr_graph_init(args_t in);

// public functions
int graph_new_base(graph_t **graph,
              int (*match)(const void *key1, const void *key2), 
              void (*destroy)(void *data),
              void (*cb)(const void *pdata),
              void (*lcb)(const void *pdata),
              bool directed);

void graph_init_base(graph_t *graph, 
                int (*match)(const void *key1, const void *key2), 
                void (*destroy)(void *data),
                void (*cb)(const void *pdata),
                void (*lcb)(const void *pdata),
                bool directed);

void graph_free(graph_t **graph);

int graph_ins_vertex(graph_t *graph, const void *data);

int graph_ins_edge(graph_t *graph, const void *data1, const void *data2);

int graph_rem_vertex(graph_t *graph, void **data);

int graph_rem_edge(graph_t *graph, void *data1, void **data2);

/*
 * graph_adjlist returns 0 if retrieving the adjacency list is successful,
 * -1 otherwise
 */
int graph_adjlist(const graph_t *graph, const void *data, adjlist_t **adjlist);

/*
 *  graph_is_adjacent returns 1, if the 2nd vertex data2 is adjacent to the 1st vertex 
 *  data1 and 0 otherwise
 */
bool graph_is_adjacent(const graph_t *graph, const void *data1, const void *data2);

void graph_iter(const graph_t *graph);

// convenient macros
#define graph_adjlists(graph) ((graph)->adjlists)

#define graph_vcount(graph) ((graph)->vcount)

#define graph_ecount(graph) ((graph)->ecount)

#define graph_new(...) wr_graph_new((args_t){__VA_ARGS__});

#define graph_init(...) wr_graph_init((args_t){__VA_ARGS__});

#endif
