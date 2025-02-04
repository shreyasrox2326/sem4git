import torch
import numpy as np
import cv2 as cv
import matplotlib.pyplot as plt
from tqdm import tqdm

# Enable memory-efficient settings
torch.backends.cudnn.benchmark = True
# Set this to help with memory fragmentation
torch.cuda.set_per_process_memory_fraction(1.0)  # Use only 80% of available memory

def stretch_batch(img, params_batch):
    """
    Memory-efficient batch processing function.
    """
    batch_size = len(params_batch)
    H, W = img.shape
    result = torch.empty((batch_size, H, W), dtype=torch.uint8, device='cuda')
    
    # Process each image in the batch
    for i, (r1, s1, r2, s2) in enumerate(params_batch):
        x_float = img.float()
        
        # Create masks
        mask1 = x_float < r1
        mask2 = x_float >= r2
        mask3 = ~(mask1 | mask2)
        
        # Initialize output
        output = torch.zeros_like(x_float)
        
        # Apply transformations
        output[mask1] = x_float[mask1] / 2
        output[mask2] = 255 - (255 - x_float[mask2]) / 2
        
        # Linear interpolation for middle range
        slope = (s2 - s1) / (r2 - r1)
        output[mask3] = s1 + (x_float[mask3] - r1) * slope
        
        result[i] = torch.clamp(output, 0, 255).to(torch.uint8)
        
        # Clear GPU cache periodically
        if i % 100 == 0:
            pass
            # torch.cuda.empty_cache()
    
    return result

def calculate_rmse(img1, img2):
    """
    Memory-efficient RMSE calculation
    """
    diff = img1.float() - img2.float()
    return torch.sqrt(torch.mean(diff ** 2))

def main():
    # Load images
    img = cv.imread('./cat-in-low-dark-gray.jpg', cv.IMREAD_GRAYSCALE)
    img_GT = cv.imread('./cat-in-dark-gray.jpg', cv.IMREAD_GRAYSCALE)
    
    if img is None or img_GT is None:
        raise FileNotFoundError("One or both images not found or unable to load.")
    
    # Convert to tensors and move to GPU
    img_tensor = torch.tensor(img, dtype=torch.uint8, device='cuda')
    img_GT_tensor = torch.tensor(img_GT, dtype=torch.uint8, device='cuda')
    
    # Initialize variables
    precision = 128
    batch_size = 64  # Reduced batch size for memory efficiency
    min_rmse = float('inf')
    best_params = None
    
    # Generate parameter combinations in smaller chunks
    total_combinations = 0
    best_params = None
    
    # Process in smaller batches with progress bar
    with tqdm(total=precision * precision) as pbar:
        for i in range(precision):
            for j in range(0, precision, batch_size):
                # Generate batch of parameters
                params_batch = []
                for k in range(j, min(j + batch_size, precision)):
                    r1 = int(i * 127 / precision)
                    r2 = 255 - r1
                    s1 = int(k * 127 / precision)
                    s2 = 255 - s1
                    if r1 < r2 and s1 < s2:
                        params_batch.append([r1, s1, r2, s2])
                
                if not params_batch:
                    continue
                
                # Process batch
                try:
                    stretched_batch = stretch_batch(img_tensor, params_batch)
                    
                    # Calculate RMSE for each result in batch
                    for idx, stretched_img in enumerate(stretched_batch):
                        rmse_val = calculate_rmse(stretched_img, img_GT_tensor)
                        
                        if rmse_val < min_rmse:
                            min_rmse = rmse_val.item()
                            best_params = params_batch[idx]
                    
                    # Update progress
                    pbar.update(len(params_batch))
                    
                except RuntimeError as e:
                    if "out of memory" in str(e):
                        print(f"WARNING: Out of memory at batch {i}. Clearing cache and reducing batch size.")
                        torch.cuda.empty_cache()
                        continue
                    else:
                        raise e
                
                # Clear cache after each batch
                torch.cuda.empty_cache()
    
    print(f"\nBest parameters found:")
    print(f"r1: {best_params[0]}, s1: {best_params[1]}")
    print(f"r2: {best_params[2]}, s2: {best_params[3]}")
    print(f"Minimum RMSE: {min_rmse}")
    
    # Generate final image using best parameters
    final_batch = stretch_batch(img_tensor, [best_params])
    enhanced_img = final_batch[0]
    
    # Display result
    plt.imshow(enhanced_img.cpu().numpy(), cmap='gray', vmin=0, vmax=255)
    plt.axis('off')
    plt.show()

if __name__ == "__main__":
    main()