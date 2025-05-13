#include <stdio.h>
#include <stdbool.h>

#define MAX 100

int main() {
    int n, frames, pages[MAX];
    int queue[MAX], front = 0, rear = 0, count = 0;
    int pageFaults = 0, pageHits = 0;
    bool found;

    printf("Enter the number of page references: ");
    scanf("%d", &n);

    printf("Enter the page requests: ");
    for (int i = 0; i < n; i++) {
        scanf("%d", &pages[i]);
    }

    printf("Enter the number of frames: ");
    scanf("%d", &frames);

    printf("\n");

    for (int i = 0; i < n; i++) {
        found = false;
        // Check if page already in frames
        for (int j = front; j < rear; j++) {
            if (queue[j % frames] == pages[i]) {
                found = true;
                break;
            }
        }

        printf("Page %d -> ", pages[i]);

        if (found) {
            pageHits++;
            // Print current frame content
            for (int j = front; j < rear; j++) {
                printf("%d ", queue[j % frames]);
            }
            printf("(Page Hit)\n");
        } else {
            pageFaults++;
            if (count < frames) {
                queue[rear % frames] = pages[i];
                rear++;
                count++;
            } else {
                queue[front % frames] = pages[i];
                front++;
                rear++;
            }

            // Print current frame content
            for (int j = front; j < rear; j++) {
                printf("%d ", queue[j % frames]);
            }
            printf("(Page Miss)\n");
        }
    }

    printf("\nTotal Page Miss: %d\n", pageFaults);
    printf("Total Page Hit: %d\n", pageHits);

    return 0;
}
