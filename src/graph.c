/*
 * In this File we define various function & API regarding Graph 
 * for C Library
 * 
 * Author: Kaushik, Koneru
 * Email: konerukaushik@gmail.com
 */

/*
 *
 * ToDo:
 *  -> Need to replace Shallow Copying with Depth Copying
 *  -> Need to Modularize the Graph_add_edge directed version
 */

#include "graph.h"

/*
 * Function:
 * Graph_add_edge_to_vertex
 *
 * In this function we append new_edge to
 * the adjacency list of vertices list
 *
 * Input: Graph_edges_t     adjacency_list
 *        Graph_edges_t     new_edge
 * output:
 *        Graph_edges_t* (Graph Vertex adjacencyList)
 */
Graph_edges_t *
Graph_add_edge_to_vertex(Graph_edges_t  *adjacency_list,
                         Graph_edges_t  *new_edge) {

  Graph_edges_t      *runner; /* To Parse through to vertices_list */
  
  if (adjacency_list == NULL) {
    free(runner);
    return new_edge;
  }

  runner = adjacency_list;

  /* Parse till we reach end of Adjacency List */
  while(runner->next != NULL) {
    runner = runner->next;
  }

  /* Append the New edge to List */
  runner->next = new_edge;

  return adjacency_list;
}

/* 
 * Function:
 * Graph_add_edge_template
 *
 * In this function we create new Edge Object
 * to append into the vertices Adjacency List
 * So, We consider destination and weight only
 *
 * Input:
 *      vertex_number_t destination (Destination Vertex)
 *      edge_weight_t   weight  (Weight of edge)
 * Output:
 *      Graph_edges_t  (new edge Object)
 */
Graph_edges_t *
Graph_add_edge_template(vertex_number_t Destination,
                        edge_weight_t   weight) {

    Graph_edges_t      *temp;

    temp = (Graph_edges_t *)malloc(sizeof(Graph_edges_t));

    if (temp == NULL) {
      LOG_ERR("Unable to allocate memory for Edge with Dest:%d",Destination);
      goto destroy;
    }

    temp->target = Destination;
    temp->weight = weight;
    temp->next   = NULL;

    return temp;
    
destroy:
    free(temp);
    return NULL;
}

/*
 * Function: Graph_add_edge
 *
 * In this function we add edges 
 * To a Specific Graph
 *
 * Input:
 *      G <-- Graph In which we Need to Append the Edge
 *      Source <-- This will a 
 *      Destination <-- 
 *      is_directed <-- if then add to specific one vertices
 *                      else add to both vertices
 *                     
 */
Graph_t *
Graph_add_edge(Graph_t *G, vertex_number_t S, vertex_number_t D,  
                edge_weight_t weight, bool is_directed) {


  Graph_edges_t        *new_edge;
  Graph_vertices_t    *vertices_list = G->vertices_list;
  int                  found = 0;

  if (vertices_list == NULL) {
    LOG_ERR("No Existing Vertices Present in Graph");
    goto destroy;
  }

  new_edge   =  Graph_add_edge_template(D, weight);
  if (new_edge == NULL) {
    LOG_ERR("Unable to Create edge Template for Source %d - Destination %d\n",S,D);
    goto destroy;
  }

  while(vertices_list != NULL && vertices_list->interface_number != S) {
      vertices_list = vertices_list->next;
  }

  if (vertices_list == NULL || vertices_list->interface_number != S) {
    LOG_ERR("Unable to find vertex: %d",S);
    goto destroy;
  }

  /* If we are unable to add certain edge, Notify User
   * and Proceed to execute further
   */
  vertices_list->adjacency_list = Graph_add_edge_to_vertex(vertices_list->adjacency_list, new_edge);

  if (!is_directed) {
      new_edge = NULL;
      new_edge = Graph_add_edge_template(S, weight);
      if (new_edge == NULL) {
        LOG_ERR("Unable to Create edge Template for Source %d - Destination %d\n",D,S);
        goto destroy;
      }

      new_edge   =  Graph_add_edge_template(D, weight);
      if (new_edge == NULL) {
        LOG_ERR("Unable to Create edge Template for Source %d - Destination %d\n",D,S);
        goto destroy;
      }

      while(vertices_list != NULL && vertices_list->interface_number != D) {
        vertices_list = vertices_list->next;
      }

      if (vertices_list == NULL || vertices_list->interface_number != D) {
        LOG_ERR("Unable to find vertex: %d",D);
        goto destroy;
      }

      /* If we are unable to add certain edge, Notify User
       * and Proceed to execute further
       */
      vertices_list->adjacency_list = Graph_add_edge_to_vertex(vertices_list->adjacency_list, new_edge);
  }

destroy:
  free(new_edge);
  free(vertices_list);
  return NULL;
}
  

/*
 * Function: Graph_add_vertices()
 *
 * In this function we create 
 * single vertex and return it
 *
 * Input : none
 * output: Graph_vertices_t object or NULL
 */
Graph_vertices_t *
Graph_add_vertices_template() {
  
    Graph_vertices_t     *V;

    V = (Graph_vertices_t *)malloc(sizeof(Graph_vertices_t));
    if (V == NULL) {
      LOG_ERR("Unable to assign Memory for Vertices");
      goto destroy;
    }

    /* Initialize V with template */
    V->interface_number   = NaN;
    V->is_visited         = FALSE;
    V->adjacency_list     = NULL;
    V->min_distance       = NaN;
    V->next               = NULL;


    return V;

destroy:
    free(V);
    return NULL;
}

/*
 * Function: Graph_add_vertices
 *
 * In this function we create 
 * single vertex and return it
 *
 * Input : N <- No Of vertices to create
 *         G <- To Which we need to append N vertices
 * output: Graph_vertices_t object or NULL
 */
Graph_vertices_t *
Graph_add_vertices(Graph_t *G, int no_of_vertices) {
  
    Graph_vertices_t     *V        = NULL;
    Graph_vertices_t     *runner   = NULL;
    int                   iterator = G->total_vertices;

    V = G->vertices_list;

    while(V != NULL && V->next != NULL) {
      V = V->next;
    }

    while(iterator < no_of_vertices + G->total_vertices) {
      LOG_DEBUG("Memory Appending for vertices %d\n",iterator);
      LOG_DEBUG("Total required vertices %d, Present vertices %d",no_of_vertices, G->total_vertices);

      runner  = Graph_add_vertices_template();
      if (runner == NULL) {
        LOG_ERR("Runner is NULL for iterator %d",iterator);
        goto destroy;
      }

      runner->interface_number = iterator;

      if (V == NULL) {
          V                = runner;
          G->vertices_list = V;
      } else {
          V->next = runner;
          V       = V->next;
      }

      runner = runner->next;

      iterator++;
    }

    G->total_vertices += iterator;

    return G->vertices_list;

destroy:
    free(V);
    free(runner);
    return G->vertices_list;;
}

/* 
 * In this function we create 
 * Graph Object and initiate basic template
 *
 * Input : none
 * Output: Graph_t Object
 */
Graph_t *
Graph_init_template() {

    Graph_t         *G = NULL;

    /* 
     * Allocate Memory for Graph Object 
     */
    G   =   (Graph_t *) malloc(sizeof(Graph_t));
    if (G == NULL) {
        LOG_ERR("Unable to allocate memory");
        goto destroy;
    }

    /* 
     * Initialize variables
     */
    G->total_vertices   =   0;
    G->total_edges      =   0;
    G->vertices_list    =   NULL;
    G->edges_list       =   NULL;
    G->source           =   NaN;
    G->is_directed      =   FALSE;

    return G;

destroy:
    free(G);
    return NULL;
}


/* 
 * In this function we create 
 * Graph Object and initiate basic template
 * and Initialize N Vertices and 
 * set if it is a Directed or Not (Passed as Argument)
 *
 * Input : int no_of_vertices
 *         bool is_directed
 * Output: Graph_t Object
 */
Graph_t *
Graph_init(int no_of_vertices, bool is_directed) {
    
    Graph_t             *G      = NULL;

    G   =   Graph_init_template();
    if(G == NULL) {
        LOG_ERR("Unable to Initialize %d vertices",no_of_vertices);
        goto destroy;
    }

    Graph_add_vertices(G, no_of_vertices);
    if (G->vertices_list  == NULL) {
      LOG_ERR("Unable to create %d vertices",no_of_vertices);
      goto destroy;
    }

    G->is_directed = is_directed; 

    return G;

destroy:
    free(G);
    return NULL;
}


/*
 * Function
 * Graph_dump_vertices
 *
 * In this function we display Vertices with 
 * Adjacency List
 * 
 * Input:
 *      Graph_vertices_t  - Vertices
 *      print_adjacency   - True/False (If adjacency needs
 *                                      to be printed)
 * Output:
 *      no-return
 */
void
Graph_dump_vertices(Graph_vertices_t *V, bool print_adjacency) {
  
    if (V == NULL) {
      return;
    }

    printf("-------------------------\n");
    printf("| Vertex ID : %4d      |\n",V->interface_number);
    printf("| is_visited: %s     |\n",V->is_visited?"TRUE":"FALSE");
    printf("| min_dis   : %s       |\n",(V->min_distance == NaN)?"NaN":(char *)(V->min_distance));
    printf("-------------------------\n");

    return;
}

/*
 * Function:
 * Graph_display_graph
 *
 * In this function we display complete graph 
 * Information
 *
 * Input: 
 *      Graph_t  - Graph Pointer
 *
 * Output:
 *      NULL
 */
 void
 Graph_display_graph(Graph_t *G) {
  
   Graph_vertices_t         *V_parse;

   if (G == NULL) {
    LOG_INFO("Provided Graph to display is NULL");
    goto destroy;
   }

   if (G->total_vertices == 0 ) {
     LOG_INFO("There are no vertices in the Graph");
     goto destroy;
   }

   if (G->vertices_list == NULL) {
      LOG_ERR("Unable to retrieve interfaces info from Graph");
      goto destroy;
   }

   V_parse  = G->vertices_list;

   while (V_parse != NULL) {
      Graph_dump_vertices(V_parse, FALSE);
      V_parse = V_parse->next;
   }
 
destroy:

   free(V_parse);
   return;
 }
