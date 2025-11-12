#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "graph_analysis.h"

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
    printf("  Steps 1 to 3 completed!\n");
    printf("========================================\n");
    printf("\nTo visualize the graph:\n");
    printf("1. Open https://www.mermaidchart.com/\n");
    printf("2. Copy the contents of '%s'\n", output_filename);
    printf("3. Paste into the Mermaid code editor\n");
    printf("4. View your graph!\n\n");

    printf("STEP 4: Grouping vertices into strongly connected classes (Tarjan)...\n");
    printf("------------------------------------------------------------------\n");

    int* vertex_to_class = NULL;
    t_partition partition = tarjan_partition_graph(&graph, &vertex_to_class);
    print_partition(&partition);

    printf("STEP 5: Building class links and Hasse diagram...\n");
    printf("-----------------------------------------------\n");

    t_link_array direct_links = build_link_array(&partition, &graph, vertex_to_class);
    print_link_array(&direct_links, &partition);

    t_link_array hasse_links = clone_link_array(&direct_links);
    removeTransitiveLinks(&hasse_links);

    char hasse_filename[256];
    snprintf(hasse_filename, sizeof(hasse_filename), "classes_%s", output_filename);
    export_hasse_mermaid(&partition, &hasse_links, hasse_filename);

    printf("\nSTEP 6: Analysing class and graph properties...\n");
    printf("----------------------------------------------\n");

    graph_characteristics characteristics = compute_graph_characteristics(&partition, &direct_links);
    print_graph_characteristics(&partition, &characteristics);

    printf("\n========================================\n");
    printf("  Part 2 analysis completed!\n");
    printf("========================================\n\n");



    free(vertex_to_class);
    free_link_array(&direct_links);
    free_link_array(&hasse_links);
    free_partition(&partition);
    free_graph_characteristics(&characteristics);
    free_adjacency_list(&graph);

    printf("Program finished.\n\n");

    return 0;
}
