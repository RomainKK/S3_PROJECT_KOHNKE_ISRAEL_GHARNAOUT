#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>

// Structure for a cell (represents an edge)
// Each cell contains: arrival vertex, probability, and pointer to next cell
typedef struct cell {
    int arrival_vertex;        // The destination vertex
    float probability;         // The probability of this edge
    struct cell* next;         // Pointer to the next cell in the list
} cell;

// Structure for a list (stores all edges exiting from a vertex)
// Each list has a head pointer pointing to the first cell
typedef struct list {
    struct cell* head;         // Pointer to the first cell (or NULL if list is empty)
} list;

// Structure for an adjacency list (the whole graph)
// Contains an array of lists (one list per vertex) and the number of vertices
typedef struct adjacency_list {
    list* lists;              // Array of lists (one per vertex)
    int num_vertices;          // Number of vertices in the graph
} adjacency_list;

// Function to create a new cell
// Parameters: arrival vertex number, probability value
// Returns: pointer to the newly created cell
cell* create_cell(int arrival, float prob);

// Function to create an empty list
// Returns: a new list with head = NULL
list create_empty_list(void);

// Function to add a cell to a list
// Parameters: pointer to the list, arrival vertex, probability
// The new cell is added at the beginning of the list
void add_cell_to_list(list* lst, int arrival, float prob);

// Function to display a list (for debugging)
// Parameters: the list to display, vertex number (for display purposes)
void display_list(list lst, int vertex_num);

// Function to create an empty adjacency list
// Parameters: number of vertices
// Returns: an adjacency list with all lists initialized as empty
adjacency_list create_empty_adjacency_list(int num_vertices);

// Function to display an adjacency list (for debugging)
// Parameters: the adjacency list to display
void display_adjacency_list(adjacency_list adj_list);

// Function to read a graph from a file
// Parameters: filename (path to the data file)
// Returns: a complete adjacency list representing the graph
adjacency_list read_graph(const char* filename);

// Function to check if a graph is a valid Markov graph
// A Markov graph must have: sum of outgoing probabilities per vertex = 1 (with tolerance 0.99-1.0)
// Parameters: the adjacency list to check
// Returns: 1 if valid, 0 if not valid
int is_markov_graph(adjacency_list adj_list);

// Function to generate a Mermaid file from the graph
// Parameters: the adjacency list, output filename
// Creates a .mmd file that can be used with Mermaid to visualize the graph
void generate_mermaid_file(adjacency_list adj_list, const char* output_filename);

// Function to get ID string (A, B, C, ..., Z, AA, AB, ...) from vertex number
// Parameters: vertex number (1-based)
// Returns: string like "A" for 1, "B" for 2, etc.
char* get_id(int vertex_num);

// Function to free memory allocated for an adjacency list
// Parameters: pointer to the adjacency list
void free_adjacency_list(adjacency_list* adj_list);

#endif