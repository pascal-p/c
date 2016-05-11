/*****************************************************************************
*
* -------------------------------- graph.c --------------------------------- *
*
*****************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "graph.h"
#include "sllist.h"
#include "my_malloc.h"


int wr_graph_new(args_t in) {
  int  (*arg_match)(const void *, const void *)   = (in.match != NULL) ? in.match : NULL;
  void (*arg_destroy)(void *)  = (in.destroy != NULL) ? in.destroy : NULL;
  void (*arg_cb)(const void *) = (in.cb != NULL) ? in.cb : NULL;
  void (*arg_lcb)(const void *) = (in.lcb != NULL) ? in.lcb : NULL;
  bool arg_directed = in.directed ? in.directed : true;
  
  return graph_new_base(in.graph, arg_match, arg_destroy, arg_cb, arg_lcb, arg_directed);
}


// hum what about DRY
void wr_graph_init(args_t in) {
  int  (*arg_match)(const void *, const void *)   = (in.match != NULL) ? in.match : NULL;
  void (*arg_destroy)(void *)  = (in.destroy != NULL) ? in.destroy : NULL;
  void (*arg_cb)(const void *) = (in.cb != NULL) ? in.cb : NULL;
  void (*arg_lcb)(const void *) = (in.lcb != NULL) ? in.lcb : NULL;
  bool arg_directed = in.directed ? in.directed : true;

  graph_init_base(*in.graph, arg_match, arg_destroy, arg_cb, arg_lcb, arg_directed);
}


int graph_new_base(graph_t **graph, 
                   int  (*match)(const void *key1, const void *key2), 
                   void (*destroy)(void *data),
                   void (*cb)(const void *pdata),
                   void (*lcb)(const void *pdata),
                   bool directed) {
  
  *graph = xmalloc0(sizeof(graph_t));

  graph_init_base(*graph, match, destroy, cb, lcb, directed);
  return 0;
}


void graph_init_base(graph_t *graph, 
                     int (*match)(const void *key1, const void *key2), 
                     void (*destroy)(void *data),
                     void (*cb)(const void *pdata),
                     void (*lcb)(const void *pdata),
                     bool directed) {

  graph->vcount   = 0;
  graph->ecount   = 0;
  graph->match    = match;
  graph->destroy  = destroy;
  graph->cb       = cb;
  graph->lcb      = lcb;
  graph->directed = directed;

  list_init(&graph->adjlists, match, NULL, lcb);
}


void graph_free(graph_t **graph) {
  // destroy the adjlists
  sllist_t *adjlsts = &(*graph)->adjlists;  

  // remove vertices and their adjacency lists
  while (list_size(adjlsts) > 0) {
    adjlist_t *adjlst;

    // remove vertex
    if (list_rem_next(adjlsts, NULL, (void **) &adjlst) == 0) {

      // remove adjacency list of this vertex
      sllist_t *pl = &adjlst->adjacent;
      list_free(&pl);
      
      if ((*graph)->destroy != NULL)
        (*graph)->destroy(adjlst->vertex);

      free(adjlst); 
    }
  } 
  //
  list_free(&adjlsts);
  memset(*graph, 0, sizeof(graph_t));
  // 
  *graph = NULL;
  return;
}


/*
 * Private function, returns true if graph contains the vertex designated by data
 * also return a pointer to the adjacency list of that vertex 
 * otherwise, returns false (and adjacency list == NULL) if vertex not present 
 */
static bool graph_has_vertex(const graph_t *graph, const void *data, adjlist_t **adjlst) {
  
  for (sllistItm_t *pl = list_head(&graph->adjlists); pl != NULL; pl = pl->next) {
    if (graph->match(data, ((adjlist_t *) list_data(pl))->vertex)) {
      *adjlst = (adjlist_t *) list_data(pl);
      return true;
    }
  } 
  // not found
  *adjlst = NULL;
  return false;
}


int graph_ins_vertex(graph_t *graph, const void *data) {
  // check if the vertex is already in the graph
  adjlist_t *adjlst;
  if (graph_has_vertex(graph, data, &adjlst)) 
    return 1;
  
  int rval;

  // do the alloc and check if OK or not
  adjlst = xmalloc0(sizeof(adjlist_t));

  // malloc OK, add the vertex
  adjlst->vertex = (void *) data;
  
  // init the adjacency list
  list_init(&adjlst->adjacent, graph->match, NULL, graph->lcb);  // no destroy here
 
  // update the graph adjlists with newly addded adjlst (adjacency list)
  if ((rval = list_ins_next(&graph->adjlists, list_tail(&graph->adjlists), adjlst)) != 0) 
    return rval;
  
  // update number of vertex  
  ++graph->vcount;
  return 0;
}


int graph_ins_edge(graph_t *graph, const void *data1, const void *data2) {
  // check that both vertex are  in the graph
  adjlist_t *adjlst;
  if (! graph_has_vertex(graph, data2, &adjlst)) return -1;
  
  sllistItm_t *plitm = NULL;
  for (plitm = list_head(&graph->adjlists); plitm != NULL; plitm = plitm->next) {
    if (graph->match(data1, ((adjlist_t *)list_data(plitm))->vertex))
      break;
  }   
  //
  if (plitm == NULL) return -1;
  /*
   * OK, so now insert edge (pointed to by) data2 in the adjacency list of edge 
   * (pointed to by) data1  - the adjacency list is pointed to by pl
   */
  int rval;
  sllist_t *pl = &((adjlist_t *)list_data(plitm))->adjacent;
  if ((rval = list_ins_next(pl, pl->tail, data2)) != 0)
    return rval; 
  
  // Adjust the edge count
  ++graph->ecount;

  //
  return 0;
}


int graph_rem_vertex(graph_t *graph, void **data) {
  adjlist_t *adjlst = NULL;
  
  // locate the vertex to remove (pl) and keep a pointer to the previous itm (ppl)
  sllist_t    *adjlsts = &graph->adjlists; 
  sllistItm_t *pl      = NULL;  // current  pointer in the adjacency list 
  sllistItm_t *ppl     = NULL;  // previous pointer in the adjacency list
  bool        found    = false;

  for (pl = list_head(adjlsts); pl != NULL; ppl = pl, pl = pl->next) {
    if (graph->match(data, ((adjlist_t *) list_data(pl))->vertex)) {
      ppl   = pl;
      found = true;
    }

    // check if the vertex is member of an adjacency list
    sllistItm_t *plt;
    if (list_find(&((adjlist_t *)list_data(pl))->adjacent, *data, &plt))
      return -1;
  }
  //
  // found the vertex to remove
  if (found) {
    if (list_size(&((adjlist_t *)list_data(pl))->adjacent) > 0) 
      return -1; // vertex has an adjacent list not empty !

    if (list_rem_next(adjlsts, ppl, (void **) &adjlst) == 0) {
      *data = adjlst->vertex;
      free(adjlst);
      --graph->vcount;
      return 0;
    }
  }
  return 1;
}


int graph_rem_edge(graph_t *graph, void *data1, void **data2) {
  //
  adjlist_t *adjlst;
  //
  if (graph_has_vertex(graph, data1, &adjlst)) {
    // found edge designated by data1
    // list_iter(&adjlst->adjacent); printf("++++++ data is %d\n", *(int *) *data2);
    if (list_rem(&adjlst->adjacent, data2) == 0) {
      // removal of data2 from adjLst(data1) is OK
      --graph->ecount;
      return 0;      
    }
    else {
      return -1; // because data2 is not in the adjacency list of data1
    }
  }
  return -1; // (vertex) data1 is not in the graph 
}


int graph_adjlist(const graph_t *graph, const void *data, adjlist_t **adjlist) {
  // find the vertex designated by data
  if (graph_has_vertex(graph, data, adjlist))
    return 0;
  else 
    return -1;
}


bool graph_is_adjacent(const graph_t *graph, const void *data1, const void *data2) {
  adjlist_t *adjlst;
  //
  if (graph_has_vertex(graph, data1, &adjlst)) {
    sllistItm_t *plt;
    return list_find(&adjlst->adjacent, data2, &plt); 
  }
  else 
    return false;
}


void graph_iter(const graph_t *graph) {
  printf(" - graph: (#vertices: %3d, #edges: %3d) \n", graph->vcount, graph->ecount);
  //
  for (sllistItm_t *pl = list_head(&graph->adjlists); pl != NULL; pl = pl->next) {
    adjlist_t  *adjlst = (adjlist_t *) list_data(pl);
    graph->cb(adjlst->vertex);
    if (&adjlst->adjacent != NULL) list_iter(&adjlst->adjacent);
    printf("]\n");
  }
  //
  printf("\n");
}
