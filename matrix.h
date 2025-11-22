#ifndef MATRIX_H
#define MATRIX_H

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "graph_analysis.h"

// Structure to represent a matrix
// A matrix is a 2D array of floats (probabilities)
typedef struct
{
    float** data;      // 2D array: data[i][j] is the element at row i, column j
    int rows;          // Number of rows
    int cols;          // Number of columns (should equal rows for square matrices)
} t_matrix;

// Step 1: Matrix calculation functions

/**
 * @brief Creates a transition probability matrix from an adjacency list
 * 
 * This function converts the graph representation (adjacency list) into a matrix
 * where each entry M[i][j] represents the probability of transitioning from
 * state i+1 to state j+1.
 * 
 * @param graph The adjacency list representing the Markov graph
 * @return t_matrix The transition probability matrix
 */
t_matrix createTransitionMatrix(const adjacency_list* graph);

/**
 * @brief Creates an empty matrix filled with zeros
 * 
 * Allocates memory for a matrix of size n x n and initializes all values to 0.0
 * 
 * @param n The size of the square matrix (n x n)
 * @return t_matrix An empty matrix filled with zeros
 */
t_matrix createEmptyMatrix(int n);

/**
 * @brief Copies the values from one matrix to another
 * 
 * Copies all values from source matrix to destination matrix.
 * Both matrices must have the same dimensions.
 * 
 * @param dest The destination matrix (where values will be copied to)
 * @param src The source matrix (where values will be copied from)
 */
void copyMatrix(t_matrix dest, t_matrix src);

/**
 * @brief Multiplies two matrices
 * 
 * Performs matrix multiplication: result = A * B
 * For Markov chains: if A is the current distribution and B is the transition matrix,
 * then result is the distribution after one step.
 * 
 * @param A The first matrix (left operand)
 * @param B The second matrix (right operand)
 * @param result The matrix to store the result (must be pre-allocated)
 */
void multiplyMatrices(t_matrix A, t_matrix B, t_matrix result);

/**
 * @brief Calculates the difference between two matrices
 * 
 * Computes the sum of absolute differences between corresponding elements:
 * diff(M, N) = sum over all i,j of |M[i][j] - N[i][j]|
 * 
 * This is used to check if two matrices are "close enough" (convergence check)
 * 
 * @param M The first matrix
 * @param N The second matrix
 * @return float The sum of absolute differences
 */
float matrixDifference(t_matrix M, t_matrix N);

// Step 2: Properties of Markov graphs

/**
 * @brief Extracts a submatrix corresponding to a specific component
 * 
 * Creates a submatrix that contains only the rows and columns corresponding
 * to vertices that belong to a specific strongly connected component.
 * This is used to analyze each class separately.
 * 
 * @param matrix The original adjacency matrix of the graph
 * @param part The partition of the graph into strongly connected components
 * @param compo_index The index of the component to extract
 * @return t_matrix The submatrix corresponding to the specified component
 */
t_matrix subMatrix(t_matrix matrix, t_partition part, int compo_index);

// Step 3: Periodicity (bonus challenge)

/**
 * @brief Calculates the greatest common divisor (GCD) of an array of integers
 * 
 * This is a helper function for calculating the period of a Markov chain class.
 * The period is the GCD of all return times to a state.
 * 
 * @param vals Array of integer values
 * @param nbvals Number of values in the array
 * @return int The GCD of all values in the array
 */
int gcd(int* vals, int nbvals);

/**
 * @brief Calculates the period of a Markov chain class
 * 
 * The period is the greatest common divisor of all return times to states
 * in the class. A class with period 1 is aperiodic (has a stationary distribution).
 * 
 * @param sub_matrix The transition matrix for a specific class
 * @return int The period of the class
 */
int getPeriod(t_matrix sub_matrix);

// Utility functions

/**
 * @brief Prints a matrix to the console
 * 
 * Displays the matrix in a readable format for debugging and validation
 * 
 * @param matrix The matrix to print
 */
void printMatrix(t_matrix matrix);

/**
 * @brief Frees the memory allocated for a matrix
 * 
 * Deallocates all memory used by the matrix structure
 * 
 * @param matrix The matrix to free
 */
void freeMatrix(t_matrix* matrix);

#endif // MATRIX_H

