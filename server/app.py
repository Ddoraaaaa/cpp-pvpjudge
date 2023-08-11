from flask import Flask, request, send_from_directory, jsonify
from werkzeug.utils import secure_filename
import threading
from judge import Hub
import os
from dotenv import load_dotenv
import time
import random

load_dotenv()

app = Flask(__name__)
judge_cnt = int(os.getenv("JUDGE_COUNT"))
judge_dir = os.getenv("JUDGE_DIR")
p1_dir = os.getenv("P1_DIR")
p2_dir = os.getenv("P2_DIR")
log_dir = os.getenv("LOG_DIR")
socket_url = os.getenv("SOCKET_URL")

hub = Hub(judge_cnt, judge_dir, p1_dir, p2_dir, log_dir, socket_url)

@app.route('/submit', methods=['POST'])
def submit():
    if 'player1' not in request.files or 'player2' not in request.files:
        return 'Missing player file', 400

    p1File = request.files['player1']
    p2File = request.files['player2']

    if not (p1File.filename.endswith('.cpp') or p2File.filename.endswith('.cpp')):
        return 'Both files must be .cpp files', 400

    submission_id = str(random.randint(10000, 99999)) + time.strftime("%H%M%S")
    hub.addSubmission(p1File, p2File, submission_id)
    return {'message': 'Submission received.', 'submission_id': submission_id}, 200

@app.route('/result/<submissionId>', methods=['POST'])
def result(submissionId):
    log_file_path = os.path.join(hub.logDir, f'{submissionId}.txt')
    
    if os.path.exists(log_file_path):
        return jsonify({'message': True}), send_from_directory(hub.logDir, f'{submissionId}.txt')
    else:
        return jsonify({'message': False, 'file': ''}), 400

if __name__ == '__main__':
    app.run(debug=True)
    