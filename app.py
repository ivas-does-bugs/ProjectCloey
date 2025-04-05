import os
from flask import Flask, render_template, request, jsonify, send_from_directory
from PIL import Image

app = Flask(__name__)
app.config['UPLOAD_FOLDER'] = 'static/uploads'
app.config['PROCESSED_FOLDER'] = 'static/processed'

# Ensure the upload and processed folders exist
os.makedirs(app.config['UPLOAD_FOLDER'], exist_ok=True)
os.makedirs(app.config['PROCESSED_FOLDER'], exist_ok=True)

def remove_background_and_convert(input_path, output_path, bg_color=(255, 255, 255), threshold=30):
    """
    Opens a PNG or JPG image, converts it to RGBA, resizes it to a maximum of 240x320 pixels,
    removes pixels that are close to bg_color (making them transparent), and saves the result as a 32-bit BMP.
    """
    try:
        # Open the image and convert to RGBA for transparency.
        image = Image.open(input_path).convert("RGBA")
        
        # Resize the image to maximum dimensions 240x320 while preserving aspect ratio.
        image.thumbnail((240, 320), resample=Image.LANCZOS)
        
        pixels = image.getdata()
        new_pixels = []
        
        for pixel in pixels:
            # If the pixel is close to the background color, make it transparent.
            if (abs(pixel[0] - bg_color[0]) < threshold and
                abs(pixel[1] - bg_color[1]) < threshold and
                abs(pixel[2] - bg_color[2]) < threshold):
                new_pixels.append((pixel[0], pixel[1], pixel[2], 0))
            else:
                new_pixels.append(pixel)
        
        image.putdata(new_pixels)
        image.save(output_path, "BMP")
        return output_path
    except Exception as e:
        print("Error processing image:", e)
        raise e


@app.route('/')
def index():
    return render_template('index.html')

@app.route('/upload', methods=['POST'])
def upload():
    if 'file' not in request.files:
        return jsonify({'error': 'No file provided'}), 400
    file = request.files['file']
    if file.filename == '':
        return jsonify({'error': 'No file selected'}), 400

    # Save the original file
    original_path = os.path.join(app.config['UPLOAD_FOLDER'], file.filename)
    file.save(original_path)

    # Define the processed filename (converted to BMP)
    base, _ = os.path.splitext(file.filename)
    processed_filename = "processed_" + base + ".bmp"
    processed_path = os.path.join(app.config['PROCESSED_FOLDER'], processed_filename)

    try:
        remove_background_and_convert(original_path, processed_path)
    except Exception as e:
        return jsonify({'error': 'Processing failed', 'message': str(e)}), 500

    # Build URL for the processed image (Flask serves static files from /static)
    processed_url = f"/static/processed/{processed_filename}"
    return jsonify({'processed_url': processed_url})

@app.route('/<path:filename>')
def serve_static(filename):
    return send_from_directory('.', filename)

if __name__ == '__main__':
    # Host set to 0.0.0.0 to allow connections from any device on your network.
    app.run(host='0.0.0.0', port=8000, debug=True)
