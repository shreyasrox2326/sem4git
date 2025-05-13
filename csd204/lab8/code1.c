#include <stdio.h>

#define MAX 100

int main() {
    int blockSize[MAX], processSize[MAX];
    int blockCount, processCount;
    int allocation[MAX];

    // Input number of memory blocks
    printf("Enter the number of memory blocks: ");
    scanf("%d", &blockCount);
    printf("Enter the sizes of %d memory blocks:\n", blockCount);
    for (int i = 0; i < blockCount; i++) {
        printf("Block %d: ", i + 1);
        scanf("%d", &blockSize[i]);
    }

    // Input number of processes
    printf("\nEnter the number of processes: ");
    scanf("%d", &processCount);
    printf("Enter the sizes of %d processes:\n", processCount);
    for (int i = 0; i < processCount; i++) {
        printf("Process %d: ", i + 1);
        scanf("%d", &processSize[i]);
        allocation[i] = -1; // Initially not allocated
    }

    // Best Fit Allocation
    for (int i = 0; i < processCount; i++) {
        int bestIdx = -1;
        for (int j = 0; j < blockCount; j++) {
            if (blockSize[j] >= processSize[i]) {
                if (bestIdx == -1 || blockSize[j] < blockSize[bestIdx]) {
                    bestIdx = j;
                }
            }
        }

        if (bestIdx != -1) {
            // Allocate block to process
            allocation[i] = bestIdx;
            blockSize[bestIdx] -= processSize[i]; // Reduce available size
        }
    }

    // Output results
    printf("\nProcess No.\tProcess Size\tBlock Allocated\n");
    for (int i = 0; i < processCount; i++) {
        printf("%d\t\t%d\t\t", i + 1, processSize[i]);
        if (allocation[i] != -1)
            printf("%d\n", allocation[i] + 1); // Block number (1-based)
        else
            printf("Not Allocated\n");
    }

    return 0;
}
