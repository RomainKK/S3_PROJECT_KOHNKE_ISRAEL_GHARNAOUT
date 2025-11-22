#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "graph_analysis.h"
#include "matrix.h"

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

    // ========================================
    // PART 3: Matrix calculations and distributions
    // ========================================
    
    printf("\n========================================\n");
    printf("  Markov Graph Project - Part 3\n");
    printf("========================================\n\n");
    
    // STEP 1: Matrix calculations
    printf("STEP 1: Matrix calculations...\n");
    printf("-------------------------------\n");
    
    // Create the transition probability matrix from the graph
    printf("Creating transition probability matrix M...\n");
    t_matrix M = createTransitionMatrix(&graph);
    printf("Transition matrix M:\n");
    printMatrix(M);
    
    // Calculate M^3
    printf("Calculating M^3...\n");
    t_matrix M_power = createEmptyMatrix(graph.num_vertices);
    t_matrix M_temp = createEmptyMatrix(graph.num_vertices);
    t_matrix M_result = createEmptyMatrix(graph.num_vertices);
    
    // Start with M^1
    copyMatrix(M_power, M);
    
    // Calculate M^2 = M * M
    multiplyMatrices(M_power, M, M_result);
    copyMatrix(M_temp, M_result);
    
    // Calculate M^3 = M^2 * M
    multiplyMatrices(M_temp, M, M_result);
    copyMatrix(M_power, M_result);
    
    printf("Matrix M^3:\n");
    printMatrix(M_power);
    
    // Calculate M^7
    printf("Calculating M^7...\n");
    // We already have M^3, so we need M^4, M^5, M^6, M^7
    // M^4 = M^3 * M
    multiplyMatrices(M_power, M, M_result);
    copyMatrix(M_power, M_result);
    // M^5 = M^4 * M
    multiplyMatrices(M_power, M, M_result);
    copyMatrix(M_power, M_result);
    // M^6 = M^5 * M
    multiplyMatrices(M_power, M, M_result);
    copyMatrix(M_power, M_result);
    // M^7 = M^6 * M
    multiplyMatrices(M_power, M, M_result);
    copyMatrix(M_power, M_result);
    
    printf("Matrix M^7:\n");
    printMatrix(M_power);
    
    // Find convergence: calculate M^n until difference between M^n and M^(n-1) < epsilon
    printf("Finding convergence (difference < 0.01)...\n");
    float epsilon = 0.01f;
    int n = 1;
    t_matrix M_prev = createEmptyMatrix(graph.num_vertices);
    copyMatrix(M_prev, M);
    copyMatrix(M_power, M);
    
    float diff = 1.0f;
    int max_iterations = 100;  // Safety limit to avoid infinite loops
    
    while (diff > epsilon && n < max_iterations)
    {
        // Calculate next power: M_power = M_power * M
        multiplyMatrices(M_power, M, M_result);
        copyMatrix(M_temp, M_power);  // Save current M_power before updating
        copyMatrix(M_power, M_result);  // Update M_power to next power
        
        // Calculate difference between M^n and M^(n-1)
        diff = matrixDifference(M_power, M_temp);
        
        // Update for next iteration
        copyMatrix(M_prev, M_power);
        n++;
    }
    
    if (n < max_iterations)
    {
        printf("Convergence reached at M^%d (difference = %.6f)\n", n, diff);
        printf("Converged matrix M^%d:\n", n);
        printMatrix(M_power);
    }
    else
    {
        printf("Warning: Convergence not reached after %d iterations (difference = %.6f)\n", 
               max_iterations, diff);
        printf("This graph may not have a stationary distribution.\n");
    }
    
    // STEP 2: Properties of Markov graphs - Stationary distributions
    printf("\nSTEP 2: Calculating stationary distributions for each class...\n");
    printf("------------------------------------------------------------\n");
    
    // For each persistent class, calculate the stationary distribution
    for (int i = 0; i < partition.class_count; i++)
    {
        if (characteristics.class_is_persistent[i])
        {
            printf("\nClass %s (persistent):\n", partition.classes[i].name);
            
            // Extract submatrix for this class
            t_matrix sub = subMatrix(M, partition, i);
            printf("Submatrix for class %s:\n", partition.classes[i].name);
            printMatrix(sub);
            
            // Calculate powers until convergence
            t_matrix sub_power = createEmptyMatrix(sub.rows);
            t_matrix sub_result = createEmptyMatrix(sub.rows);
            t_matrix sub_prev = createEmptyMatrix(sub.rows);
            
            copyMatrix(sub_power, sub);
            copyMatrix(sub_prev, sub);
            
            float sub_diff = 1.0f;
            int sub_n = 1;
            
            while (sub_diff > epsilon && sub_n < max_iterations)
            {
                multiplyMatrices(sub_power, sub, sub_result);
                copyMatrix(sub_prev, sub_power);
                copyMatrix(sub_power, sub_result);
                
                sub_diff = matrixDifference(sub_power, sub_prev);
                sub_n++;
            }
            
            if (sub_n < max_iterations)
            {
                printf("Stationary distribution for class %s (from row 0 of M^%d):\n", 
                       partition.classes[i].name, sub_n);
                printf("  ");
                for (int j = 0; j < sub_power.cols; j++)
                {
                    printf("State %d: %.4f  ", partition.classes[i].members[j], sub_power.data[0][j]);
                }
                printf("\n");
            }
            else
            {
                printf("Warning: Could not find stationary distribution for class %s\n", 
                       partition.classes[i].name);
            }
            
            freeMatrix(&sub);
            freeMatrix(&sub_power);
            freeMatrix(&sub_result);
            freeMatrix(&sub_prev);
        }
        else
        {
            printf("\nClass %s (transient): limiting distribution is zero\n", 
                   partition.classes[i].name);
        }
    }
    
    // STEP 3 (bonus): Periodicity
    printf("\nSTEP 3 (bonus): Calculating periods for each class...\n");
    printf("------------------------------------------------------\n");
    
    for (int i = 0; i < partition.class_count; i++)
    {
        if (characteristics.class_is_persistent[i])
        {
            // Extract submatrix for this class
            t_matrix sub = subMatrix(M, partition, i);
            
            // Calculate period
            int period = getPeriod(sub);
            
            printf("Class %s: period = %d\n", partition.classes[i].name, period);
            
            if (period == 1)
            {
                printf("  -> This class is aperiodic (has a unique stationary distribution)\n");
            }
            else
            {
                printf("  -> This class is periodic with period %d\n", period);
                printf("  -> It may have multiple periodic stationary distributions\n");
            }
            
            freeMatrix(&sub);
        }
    }
    
    // Free matrix memory
    freeMatrix(&M);
    freeMatrix(&M_power);
    freeMatrix(&M_temp);
    freeMatrix(&M_result);
    freeMatrix(&M_prev);
    
    printf("\n========================================\n");
    printf("  Part 3 analysis completed!\n");
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
