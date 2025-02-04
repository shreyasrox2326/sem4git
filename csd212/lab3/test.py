import torch
import numpy as np
import cv2 as cv
import matplotlib.pyplot as plt

# Check if GPU is available and set the device accordingly
device = torch.device("cuda")

# Read images and check if they are loaded correctly
img = cv.imread('./cat-in-low-dark-gray.jpg', cv.IMREAD_GRAYSCALE)
img_GT = cv.imread('./cat-in-dark-gray.jpg', cv.IMREAD_GRAYSCALE)

if img is None:
    raise FileNotFoundError("Image './cat-in-low-dark-gray.jpg' not found or unable to load.")
if img_GT is None:
    raise FileNotFoundError("Image './cat-in-dark-gray.jpg' not found or unable to load.")

# Convert the images to PyTorch tensors and move to GPU (if available)
img_tensor = torch.tensor(img, dtype=torch.uint8, device=device)
img_GT_tensor = torch.tensor(img_GT, dtype=torch.uint8, device=device)

# Initialize variables for optimization
xarr = []
yarr = []
zarr = []
min_rmse = float('inf')
min_r1 = min_s1 = min_r2 = min_s2 = 0
precision = 128

print("r1    s1    r2    s2    rmse")

# Function for RMSE calculation
def rmse(img1, img2):
    """
    Calculates the Root Mean Squared Error (RMSE) between two images.
    """
    diff = img1.float() - img2.float()  # Ensure the subtraction is in float
    return torch.sqrt(torch.mean(diff ** 2))

# Function for linear stretching
def lin_stretch(img, params, device):
    """
    Performs linear stretching of an image.
    Args:
        img: The input image (PyTorch tensor).
        params: List containing [r1, s1, r2, s2] for linear stretching.
        device: The device to move the tensors to (CPU or GPU).
    Returns:
        The stretched image (PyTorch tensor).
    """
    r1, s1, r2, s2 = params
    l = 255  # Assuming the maximum pixel value is 255

    # Create a lookup table (LUT) for linear stretching
    lut = torch.zeros(256, dtype=torch.uint8, device=device)

    # Create LUT on the specified device
    for r in range(256):
        if r < r1:
            s = r // 2
        elif r >= r2:
            s = l - (l - r) // 2
        else:
            # Linear interpolation between the two critical points
            s = s1 + (r - r1) * (s2 - s1) // (r2 - r1)
        lut[r] = min(max(s, 0), 255)  # Ensure the output is within 0-255 range

    # Apply the LUT to the image (indexing the LUT with the image tensor)
    stretched_img = lut[img.long()]  # Convert img to long for indexing

    return stretched_img

# Iterate through parameter combinations to find the best linear stretch
for i in range(precision):
    for j in range(precision):
        r1factor = i / precision
        s1factor = j / precision
        r1 = int(r1factor * 127)
        r2 = 255 - r1
        s1 = int(s1factor * 127)
        s2 = 255 - s1
        if r1 > r2 or s1 > s2:
            continue
        params = [r1, s1, r2, s2]

        # Apply the linear stretching (move to GPU)
        enhanced_img = lin_stretch(img_tensor, params, device)

        # Calculate RMSE (using GPU)
        rmse_val = rmse(enhanced_img, img_GT_tensor)

        # Collect results and track the minimum RMSE
        if s1==80: print(f"{str(r1).ljust(6)}{str(s1).ljust(6)}{str(r2).ljust(6)}{str(s2).ljust(6)}{str(rmse_val.item()).ljust(6)}")
        xarr.append(r1)
        yarr.append(s1)
        zarr.append(rmse_val.item())

        if rmse_val < min_rmse:
            min_rmse = rmse_val
            min_r1 = r1
            min_s1 = s1
            min_r2 = r2
            min_s2 = s2

print(f"min_rmse: {min_rmse}\nmin_r1: {min_r1}\nmin_s1 : {min_s1} \nmin_r2 : {min_r2} \nmin_s2 : {min_s2}")

# Apply the best parameters to get the enhanced image
params = [min_r1, min_s1, min_r2, min_s2]
enhanced_img = lin_stretch(img_tensor, params, device)

# Move enhanced image to CPU and convert to NumPy for visualization
stretched_img_cpu = enhanced_img.cpu().numpy()

# Display the enhanced image
plt.imshow(stretched_img_cpu, cmap='gray', vmin=0, vmax=255)
plt.axis('off')  # Hide axes
plt.show()
