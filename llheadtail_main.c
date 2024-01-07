#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "fasta.h"
#include "LLvNode.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)
#define RECORD_THRESHOLD 10000

void deleteRecord(LLvNode *node, void *userdata) {
    fastaDeallocateRecord((FASTArecord *)node->value);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s -R <REPEATS> <FASTA_FILE>\n", argv[0]);
        return 1;
    }

    int repeats = atoi(argv[2]);
    char *fastaFileName = argv[3];

    // timing
    clock_t start, end;
    double total_time = 0;

    for (int i = 0; i < repeats; i++) {
        start = clock();

        FILE *fastaFile = fopen(fastaFileName, "r");
        if (fastaFile == NULL) {
            perror("Error opening file");
            return 1;
        }

        // fasta read and store
        LLvNode *head = NULL;
        LLvNode *tail = NULL;
        int numRecords = 0;

        char line[1024];
        while (fgets(line, sizeof(line), fastaFile) != NULL) {
            if (strncmp(line, ">sp", 3) == 0) {
          
                FASTArecord *fRecord = fastaAllocateRecord();
                if (fRecord == NULL) {
                    perror("Error allocating memory");
                    fclose(fastaFile);
                    return 1;
                }
                fRecord->sequence = strdup(line);

                // create a new node with the record
                LLvNode *newNode = llNewNode(NULL, fRecord);

                
                if (head == NULL) {
                    head = newNode;
                    tail = newNode;
                } else {
                    tail->next = newNode;
                    tail = newNode;
                }

                numRecords++;

                // print progress every 10,000 records
                if (numRecords % RECORD_THRESHOLD == 0) {
                    printf(".");
                    fflush(stdout);
                }
            }
        }

        // other record preogress
        printf(" %d FASTA records\n", numRecords);

        // Free memory
        llFree(head, deleteRecord, NULL);

        end = clock();
        total_time += (double)(end - start) / CLOCKS_PER_MS; // output time
        fclose(fastaFile);
    }

    printf("\n%.3lf seconds taken for processing total\n", total_time / 1000); 
    printf("On average: %.3lf seconds per run\n", total_time / (repeats * 1000)); 

    return 0;
}
