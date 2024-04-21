import sys
from PIL import Image

def extract_blue_channel(image_path, output_file):
    # Open the image
    img = Image.open(image_path)

    # Convert to RGB mode if not already in RGB mode
    if img.mode != 'RGB':
        img = img.convert('RGB')

    # Get the pixel data
    pixels = img.load()

    # Open the output file for writing
    with open(output_file, 'w') as f:
        # Iterate through each pixel
        for y in range(img.height):
            for x in range(img.width):
                # Get the RGB values of the pixel
                r, g, b = pixels[x, y]

                # Write the blue (B) value to the output file
                f.write(str(b))

                # Add a comma if it's not the last value in the row
                if x < img.width - 1:
                    f.write(',')

            # Add a newline character after each row
            f.write('\n')

if __name__ == "__main__":
    # Check if correct number of arguments is provided
    if len(sys.argv) != 3:
        print("Usage: python extract_blue_channel.py <input_image_path> <output_text_file>")
        sys.exit(1)

    # Get the input image path and output text file path from command line arguments
    input_image_path = sys.argv[1]
    output_text_file = sys.argv[2]

    # Extract blue channel values and write to the output text file
    extract_blue_channel(input_image_path, output_text_file)
