from flask import Flask, request, jsonify, render_template_string
import replicate
import os
import requests

from PIL import Image
from io import BytesIO
load_dotenv()

app = Flask(__name__)
os.environ["REPLICATE_API_TOKEN"] = os.getenv("REPLICATE_API_TOKEN")

masks = {
    0: "AndraShirtMask.jpg",
    1: "AndraPantsMask.jpg",
    2: "AndraBackgroundMask.jpg", 
}

styles = {
    0: "casual office look",
    1: "formal business attire",
    2: "sporty outdoor look",
}

ESP32_IP = "http://10.48.162.220"  # Replace with the actual ESP32 IP address


@app.route("/generate-image", methods=["POST"])
def generate_image():
    try:
        # Get the JSON payload from the request body
        data = request.get_json()
        if not data:
            return jsonify({"error": "Invalid JSON or empty request body"}), 400

        # Ensure 'change_index', 'style_index', and 'last_style_index' are in the payload
        mask_number = data.get('change_index')
        style_index = data.get('style_index')

        if mask_number is None:
            return jsonify({"error": "'change_index' is required"}), 400

        if style_index is None:
            return jsonify({"error": "'style_index' is required"}), 400

        # Define the path to the results folder
        results_folder = "results"
        os.makedirs(results_folder, exist_ok=True)

        # Check if the results folder is empty
        image_files = [f for f in os.listdir(results_folder) if f.endswith(('.jpg', '.jpeg', '.png'))]
        if not image_files:
            # If the folder is empty, use the default "Andra2.jpg" image
            base_image_path = "Andra2.jpg"
        else:
            # If there are images in the folder, use the latest image (or any image you'd like)
            base_image_path = os.path.join(results_folder, image_files[0])

        mask_filename = masks[mask_number]
        prompt = styles.get(style_index, "formal business attire")  # Default prompt if invalid style_index

        app.logger.info(f"Using mask: {mask_filename}")
        app.logger.info(f"Using prompt: {prompt}")
        app.logger.info(f"Using base image: {base_image_path}")

        # Open the base image and the selected mask image
        with open(base_image_path, "rb") as image, open(mask_filename, "rb") as mask:
            input_data = {
                "mask": mask,
                "image": image,
                "prompt": prompt,  # Adjust as needed
                "num_inference_steps": 400,
                "guidance_scale": 9,
            }

            # Run the Replicate inpainting model
            output = replicate.run(
                "stability-ai/stable-diffusion-inpainting:95b7223104132402a9ae91cc677285bc5eb997834bd2349fa486f53910fd68b3",
                input=input_data
            )

            # Assuming output[0] is a URL, download the image
            image_url = output[0]
            image_data = requests.get(image_url).content

            # Save the generated image to the "results" folder
            full_res_path = os.path.join(results_folder, "full_res_image.jpg")
            with open(full_res_path, "wb") as f:
                f.write(image_data)

            # Resize the image to 240x270
            image_resized = Image.open(BytesIO(image_data)).resize((240, 270))

            resized_image_bytes = BytesIO()
            image_resized.save(resized_image_bytes, format="JPEG")
            resized_image_bytes.seek(0)

            # Send the resized image (as .jpg) to the ESP32
            send_image_to_esp32(resized_image_bytes)

            return '', 200

    except Exception as e:
        app.logger.error(f"Error in generate_image: {str(e)}")
        return jsonify({"error": str(e)}), 500
    
    
def send_image_to_esp32(image_data):
    try:
        # Send the image as binary data to the ESP32 using the /upload endpoint
        files = {'file': ('server.jpg', image_data)}  # Sending the image as a file
        response = requests.post(f"{ESP32_IP}/upload", files=files)
        
        if response.status_code == 200:
            print("Image sent to ESP32 successfully")
        else:
            print(f"Failed to send image to ESP32. Status code: {response.status_code}")
    except Exception as e:
        print(f"Error sending image to ESP32: {e}")

if __name__ == "__main__":
    app.run(debug=True, host='0.0.0.0', port=5000)