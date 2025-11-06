#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

// Function to create a new cell
// We allocate memory for a cell, set its values, and return a pointer to it
cell* create_cell(int arrival, float prob)
{
    // Allocate memory for a new cell
    cell* new_cell = (cell*)malloc(sizeof(cell));
    
    // Check if allocation succeeded
    if (new_cell == NULL)
    {
        printf("Error: Could not allocate memory for cell\n");
        exit(EXIT_FAILURE);
    }
    
    // Set the values
    new_cell->arrival_vertex = arrival;
    new_cell->probability = prob;
    new_cell->next = NULL;  // Initially, it points to nothing
    
    return new_cell;
}

// Function to create an empty list
// An empty list is just a list with head = NULL
list create_empty_list(void)
{
    list new_list;
    new_list.head = NULL;  // No cells in the list yet
    return new_list;
}

// Function to add a cell to a list
// We add it at the beginning of the list (simple approach for beginners)
void add_cell_to_list(list* lst, int arrival, float prob)
{
    // Create a new cell with the given values
    cell* new_cell = create_cell(arrival, prob);
    
    // Make the new cell point to what the head was pointing to
    new_cell->next = lst->head;
    
    // Make the head point to the new cell
    lst->head = new_cell;
}

// Function to display a list (for debugging)
// We traverse the list and print each cell's information
void display_list(list lst, int vertex_num)
{
    printf("List for vertex %d: [head @] -> ", vertex_num);
    
    // Start from the head of the list
    cell* current = lst.head;
    
    // Traverse the list until we reach the end (NULL)
    while (current != NULL)
    {
        // Print the current cell's information
        printf("(%d, %.2f)", current->arrival_vertex, current->probability);
        
        // Move to the next cell
        current = current->next;
        
        // If there's a next cell, print an arrow
        if (current != NULL)
        {
            printf(" @-> ");
        }
    }
    
    printf("\n");
}

// Function to create an empty adjacency list
// We allocate an array of lists, and initialize each list as empty
adjacency_list create_empty_adjacency_list(int num_vertices)
{
    adjacency_list adj_list;
    
    // Set the number of vertices
    adj_list.num_vertices = num_vertices;
    
    // Allocate memory for the array of lists
    adj_list.lists = (list*)malloc(num_vertices * sizeof(list));
    
    // Check if allocation succeeded
    if (adj_list.lists == NULL)
    {
        printf("Error: Could not allocate memory for adjacency list\n");
        exit(EXIT_FAILURE);
    }
    
    // Initialize each list as empty
    for (int i = 0; i < num_vertices; i++)
    {
        adj_list.lists[i] = create_empty_list();
    }
    
    return adj_list;
}

// Function to display an adjacency list
// We display each list in the adjacency list
void display_adjacency_list(adjacency_list adj_list)
{
    printf("\n=== Adjacency List ===\n");
    
    // Go through each vertex
    for (int i = 0; i < adj_list.num_vertices; i++)
    {
        // Display the list for vertex i+1 (vertices are numbered from 1, arrays from 0)
        display_list(adj_list.lists[i], i + 1);
    }
    
    printf("======================\n\n");
}

// Function to read a graph from a file
// We read the file line by line and build the adjacency list
adjacency_list read_graph(const char* filename)
{
    FILE* file = fopen(filename, "rt");  // Try to open file in read text mode
    
    // If file not found and filename starts with "data/", try "../data/" instead
    // This handles the case when running from cmake-build-debug directory
    if (file == NULL && strncmp(filename, "data/", 5) == 0)
    {
        // Build new path: "../data/filename"
        char new_path[256];
        strcpy(new_path, "../");
        strcat(new_path, filename);
        file = fopen(new_path, "rt");
    }
    
    // Check if file opened successfully
    if (file == NULL)
    {
        printf("Error: Could not find file '%s'\n", filename);
        printf("Tried: %s\n", filename);
        if (strncmp(filename, "data/", 5) == 0)
        {
            printf("Also tried: ../%s\n", filename);
        }
        printf("\nMake sure the file exists in the correct location.\n");
        exit(EXIT_FAILURE);
    }
    
    int num_vertices;
    int start, end;
    float proba;
    
    // Read the first line: number of vertices
    if (fscanf(file, "%d", &num_vertices) != 1)
    {
        perror("Could not read number of vertices");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    
    // Create an empty adjacency list with the correct number of vertices
    adjacency_list adj_list = create_empty_adjacency_list(num_vertices);
    
    // Read each edge from the file
    // Each line has: start_vertex end_vertex probability
    while (fscanf(file, "%d %d %f", &start, &end, &proba) == 3)
    {
        // Add the edge to the adjacency list
        // Note: vertices in file are 1-based, arrays are 0-based, so we use start-1
        add_cell_to_list(&(adj_list.lists[start - 1]), end, proba);
    }
    
    // Close the file
    fclose(file);
    
    // Return the completed adjacency list
    return adj_list;
}

// Function to check if a graph is a valid Markov graph
// For each vertex, the sum of outgoing probabilities must be 1 (with tolerance 0.99-1.0)
int is_markov_graph(adjacency_list adj_list)
{
    int is_valid = 1;  // Assume it's valid, we'll check
    
    // Go through each vertex
    for (int i = 0; i < adj_list.num_vertices; i++)
    {
        float sum = 0.0;  // Sum of probabilities for this vertex
        
        // Traverse the list for this vertex and sum all probabilities
        cell* current = adj_list.lists[i].head;
        while (current != NULL)
        {
            sum += current->probability;
            current = current->next;
        }
        
        // Check if the sum is between 0.99 and 1.0 (with some tolerance for floating point)
        if (sum < 0.99 || sum > 1.01)  // Using 1.01 to account for floating point errors
        {
            is_valid = 0;
            printf("The graph is not a Markov graph\n");
            printf("The sum of the probabilities of vertex %d is %.4f\n", i + 1, sum);
        }
    }
    
    // If we got here and it's still valid, print success message
    if (is_valid)
    {
        printf("The graph is a Markov graph\n");
    }
    
    return is_valid;
}

// Function to get ID string from vertex number
// Converts 1->"A", 2->"B", ..., 26->"Z", 27->"AA", 28->"AB", etc.
// This is like Excel column names: A, B, C, ..., Z, AA, AB, AC, ...
char* get_id(int vertex_num)
{
    // Static buffer: this variable keeps its value between function calls
    // This is needed because we return a pointer to it
    static char buffer[10];
    char temp[10];  // Temporary array to build the string backwards
    int index = 0;  // Current position in temp array
    
    // Convert from 1-based (vertex 1, 2, 3...) to 0-based (0, 1, 2...)
    // This makes the math easier
    vertex_num--;
    
    // Build the string backwards (like counting in base-26)
    // Example: vertex 27 (which is 26 in 0-based)
    //   First: 26 % 26 = 0, so we get 'A' (0 + 'A')
    //   Then: 26 / 26 - 1 = 0, so we get another 'A'
    //   Result backwards: "AA", which reversed is "AA"
    while (vertex_num >= 0)
    {
        // Get the last "digit" (0-25) and convert to letter (A-Z)
        temp[index] = 'A' + (vertex_num % 26);
        index++;
        
        // Move to the next "digit" (divide by 26 and subtract 1)
        vertex_num = (vertex_num / 26) - 1;
    }
    
    // The string in temp is backwards, so we reverse it
    // Example: temp has "AA" backwards, we reverse to get "AA"
    for (int j = 0; j < index; j++)
    {
        buffer[j] = temp[index - j - 1];
    }
    buffer[index] = '\0';  // Add the end-of-string marker
    
    return buffer;
}

// Function to generate a Mermaid file from the graph
// Creates a file that can be used with Mermaid to visualize the graph
void generate_mermaid_file(adjacency_list adj_list, const char* output_filename)
{
    FILE* file = fopen(output_filename, "wt");  // Open file in write text mode
    
    // Check if file opened successfully
    if (file == NULL)
    {
        perror("Could not open file for writing");
        exit(EXIT_FAILURE);
    }
    
    // Write the configuration header
    fprintf(file, "---\n");
    fprintf(file, "config:\n");
    fprintf(file, " layout: elk\n");
    fprintf(file, " theme: neo\n");
    fprintf(file, " look: neo\n");
    fprintf(file, "---\n");
    fprintf(file, "flowchart LR\n");
    
    // Write vertex declarations
    // For each vertex, we declare it with its ID and number
    for (int i = 0; i < adj_list.num_vertices; i++)
    {
        char* vertex_id = get_id(i + 1);
        fprintf(file, "%s((%d))\n", vertex_id, i + 1);
    }
    
    // Write edges
    // For each vertex, we go through its list and write each edge
    for (int i = 0; i < adj_list.num_vertices; i++)
    {
        // Get the ID for the source vertex and copy it immediately
        // (because get_id uses a static buffer that gets overwritten)
        char* from_id_ptr = get_id(i + 1);
        char from_id[10];
        strcpy(from_id, from_id_ptr);  // Copy the string so it doesn't get overwritten
        
        // Traverse the list for this vertex
        cell* current = adj_list.lists[i].head;
        while (current != NULL)
        {
            // Get the ID for the destination vertex
            char* to_id_ptr = get_id(current->arrival_vertex);
            char to_id[10];
            strcpy(to_id, to_id_ptr);  // Copy the string so it doesn't get overwritten
            
            // Write the edge: from_id -->|probability|to_id
            fprintf(file, "%s -->|%.4f|%s\n", from_id, current->probability, to_id);
            
            current = current->next;
        }
    }
    
    // Close the file
    fclose(file);
    
    printf("Mermaid file '%s' generated successfully!\n", output_filename);
}

// Function to free memory allocated for an adjacency list
// We need to free all cells in all lists, then free the array of lists
void free_adjacency_list(adjacency_list* adj_list)
{
    // Go through each vertex's list
    for (int i = 0; i < adj_list->num_vertices; i++)
    {
        // Free all cells in this list
        cell* current = adj_list->lists[i].head;
        while (current != NULL)
        {
            cell* next = current->next;  // Save pointer to next cell
            free(current);               // Free current cell
            current = next;               // Move to next cell
        }
    }
    
    // Free the array of lists
    free(adj_list->lists);
    
    // Reset the structure
    adj_list->lists = NULL;
    adj_list->num_vertices = 0;
}
