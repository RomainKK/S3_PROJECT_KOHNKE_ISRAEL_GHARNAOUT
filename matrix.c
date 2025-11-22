#include "matrix.h"
#include <math.h>

// Function to create a transition probability matrix from an adjacency list
// We go through each vertex and its outgoing edges, and fill the matrix
t_matrix createTransitionMatrix(const adjacency_list* graph)
{
    int n = graph->num_vertices;
    t_matrix matrix = createEmptyMatrix(n);
    
    // Go through each vertex (row in the matrix)
    for (int i = 0; i < n; i++)
    {
        // Traverse the adjacency list for vertex i+1
        // (vertices are numbered 1-based in the graph, but arrays are 0-based)
        cell* current = graph->lists[i].head;
        
        while (current != NULL)
        {
            // The edge goes from vertex i+1 to current->arrival_vertex
            // In the matrix, this is row i, column (arrival_vertex - 1)
            int j = current->arrival_vertex - 1;  // Convert to 0-based index
            matrix.data[i][j] = current->probability;
            
            // Move to the next edge
            current = current->next;
        }
    }
    
    return matrix;
}

// Function to create an empty matrix filled with zeros
// We allocate a 2D array and initialize all values to 0.0
t_matrix createEmptyMatrix(int n)
{
    t_matrix matrix;
    matrix.rows = n;
    matrix.cols = n;
    
    // Allocate memory for the array of row pointers
    matrix.data = (float**)malloc(n * sizeof(float*));
    if (matrix.data == NULL)
    {
        printf("Error: cannot allocate memory for matrix rows\n");
        exit(EXIT_FAILURE);
    }
    
    // Allocate memory for each row
    for (int i = 0; i < n; i++)
    {
        matrix.data[i] = (float*)malloc(n * sizeof(float));
        if (matrix.data[i] == NULL)
        {
            printf("Error: cannot allocate memory for matrix row %d\n", i);
            exit(EXIT_FAILURE);
        }
        
        // Initialize all values to 0.0
        for (int j = 0; j < n; j++)
        {
            matrix.data[i][j] = 0.0f;
        }
    }
    
    return matrix;
}

// Function to copy the values from one matrix to another
// We go through each element and copy it
void copyMatrix(t_matrix dest, t_matrix src)
{
    // Check that dimensions match
    if (dest.rows != src.rows || dest.cols != src.cols)
    {
        printf("Error: cannot copy matrices of different sizes\n");
        return;
    }
    
    // Copy each element
    for (int i = 0; i < src.rows; i++)
    {
        for (int j = 0; j < src.cols; j++)
        {
            dest.data[i][j] = src.data[i][j];
        }
    }
}

// Function to multiply two matrices
// Matrix multiplication: result[i][j] = sum over k of (A[i][k] * B[k][j])
// For Markov chains: if we have distribution Π and transition matrix M,
// then Π * M gives the distribution after one step
void multiplyMatrices(t_matrix A, t_matrix B, t_matrix result)
{
    // Check that dimensions are compatible
    // For A * B to work, A must have n columns and B must have n rows
    if (A.cols != B.rows)
    {
        printf("Error: incompatible matrix dimensions for multiplication\n");
        return;
    }
    
    // Check that result matrix has correct dimensions
    if (result.rows != A.rows || result.cols != B.cols)
    {
        printf("Error: result matrix has wrong dimensions\n");
        return;
    }
    
    // Perform matrix multiplication
    // For each row i of the result
    for (int i = 0; i < result.rows; i++)
    {
        // For each column j of the result
        for (int j = 0; j < result.cols; j++)
        {
            // Calculate result[i][j] = sum of A[i][k] * B[k][j] for all k
            float sum = 0.0f;
            for (int k = 0; k < A.cols; k++)
            {
                sum += A.data[i][k] * B.data[k][j];
            }
            result.data[i][j] = sum;
        }
    }
}

// Function to calculate the difference between two matrices
// We compute the sum of absolute differences: sum over all i,j of |M[i][j] - N[i][j]|
float matrixDifference(t_matrix M, t_matrix N)
{
    // Check that dimensions match
    if (M.rows != N.rows || M.cols != N.cols)
    {
        printf("Error: cannot compute difference of matrices with different sizes\n");
        return -1.0f;
    }
    
    float diff = 0.0f;
    
    // Go through each element and add the absolute difference
    for (int i = 0; i < M.rows; i++)
    {
        for (int j = 0; j < M.cols; j++)
        {
            float abs_diff = fabsf(M.data[i][j] - N.data[i][j]);
            diff += abs_diff;
        }
    }
    
    return diff;
}

// Function to extract a submatrix for a specific component
// We create a new matrix containing only rows and columns for vertices in the component
t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index)
{
    // Get the component we want to extract
    t_class* compo = &part.classes[compo_index];
    int compo_size = compo->member_count;
    
    // Create a new matrix for the submatrix
    t_matrix sub = createEmptyMatrix(compo_size);
    
    // Map original vertex indices to submatrix indices
    // For example, if component has vertices {2, 5, 7}, then:
    // - vertex 2 maps to index 0 in submatrix
    // - vertex 5 maps to index 1 in submatrix
    // - vertex 7 maps to index 2 in submatrix
    
    // Fill the submatrix
    // For each row in the submatrix (corresponding to a vertex in the component)
    for (int i = 0; i < compo_size; i++)
    {
        // Get the original vertex number (1-based)
        int orig_row = compo->members[i];
        // Convert to 0-based index for the original matrix
        int orig_row_idx = orig_row - 1;
        
        // For each column in the submatrix
        for (int j = 0; j < compo_size; j++)
        {
            // Get the original vertex number (1-based)
            int orig_col = compo->members[j];
            // Convert to 0-based index for the original matrix
            int orig_col_idx = orig_col - 1;
            
            // Copy the value from the original matrix to the submatrix
            sub.data[i][j] = matrix.data[orig_row_idx][orig_col_idx];
        }
    }
    
    return sub;
}

// Function to calculate the greatest common divisor (GCD) of an array of integers
// This uses the Euclidean algorithm
int gcd(int* vals, int nbvals)
{
    // If array is empty, return 0
    if (nbvals == 0)
    {
        return 0;
    }
    
    // Start with the first value
    int result = vals[0];
    
    // For each remaining value, compute GCD with current result
    for (int i = 1; i < nbvals; i++)
    {
        int a = result;
        int b = vals[i];
        
        // Euclidean algorithm: GCD(a, b) = GCD(b, a % b) until b == 0
        while (b != 0)
        {
            int temp = b;
            b = a % b;  // Remainder when a is divided by b
            a = temp;
        }
        
        // After the loop, a is the GCD of the two numbers
        result = a;
    }
    
    return result;
}

// Function to calculate the period of a Markov chain class
// The period is the GCD of all return times (when we can return to a state)
int getPeriod(t_matrix sub_matrix)
{
    int n = sub_matrix.rows;
    
    // Array to store the periods (return times) we find
    int* periods = (int*)malloc(n * sizeof(int));
    if (periods == NULL)
    {
        printf("Error: cannot allocate memory for periods array\n");
        exit(EXIT_FAILURE);
    }
    
    int period_count = 0;  // How many periods we've found
    
    // We'll compute powers of the matrix: M, M^2, M^3, ..., M^n
    // If M^k[i][i] > 0, it means we can return to state i in k steps
    t_matrix power_matrix = createEmptyMatrix(n);
    t_matrix result_matrix = createEmptyMatrix(n);
    
    // Start with M^1 (the matrix itself)
    copyMatrix(power_matrix, sub_matrix);
    
    // Check powers from 1 to n
    for (int cpt = 1; cpt <= n; cpt++)
    {
        // Check if any diagonal element is non-zero
        // If M^cpt[i][i] > 0, we can return to state i in cpt steps
        int diag_nonzero = 0;
        for (int i = 0; i < n; i++)
        {
            if (power_matrix.data[i][i] > 0.0f)
            {
                diag_nonzero = 1;
                break;  // Found at least one, no need to check more
            }
        }
        
        // If we found a return time, add it to our list
        if (diag_nonzero)
        {
            periods[period_count] = cpt;
            period_count++;
        }
        
        // If we haven't reached the last power yet, compute the next power
        if (cpt < n)
        {
            // Compute power_matrix * sub_matrix and store in result_matrix
            multiplyMatrices(power_matrix, sub_matrix, result_matrix);
            // Copy result back to power_matrix for next iteration
            copyMatrix(power_matrix, result_matrix);
        }
    }
    
    // Calculate the GCD of all return times we found
    int period = gcd(periods, period_count);
    
    // Free allocated memory
    free(periods);
    freeMatrix(&power_matrix);
    freeMatrix(&result_matrix);
    
    return period;
}

// Function to print a matrix (for debugging and validation)
void printMatrix(t_matrix matrix)
{
    printf("Matrix (%d x %d):\n", matrix.rows, matrix.cols);
    for (int i = 0; i < matrix.rows; i++)
    {
        printf("  ");
        for (int j = 0; j < matrix.cols; j++)
        {
            printf("%.4f  ", matrix.data[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Function to free the memory allocated for a matrix
void freeMatrix(t_matrix* matrix)
{
    if (matrix->data == NULL)
    {
        return;
    }
    
    // Free each row
    for (int i = 0; i < matrix->rows; i++)
    {
        free(matrix->data[i]);
        matrix->data[i] = NULL;
    }
    
    // Free the array of row pointers
    free(matrix->data);
    matrix->data = NULL;
    matrix->rows = 0;
    matrix->cols = 0;
}

