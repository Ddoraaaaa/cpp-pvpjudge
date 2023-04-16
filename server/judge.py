import os
import subprocess
from werkzeug.utils import secure_filename

class Judge:
    def __init__(self, judge_id):
        self.judgeId = judge_id
        self.isOccupied = False
        self.folderPath = f'./judge_folders/judge{judge_id}'

    def markAsOccupied(self):
        self.isOccupied = True

    def markAsUnoccupied(self):
        self.isOccupied = False

    def saveFiles(self, player1_file, player2_file):
        secure_filename(player1_file.filename)
        secure_filename(player2_file.filename)
        player1_file.save(f'{self.folderPath}/player1.cpp')
        player2_file.save(f'{self.folderPath}/player2.cpp')

    def runAndMarkAsUnoccupied(self):
        run_script = f'{self.folderPath}/run.sh'
        subprocess.run([run_script], cwd=self.folderPath)
        self.markAsUnoccupied()