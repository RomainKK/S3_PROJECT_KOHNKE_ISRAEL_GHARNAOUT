#ifndef GRAPH_ANALYSIS_H
#define GRAPH_ANALYSIS_H

#include "utils.h"
#include "hasse.h"

typedef struct
{
    int identifier;
    int index;
    int low_link;
    int on_stack;
} t_tarjan_vertex;

typedef struct
{
    char name[8];
    int* members;
    int member_count;
    int capacity;
} t_class;

typedef struct
{
    t_class* classes;
    int class_count;
    int capacity;
} t_partition;

typedef struct
{
    int* class_is_persistent;
    int has_absorbing_state;
    int is_irreducible;
} graph_characteristics;

t_partition tarjan_partition_graph(const adjacency_list* graph, int** vertex_to_class);
void print_partition(const t_partition* partition);
void free_partition(t_partition* partition);

t_link_array build_link_array(const t_partition* partition, const adjacency_list* graph, const int* vertex_to_class);
t_link_array clone_link_array(const t_link_array* source);
void free_link_array(t_link_array* link_array);
void print_link_array(const t_link_array* link_array, const t_partition* partition);
void export_hasse_mermaid(const t_partition* partition, const t_link_array* link_array, const char* filename);

graph_characteristics compute_graph_characteristics(const t_partition* partition, const t_link_array* link_array);
void print_graph_characteristics(const t_partition* partition, const graph_characteristics* characteristics);
void free_graph_characteristics(graph_characteristics* characteristics);

#endif

