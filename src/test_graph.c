#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "graph.h"




/*
void sub_graph_iter_(t_AdjList  *adjlst) {
  //
  for (t_SLListItm *plitm = list_head(&adjlst->adjacent); plitm != NULL; plitm = plitm->next) {
    printf("%4d, ", *((int *) plitm->data));
  }
  //
}


void graph_iter_(graph_t *graph) {
  printf(" - graph: (#vertices: %3d, #edges: %3d) \n", graph->vcount, graph->ecount);
  //
  for (t_SLListItm *pl = list_head(&graph->adjlists); pl != NULL; pl = pl->next) {
    t_AdjList  *adjlst = (t_AdjList *) list_data(pl);
    printf("  vertex: %3d\tadjacency lst: [", *(int *) adjlst->vertex);
    sub_graph_iter_(adjlst);
    printf("]\n");

  }
  //
  printf("\n");
}
*/


void cb(const void *pdata) {
  // printf("%4d ", *((int *) pdata));
  printf("  vertex: %3d\tadjacency lst: [", *(int *) pdata);
}

void list_cb(const void *pdata) {
  // printf("%4d, ", *((int *) plitm->data));
  printf("%4d ", *((int *) pdata));
}


int i_match(const void *p1, const void *p2) {
  return (*(int *)p1 == *(int *)p2) ? 1 : 0;
}

void wr_graph_ins_vertex(graph_t *graph, int *pi) {
  if (graph_ins_vertex(graph, pi) == 0) 
    graph_iter(graph);
  else {
    printf("[!] problem inserting vertex: %d\n", *pi);
    exit(-1);
  }
}

void wr_graph_ins_edge(graph_t *graph, int *pi, int *pj) {
  printf("[+] inserting an edge between %d and %d\n", *pi, *pj);
  if (graph_ins_edge(graph, pi, pj) == 0) 
    graph_iter(graph);
  else {
    printf("[!] problem inserting edge: %d - %d\n", *pi, *pj);
    exit(-1);
  }
}

void wr_graph_rem_edge(graph_t *graph, int *o, int **d) {
  printf("-- Removing edge from the graph, from orig:%d to dest:%d\n", *o, **d);
  if ( graph_rem_edge(graph, o, (void **) d) == 0)  {
    printf("** Removed: %d !!!\n", **d);
    graph_iter(graph);
  }
  else {
    printf("[!] Could NOT remove edge for some reason...\n"); 
  }
}


/*
 * MAIN
 */
int main(int argc, char **argv) {
  graph_t *graph = NULL; // (graph_t *) malloc(sizeof(graph_t));

  graph_new(&graph,  &i_match, NULL, &cb, &list_cb); // directed will be true  
  // graph_new(&graph,  &i_match, NULL, NULL); // directed will be true  

  if (graph == NULL) {
    printf("Error: could not allocate room for new graph\n");
    return(1);
  }
  //
  int i = 2;
  int j = 6;
  int k = 4; 
  int l = 7;
  
  wr_graph_ins_vertex(graph, &i);
    
  wr_graph_ins_vertex(graph, &j);
  wr_graph_ins_vertex(graph, &k);
  wr_graph_ins_vertex(graph, &l);
  
  // TODO cont'ed
  wr_graph_ins_edge(graph, &i, &j); 
  wr_graph_ins_edge(graph, &i, &k); 
  
  wr_graph_ins_edge(graph, &j, &k); 
  wr_graph_ins_edge(graph, &j, &l); 

  wr_graph_ins_edge(graph, &k, &l); 

  int m = 5;
  wr_graph_ins_vertex(graph, &m);
  wr_graph_ins_edge(graph, &j, &m); 
  
  
  
  // remove an edge
  int *pl = &k;
  wr_graph_rem_edge(graph, &j, &pl);
  //printf("** Removed: %d !!!\n", *pl);

  wr_graph_rem_edge(graph, &i, &pl);
  //printf("** Removed: %d !!!\n", *pl);

  pl = &l; 
  wr_graph_rem_edge(graph, &j, &pl);

  pl = &l; // cannot remove this twice ! OK
  wr_graph_rem_edge(graph, &j, &pl);
 

  // free
  graph_free(&graph);
  assert(graph == NULL);
  

  return 0;
}
