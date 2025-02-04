#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cuda_runtime.h>

// Error checking macro
#define CHECK_CUDA_ERROR(call) \
    do { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            fprintf(stderr, "CUDA error at %s:%d: %s\n", __FILE__, __LINE__, \
                    cudaGetErrorString(err)); \
            exit(EXIT_FAILURE); \
        } \
    } while(0)

// Structure to hold image data
typedef struct {
    unsigned char* data;
    int width;
    int height;
} Image;

// Parameters for linear stretching
typedef struct {
    int r1;
    int s1;
    int r2;
    int s2;
} StretchParams;

// CUDA kernel for linear stretching
__global__ void linearStretchKernel(
    unsigned char* input,
    unsigned char* output,
    int width,
    int height,
    int numParams,
    StretchParams* params
) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int paramIdx = blockIdx.y;
    
    if (idx < width * height && paramIdx < numParams) {
        int pixel = input[idx];
        StretchParams p = params[paramIdx];
        float result;
        
        // Apply linear stretching based on parameters
        if (pixel < p.r1) {
            result = pixel / 2.0f;
        } else if (pixel >= p.r2) {
            result = 255.0f - (255.0f - pixel) / 2.0f;
        } else {
            float slope = (float)(p.s2 - p.s1) / (p.r2 - p.r1);
            result = p.s1 + (pixel - p.r1) * slope;
        }
        
        // Clamp result to valid range
        result = fminf(fmaxf(result, 0.0f), 255.0f);
        output[paramIdx * width * height + idx] = (unsigned char)result;
    }
}

// CUDA kernel for RMSE calculation
__global__ void calculateRMSEKernel(
    unsigned char* stretched,
    unsigned char* target,
    float* rmse,
    int width,
    int height,
    int numParams
) {
    int paramIdx = blockIdx.x * blockDim.x + threadIdx.x;
    
    if (paramIdx < numParams) {
        float sum = 0.0f;
        unsigned char* stretchedImage = stretched + paramIdx * width * height;
        
        for (int i = 0; i < width * height; i++) {
            float diff = (float)stretchedImage[i] - (float)target[i];
            sum += diff * diff;
        }
        
        rmse[paramIdx] = sqrtf(sum / (width * height));
    }
}

// Function to load an image (placeholder - implement based on your image format)
Image loadImage(const char* filename) {
    Image img;
    // Implement image loading based on your needs
    // For example, using stb_image.h or OpenCV
    return img;
}

// Function to save an image (placeholder - implement based on your image format)
void saveImage(const char* filename, const Image* img) {
    // Implement image saving based on your needs
}

// Main processing function
void processImage(const Image* input, const Image* target, Image* output) {
    // CUDA variables
    unsigned char *d_input, *d_target, *d_output;
    float *d_rmse, *h_rmse;
    StretchParams *d_params, *h_params;
    
    // Constants
    const int precision = 128;
    const int maxParams = precision * precision;
    const int blockSize = 256;
    
    // Allocate device memory
    CHECK_CUDA_ERROR(cudaMalloc(&d_input, input->width * input->height));
    CHECK_CUDA_ERROR(cudaMalloc(&d_target, target->width * target->height));
    CHECK_CUDA_ERROR(cudaMalloc(&d_output, input->width * input->height * maxParams));
    CHECK_CUDA_ERROR(cudaMalloc(&d_rmse, maxParams * sizeof(float)));
    CHECK_CUDA_ERROR(cudaMalloc(&d_params, maxParams * sizeof(StretchParams)));
    
    // Allocate host memory
    h_rmse = (float*)malloc(maxParams * sizeof(float));
    h_params = (StretchParams*)malloc(maxParams * sizeof(StretchParams));
    
    // Copy input data to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_input, input->data, 
        input->width * input->height, cudaMemcpyHostToDevice));
    CHECK_CUDA_ERROR(cudaMemcpy(d_target, target->data, 
        target->width * target->height, cudaMemcpyHostToDevice));
    
    // Generate parameters
    int paramCount = 0;
    for (int i = 0; i < precision; i++) {
        for (int j = 0; j < precision; j++) {
            int r1 = i * 127 / precision;
            int r2 = 255 - r1;
            int s1 = j * 127 / precision;
            int s2 = 255 - s1;
            
            if (r1 < r2 && s1 < s2) {
                h_params[paramCount].r1 = r1;
                h_params[paramCount].s1 = s1;
                h_params[paramCount].r2 = r2;
                h_params[paramCount].s2 = s2;
                paramCount++;
            }
        }
    }
    
    // Copy parameters to device
    CHECK_CUDA_ERROR(cudaMemcpy(d_params, h_params, 
        paramCount * sizeof(StretchParams), cudaMemcpyHostToDevice));
    
    // Calculate grid dimensions for stretching
    dim3 stretchBlock(blockSize);
    dim3 stretchGrid((input->width * input->height + blockSize - 1) / blockSize, paramCount);
    
    // Launch stretching kernel
    linearStretchKernel<<<stretchGrid, stretchBlock>>>(
        d_input, d_output, input->width, input->height, paramCount, d_params);
    CHECK_CUDA_ERROR(cudaGetLastError());
    
    // Calculate grid dimensions for RMSE
    int rmseBlocks = (paramCount + blockSize - 1) / blockSize;
    
    // Launch RMSE kernel
    calculateRMSEKernel<<<rmseBlocks, blockSize>>>(
        d_output, d_target, d_rmse, input->width, input->height, paramCount);
    CHECK_CUDA_ERROR(cudaGetLastError());
    
    // Copy RMSE results back to host
    CHECK_CUDA_ERROR(cudaMemcpy(h_rmse, d_rmse, 
        paramCount * sizeof(float), cudaMemcpyDeviceToHost));
    
    // Find best parameters
    float minRMSE = INFINITY;
    int bestParamIdx = 0;
    
    for (int i = 0; i < paramCount; i++) {
        if (h_rmse[i] < minRMSE) {
            minRMSE = h_rmse[i];
            bestParamIdx = i;
        }
    }
    
    // Apply best parameters to create final output
    dim3 finalGrid((input->width * input->height + blockSize - 1) / blockSize, 1);
    linearStretchKernel<<<finalGrid, blockSize>>>(
        d_input, d_output, input->width, input->height, 1, &d_params[bestParamIdx]);
    
    // Copy final result back to host
    CHECK_CUDA_ERROR(cudaMemcpy(output->data, d_output, 
        input->width * input->height, cudaMemcpyDeviceToHost));
    
    // Cleanup
    cudaFree(d_input);
    cudaFree(d_target);
    cudaFree(d_output);
    cudaFree(d_rmse);
    cudaFree(d_params);
    free(h_rmse);
    free(h_params);
    
    printf("Best parameters found:\n");
    printf("r1: %d, s1: %d\n", h_params[bestParamIdx].r1, h_params[bestParamIdx].s1);
    printf("r2: %d, s2: %d\n", h_params[bestParamIdx].r2, h_params[bestParamIdx].s2);
    printf("Minimum RMSE: %f\n", minRMSE);
}

int main(int argc, char** argv) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <input_image> <target_image> <output_image>\n", argv[0]);
        return 1;
    }
    
    // Load input images
    Image input = loadImage(argv[1]);
    Image target = loadImage(argv[2]);
    
    // Allocate output image
    Image output;
    output.width = input.width;
    output.height = input.height;
    output.data = (unsigned char*)malloc(output.width * output.height);
    
    // Process image
    processImage(&input, &target, &output);
    
    // Save result
    saveImage(argv[3], &output);
    
    // Cleanup
    free(input.data);
    free(target.data);
    free(output.data);
    
    return 0;
}