#include <stdlib.h>
#include "array2D.h"

/**
 * Allocates a 2D array.
 * @param lines
 * @param cols
 */
double** malloc2D(int lines, int cols) {
    double*  arrayBlock = malloc(lines * cols * sizeof(double));
    double** arrayLines = malloc(lines * sizeof(double*));
    
    for (int i = 0 ; i < lines ; i++) {
    	arrayLines[i] = &arrayBlock[i*cols];
    }
    
    return arrayLines;
}

/**
 * Frees a 2D array.
 * @param array
 */
void free2D(double** array) {
	free(array[0]);
	free(array);
}
