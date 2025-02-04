import torch
import numpy as np
import cv2 as cv
import matplotlib.pyplot as plt
from tqdm import tqdm  # For progress tracking

# For this specific task, CPU might be more efficient
device = torch.device("cuda")  # Changed to CPU

def create_lut(r1, s1, r2, s2):
    """
    Creates a lookup table for linear stretching.
    Vectorized version that creates the entire LUT at once.
    """
    lut = torch.arange(256, dtype=torch.uint8)
    
    # Vectorized conditions
    mask1 = lut < r1
    mask2 = lut >= r2
    mask3 = ~(mask1 | mask2)
    
    # Apply transformations
    lut[mask1] = lut[mask1] // 2
    lut[mask2] = 255 - (255 - lut[mask2]) // 2
    
    # Linear interpolation for middle range
    slope = (s2 - s1) / (r2 - r1)
    lut[mask3] = torch.clip(s1 + ((lut[mask3] - r1) * slope).to(torch.uint8), 0, 255)
    
    return lut

def apply_lut(img, lut):
    """
    Apply lookup table to image
    """
    return lut[img.long()]

def batch_lin_stretch(img, params_batch):
    """
    Applies linear stretching to an image with multiple parameter sets at once.
    """
    batch_size = len(params_batch)
    H, W = img.shape
    stretched_images = torch.empty((batch_size, H, W), dtype=torch.uint8)
    
    for i, params in enumerate(params_batch):
        lut = create_lut(*params)
        stretched_images[i] = apply_lut(img, lut)
    
    return stretched_images

def batch_rmse(img1_batch, img2):
    """
    Calculates RMSE for a batch of images against a single target image.
    Handles broadcasting automatically.
    """
    diff = img1_batch.float() - img2.float().unsqueeze(0)  # Add batch dimension to img2
    return torch.sqrt(torch.mean(diff ** 2, dim=(1, 2)))

def main():
    # Load images
    img = cv.imread('./cat-in-low-dark-gray.jpg', cv.IMREAD_GRAYSCALE)
    img_GT = cv.imread('./cat-in-dark-gray.jpg', cv.IMREAD_GRAYSCALE)
    
    if img is None or img_GT is None:
        raise FileNotFoundError("One or both images not found or unable to load.")
    
    # Convert to tensors
    img_tensor = torch.tensor(img, dtype=torch.uint8)
    img_GT_tensor = torch.tensor(img_GT, dtype=torch.uint8)
    
    # Initialize variables
    precision = 128
    batch_size = 128  # Process this many parameter combinations at once
    min_rmse = float('inf')
    best_params = None
    
    # Generate all valid parameter combinations
    params_list = []
    for i in range(precision):
        for j in range(precision):
            r1 = int(i * 127 / precision)
            r2 = 255 - r1
            s1 = int(j * 127 / precision)
            s2 = 255 - s1
            if r1 < r2 and s1 < s2:
                params_list.append([r1, s1, r2, s2])
    
    # Process in batches
    total_batches = len(params_list) // batch_size + (1 if len(params_list) % batch_size else 0)
    
    for batch_idx in tqdm(range(total_batches)):
        start_idx = batch_idx * batch_size
        end_idx = min((batch_idx + 1) * batch_size, len(params_list))
        params_batch = params_list[start_idx:end_idx]
        
        # Process batch
        stretched_batch = batch_lin_stretch(img_tensor, params_batch)
        rmse_batch = batch_rmse(stretched_batch, img_GT_tensor)
        
        # Find best parameters in batch
        min_idx = torch.argmin(rmse_batch)
        if rmse_batch[min_idx] < min_rmse:
            min_rmse = rmse_batch[min_idx].item()
            best_params = params_batch[min_idx]
    
    print(f"Best parameters found:")
    print(f"r1: {best_params[0]}, s1: {best_params[1]}")
    print(f"r2: {best_params[2]}, s2: {best_params[3]}")
    print(f"Minimum RMSE: {min_rmse}")
    
    # Generate final image
    enhanced_img = batch_lin_stretch(img_tensor, [best_params])[0]
    
    # Display result
    plt.imshow(enhanced_img.numpy(), cmap='gray', vmin=0, vmax=255)
    plt.axis('off')
    plt.show()

if __name__ == "__main__":
    main()