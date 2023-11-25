import asyncio
import websockets
import binascii
from io import BytesIO
from PIL import Image
from flask import Flask, Response, render_template, make_response
from base64 import b64encode
import cv2
import numpy as np
from sklearn.cluster import KMeans


app = Flask(__name__, template_folder='templates')

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/video_feed')
def index_video():
    return Response(get_image(), mimetype='multipart/x-mixed-replace; boundary=frame')

@app.route('/color')
def index_color():
    # Load the image
    image = cv2.imread('image.jpg')
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # Reshape the image to be a list of pixels
    pixels = image.reshape(-1, 3)

    # Use KMeans to find dominant colors
    kmeans = KMeans(n_clusters=1)
    kmeans.fit(pixels)

    # Get the dominant color(s)
    dominant_color = kmeans.cluster_centers_.astype(int)
    
    response = make_response(str(dominant_color[0][0]) + ',' + str(dominant_color[0][1]) + ',' + str(dominant_color[0][2]))
    return response


def get_image():
    while True:
        try:
            with open("image.jpg", "rb") as f:
                image_bytes = f.read()
            image = Image.open(BytesIO(image_bytes))
            img_io = BytesIO()
            image.save(img_io, 'JPEG')
            img_io.seek(0)
            img_bytes = img_io.read()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + img_bytes + b'\r\n')

        except Exception as e:
            print("encountered an exception: ")
            print(e)

            with open("placeholder.jpg", "rb") as f:
                image_bytes = f.read()
            image = Image.open(BytesIO(image_bytes))
            img_io = BytesIO()
            image.save(img_io, 'JPEG')
            img_io.seek(0)
            img_bytes = img_io.read()
            yield (b'--frame\r\n'
                   b'Content-Type: image/jpeg\r\n\r\n' + img_bytes + b'\r\n')
            continue


app.run(host='0.0.0.0', port=5000, debug=False, threaded=True)