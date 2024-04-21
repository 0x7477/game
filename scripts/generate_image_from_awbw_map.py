import sys
import numpy as np
from PIL import Image

def create_image_from_string(input_string, output_file):
    # Split the string into rows
    rows = input_string.strip().split('\n')

    # Split each row into numbers
    data = [[int(num) for num in row.split(',')] for row in rows]

    # Create an array filled with zeros for red and green channels, and assign numbers to the blue channel
    blue_channel = np.array(data, dtype=np.uint8)[:, :, None]  # Expand dimensions to match the RGB format

    # Create the image array by stacking the blue channel with zeros for red and green channels
    image = np.concatenate((np.zeros_like(blue_channel), np.zeros_like(blue_channel), blue_channel), axis=-1)

    # Create a PIL image from the numpy array
    pil_image = Image.fromarray(image)

    # Save the image to the specified output file
    pil_image.save(output_file)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python script.py <input_filename> <output_filename>")
        sys.exit(1)
    
    input_filename = sys.argv[1]
    output_filename = sys.argv[2]

    try:
        with open(input_filename, 'r') as file:
            input_string = file.read()
            create_image_from_string(input_string, output_filename)
    except FileNotFoundError:
        print(f"Error: File '{input_filename}' not found.")
        sys.exit(1)
