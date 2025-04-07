from flask import Flask, render_template, redirect, url_for, send_from_directory
import os
import subprocess

app = Flask(__name__)

MODULES_DIR = './modules'
OUTPUT_DIR = './web/manuals'
WEB_DIR = './web'  # Serve this directory as static files

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/generate/<code>')
def generate(code):
    modules = [MODULES_DIR + "/" + name for name in os.listdir(MODULES_DIR) if os.path.isdir(os.path.join(MODULES_DIR, name))]
    cmd = ['.pio/build/manual/program', code] + modules
    subprocess.run(cmd, check=True)
    return redirect("/manuals/" + code + ".html")

# Serve static files from ./web
@app.route('/<path:filename>')
def serve_web(filename):
    file_path = os.path.join(WEB_DIR, filename)
    print(f"Requested: {filename}, Full Path: {file_path}")
    return send_from_directory(WEB_DIR, filename)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port='5000', debug=True)

