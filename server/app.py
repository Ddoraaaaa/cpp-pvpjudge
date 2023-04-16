from flask import Flask, request, send_from_directory
from werkzeug.utils import secure_filename
import threading
from judge import Judge

app = Flask(__name__)
judges = [Judge(i) for i in range(10)]

@app.route('/submit', methods=['POST'])
def submit():
    if 'player1' not in request.files or 'player2' not in request.files:
        return 'Missing player file', 400

    p1File = request.files['player1']
    p2File = request.files['player2']

    if not (p1File.filename.endswith('.cpp') and p2File.filename.endswith('.cpp')):
        return 'Both files must be .cpp files', 400

    for judge in judges:
        if not judge.isOccupied:
            judge.markAsOccupied()
            judge.saveFiles(p1File, p2File)
            
            threading.Thread(target=judge.runAndMarkAsUnoccupied).start()
            return f'Judge {judge.judgeId} is processing the files.', 200

    return 'All judges are currently occupied. Please try again later.', 503

@app.route('/result/<int:judgeId>')
def result(judgeId):
    if 0 <= judgeId < len(judges) and not judges[judgeId].isOccupied:
        return send_from_directory(f'judge_folders/judge{judgeId}', 'log.txt')
    else:
        return 'Invalid judge ID or judge is still occupied.', 400

if __name__ == '__main__':
    app.run(debug=True)