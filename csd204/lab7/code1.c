#include <stdio.h>
#include <stdbool.h>

#define P 5 // number of processes
#define R 3 // number of resources

int main() {
    int i, j;

    int total[R], allocation[P][R], max[P][R];

    // Step 1: Input total available resources
    printf("Enter total available instances of resources X, Y, Z: ");
    for (j = 0; j < R; j++) {
        scanf("%d", &total[j]);
    }

    // Step 2: Input Allocation matrix
    printf("Enter Allocation matrix (5 processes × 3 resources):\n");
    for (i = 0; i < P; i++) {
        printf("P%d: ", i + 1);
        for (j = 0; j < R; j++) {
            scanf("%d", &allocation[i][j]);
        }
    }

    // Step 3: Input Max matrix
    printf("Enter Max matrix (5 processes × 3 resources):\n");
    for (i = 0; i < P; i++) {
        printf("P%d: ", i + 1);
        for (j = 0; j < R; j++) {
            scanf("%d", &max[i][j]);
        }
    }

    // Step 4: Calculate Available = Total - Allocated
    int available[R];
    for (j = 0; j < R; j++) {
        int sum = 0;
        for (i = 0; i < P; i++) {
            sum += allocation[i][j];
        }
        available[j] = total[j] - sum;
    }

    // Step 5: Calculate Need = Max - Allocation
    int need[P][R];
    for (i = 0; i < P; i++) {
        for (j = 0; j < R; j++) {
            need[i][j] = max[i][j] - allocation[i][j];
        }
    }

    // Step 6: Safety algorithm
    bool finish[P] = {false};
    int safeSequence[P], work[R];

    for (j = 0; j < R; j++) {
        work[j] = available[j];
    }

    int count = 0;
    while (count < P) {
        bool found = false;
        for (i = 0; i < P; i++) {
            if (!finish[i]) {
                bool canAllocate = true;
                for (j = 0; j < R; j++) {
                    if (need[i][j] > work[j]) {
                        canAllocate = false;
                        break;
                    }
                }

                if (canAllocate) {
                    for (j = 0; j < R; j++) {
                        work[j] += allocation[i][j];
                    }
                    safeSequence[count++] = i;
                    finish[i] = true;
                    found = true;
                }
            }
        }

        if (!found) {
            printf("\nSystem is not in a safe state.\n");
            return -1;
        }
    }

    // Step 7: Print Safe Sequence
    printf("\nThe safe sequence is: ");
    for (i = 0; i < P; i++) {
        printf("P%d", safeSequence[i] + 1);
        if (i != P - 1) printf(" -> ");
    }
    printf("\n");

    return 0;
}
