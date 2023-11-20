import cv2
import numpy as np
from PIL import Image
import random
import os
import struct

NUMBER_OF_IMAGE_FOR_ONE_IMAGE = 10

def add_noise(img):
    ''' Adding random noise to the image '''
    row, col = img.shape
    number_of_pixels = random.randint(0, 15)
    for _ in range(number_of_pixels):
        y_coord=random.randint(0, row - 1)
        x_coord=random.randint(0, col - 1)
        img[y_coord][x_coord] = 255
    return img

def rotate_image(img, angle):
    ''' Rotating the image between -30 and 30 degrees '''
    center = tuple(np.array(img.shape[1::-1]) / 2)
    rot_mat = cv2.getRotationMatrix2D(center, angle, 1.0)
    result = cv2.warpAffine(img, rot_mat, img.shape[1::-1], flags=cv2.INTER_LINEAR)
    return result

def resize_image(img, output_size):
    ''' Resizing image to output_size '''
    img = Image.fromarray(img)
    img = img.resize(output_size, Image.LANCZOS)
    return np.array(img)

def process_image(img_path):
    ''' Applies resizing, rotation, and noise addition to the image '''
    # Read the image in grayscale
    img = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
    
    # Randomly zoom in or out
    zoom_factor = random.uniform(0.5, 1.5)  # Random zoom factor between 0.5x - 1.5x
    img = cv2.resize(img, None, fx=zoom_factor, fy=zoom_factor, interpolation=cv2.INTER_LINEAR)
    
    # Add noise
    img = add_noise(img)
    
    # Randomly rotate the image between -30 to 30 degrees
    angle = random.uniform(-10, 10)
    img = rotate_image(img, angle)
    
    # Final resize to 28x28
    final_img = resize_image(img, (28, 28))
    
    return final_img

def save_all_images_to_one_file(all_images, output_file):
    with open(output_file, 'wb') as f:
        #Write the number of all images
        size = 0
        for digit, images in all_images.items():
            size += len(images)
        print(f'Number of all images: {size}')
        f.write(size.to_bytes(4, byteorder='little', signed=False))
        for digit, images in all_images.items():
            # 1 byte for the digit
            for images in images:
                f.write(digit.to_bytes(1, byteorder='little', signed=False))
                # 28x28 bytes for each image
                for row in images:
                    for pixel in row:
                        f.write(pixel)
                # 4 bytes for the digit

def main(all_images_folder, output_file):
    all_images = {}  # Dictionary to store all images for each digit
    for digit in range(10):
        folder_name = os.path.join(all_images_folder, f'images_{digit}')
        image_files = [f for f in os.listdir(folder_name) if f.endswith(('.png', '.jpg', '.jpeg'))]

        all_images[digit] = []  # List to store images for the current digit
        for image_file in image_files:
            img_path = os.path.join(folder_name, image_file)
            for _ in range(NUMBER_OF_IMAGE_FOR_ONE_IMAGE):
                final_img = process_image(img_path)
                all_images[digit].append(final_img)

    # Save all images to a single file
    save_all_images_to_one_file(all_images, output_file)
    print(f'Saved all images to {output_file}')

# The folder that contains subfolders for each digit
all_images_folder = './datasets/dataset_creation'
# The output file where all images will be stored
output_file = './datasets/dataset_creation/processed/all_digits_images.bin'

main(all_images_folder, output_file)