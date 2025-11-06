#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"

int main(int argc, char* argv[])
{
    char filename[256];  // Array to store the filename
    
    // Check if a filename was provided as command line argument
    if (argc >= 2)
    {
        // If filename provided, use it
        strcpy(filename, argv[1]);
    }
    else
    {
        // If no filename provided, ask the user to enter it
        printf("Enter the name of the input file: ");
        scanf("%255s", filename);  // Read filename from user (max 255 characters)
    }
    
    printf("\n========================================\n");
    printf("  Markov Graph Project - Part 1\n");
    printf("========================================\n\n");
    
    // STEP 1: Create a graph from file and display it
    printf("STEP 1: Creating graph from file '%s'...\n", filename);
    printf("----------------------------------------\n");
    
    adjacency_list graph = read_graph(filename);
    
    printf("\nGraph loaded successfully!\n");
    printf("Number of vertices: %d\n", graph.num_vertices);
    
    // Display the adjacency list
    display_adjacency_list(graph);
    
    // STEP 2: Check if it's a valid Markov graph
    printf("STEP 2: Checking if graph is a valid Markov graph...\n");
    printf("----------------------------------------\n");
    
    int is_valid = is_markov_graph(graph);
    
    printf("\n");
    
    // STEP 3: Generate Mermaid file for visualization
    printf("STEP 3: Generating Mermaid visualization file...\n");
    printf("----------------------------------------\n");
    
    // Create output filename: extract just the filename part and add .mmd
    // We write to the current directory (where the program runs from)
    char output_filename[256];
    
    // Find the last slash in the filename (to get just the filename part)
    // Example: "data/exemple1.txt" -> we want "exemple1.mmd"
    const char* last_slash = strrchr(filename, '/');
    const char* last_backslash = strrchr(filename, '\\');
    const char* name_start = filename;  // Start with whole filename
    
    // If we found a slash, start after it
    if (last_slash != NULL)
    {
        name_start = last_slash + 1;  // Point to character after the slash
    }
    if (last_backslash != NULL)
    {
        const char* after_backslash = last_backslash + 1;
        // Use whichever comes later (slash or backslash)
        if (after_backslash > name_start)
        {
            name_start = after_backslash;
        }
    }
    
    // Copy the filename part (without the path)
    strcpy(output_filename, name_start);
    
    // Find the dot and replace .txt with .mmd
    // Simple loop to find the dot
    int i = 0;
    while (output_filename[i] != '\0' && output_filename[i] != '.')
    {
        i++;
    }
    
    // If we found a dot, replace everything after it with .mmd
    if (output_filename[i] == '.')
    {
        output_filename[i] = '\0';  // Cut the string at the dot
    }
    
    // Add .mmd extension
    strcat(output_filename, ".mmd");
    
    generate_mermaid_file(graph, output_filename);
    
    printf("\n========================================\n");
    printf("  All steps completed!\n");
    printf("========================================\n");
    printf("\nTo visualize the graph:\n");
    printf("1. Open https://www.mermaidchart.com/\n");
    printf("2. Copy the contents of '%s'\n", output_filename);
    printf("3. Paste into the Mermaid code editor\n");
    printf("4. View your graph!\n\n");
    
    // Free the memory we allocated
    free_adjacency_list(&graph);
    
    return 0;
}
