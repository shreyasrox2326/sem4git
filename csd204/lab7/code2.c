#include <stdio.h>
#include <math.h>

int log2int(int x) {
    int res = 0;
    while (x > 1) {
        x >>= 1;
        res++;
    }
    return res;
}

int main() {
    int process_size, logical_address_bits;
    int main_memory_size, page_size;

    printf("Enter Process Size (in bytes): ");
    scanf("%d", &process_size);

    printf("Enter Logical Address Bits: ");
    scanf("%d", &logical_address_bits);

    printf("Enter Main Memory Size (in bytes): ");
    scanf("%d", &main_memory_size);

    printf("Enter Page Size (in bytes): ");
    scanf("%d", &page_size);

    // Calculations
    int page_offset_bits = log2int(page_size);
    int page_number_bits = logical_address_bits - page_offset_bits;

    int total_pages = process_size / page_size;
    int total_frames = main_memory_size / page_size;

    int frame_number_bits = log2int(total_frames);
    int frame_offset_bits = log2int(page_size);
    int physical_address_bits = frame_number_bits + frame_offset_bits;

    // Output
    printf("\n--- Address Translation Details ---\n");
    printf("Logical Address Bits = %d\n", logical_address_bits);
    printf("Page Number Bits = %d\n", page_number_bits);
    printf("Page Offset Bits = %d\n", page_offset_bits);
    printf("Physical Address Bits = %d\n", physical_address_bits);
    printf("Frame Number Bits = %d\n", frame_number_bits);
    printf("Frame Offset Bits = %d\n", frame_offset_bits);
    printf("Total number of pages in Logical Address Space = %d\n", 1 << page_number_bits);
    printf("Total number of frames in Main Memory = %d\n", total_frames);

    return 0;
}
