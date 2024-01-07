#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fasta.h" // Include the FASTA data structure and functions

#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000) // For converting clock ticks to milliseconds

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s -R <REPEATS> <FASTA_FILE>\n", argv[0]);
        return 1;
    }

    int repeats = atoi(argv[2]);
    char *fastaFileName = argv[3];

    FILE *fastaFile = fopen(fastaFileName, "r");
    if (fastaFile == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Initialize variables for timing
    clock_t start, end;
    double total_time = 0;

    for (int i = 0; i < repeats; i++) {
        start = clock();

        // Read FASTA data and store it in an array (arraydouble logic)
        char **fastaData = NULL;
        int numRecords = 0;
        int arraySize = 0;
        
        char line[1024];
        while (fgets(line, sizeof(line), fastaFile) != NULL) {
            // Check if the array needs to be resized
            if (numRecords == arraySize) {
                if (arraySize == 0) {
                    arraySize = 1;
                } else {
                    arraySize = arraySize * 2;
                }
                
                fastaData = (char **)realloc(fastaData, sizeof(char *) * arraySize);
                if (fastaData == NULL) {
                    perror("Error reallocating memory");
                    fclose(fastaFile);
                    return 1;
                }
            }
            
            // Allocate memory for the new record and copy the line
            fastaData[numRecords] = strdup(line);
            if (fastaData[numRecords] == NULL) {
                perror("Error allocating memory");
                fclose(fastaFile);
                return 1;
            }
            numRecords++;
        }

        // Free allocated memory
        for (int j = 0; j < numRecords; j++) {
            free(fastaData[j]);
        }
        free(fastaData);

        end = clock();
        total_time += (double)(end - start) / CLOCKS_PER_MS; // Calculate time in milliseconds
    }

    fclose(fastaFile);

    printf("%d FASTA records\n", repeats * numRecords);
    printf("%.3lf seconds taken for processing total\n", total_time / 1000); // Convert time to seconds
    printf("On average: %.3lf seconds per run\n", total_time / (repeats * 1000)); // Convert time to seconds

    return 0;
}
