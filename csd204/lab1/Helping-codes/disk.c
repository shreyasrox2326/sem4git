#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#define FNAME_SIZE 100
#define MAX_FILE_NO 5000
#define BLOCK_SIZE 1024

int main(int argc, char *argv[]) {
    int n, file_no;
    FILE *fp;
    char dest_file_name[FNAME_SIZE];
    char buf[BLOCK_SIZE];

    // Ensure the directory exists
    if (mkdir("disk-files", 0777) && errno != EEXIST) {
        perror("Failed to create directory");
        exit(1);
    }

    // Seed random number generator
    srand(time(NULL));

    while (1) {
        file_no = rand() % MAX_FILE_NO;

        memset(dest_file_name, 0, FNAME_SIZE);
        snprintf(dest_file_name, FNAME_SIZE, "disk-files/foo%d.pdf", file_no);

        fp = fopen(dest_file_name, "rb");
        if (fp == NULL) {
            fprintf(stderr, "Error opening file %s: %s\n", dest_file_name, strerror(errno));
            continue;  // Skip and try another file instead of exiting
        }

        memset(buf, 0, BLOCK_SIZE);
        while ((n = (int)fread(buf, 1, BLOCK_SIZE, fp)) > 0) {
            // Do nothing with the read data
            memset(buf, 0, BLOCK_SIZE);
        }

        fclose(fp);
    }

    return 0;
}

