import os
import shutil
import subprocess
from werkzeug.utils import secure_filename
from collections import deque

class Hub:

    def __init__(self, judge_cnt, judge_dir):
        self.judges = []
        self.problems = [] 
        self.judgeCnt = judge_cnt
        self.judgeDir = judge_dir
        self.submission_queue = deque()
        self.player1_dir = os.path.join(judge_dir, 'player1')
        self.player2_dir = os.path.join(judge_dir, 'player2')
        sDir = os.path.join(judge_dir, 'scaffold')
        for i in range(judge_cnt):
            jDir = os.path.join(judge_dir, f'judge{i}')  
            shutil.copy(sDir, jDir)
            self.judges.append(Judge(jDir, self))

    def judgeComplete(self, judge):
        if self.submission_queue:
            self.runNextSubmission()

    def clearJudges(self, judge_cnt = None, judge_dir = None):
        if judge_cnt is None:
            judge_cnt = self.judgeCnt
            judge_dir = self.judgeDir
        for i in range(judge_cnt):
            jDir = os.path.join(judge_dir, f'judge{i}') 
            shutil.rmtree(jDir)

    def __del__(self):
        self.clearJudges()

    def addSubmission(self, player1_file, player2_file, submission_id):
        player1_file.save(os.path.join(self.player1_dir, f'{submission_id}.{player1_file.filename.split(".")[-1]}'))
        player2_file.save(os.path.join(self.player2_dir, f'{submission_id}.{player2_file.filename.split(".")[-1]}'))
        self.submission_queue.append(submission_id)

    def runNextSubmission(self):
        if self.submission_queue:
            submission_id = self.submission_queue.popleft()
            for judge in self.judges:
                if not judge.isOccupied:
                    judge.saveFiles(os.path.join(self.player1_dir, f'{submission_id}.py'),
                                    os.path.join(self.player2_dir, f'{submission_id}.py'))
                    judge.runAndMarkAsUnoccupied()
                    break
    

class Judge:
    def __init__(self, judge_dir, hub):
        self.hub = hub
        self.isOccupied = False
        self.folderPath = judge_dir

    def markAsOccupied(self):
        self.isOccupied = True

    def markAsUnoccupied(self):
        self.isOccupied = False
        self.hub.judgeComplete(self)

    def saveFiles(self, player1_file, player2_file):
        shutil.copy(player1_file, f'{self.folderPath}/player1.cpp')
        shutil.copy(player2_file, f'{self.folderPath}/player2.cpp')

    def runAndMarkAsUnoccupied(self):
        run_script = f'{self.folderPath}/run.sh'
        subprocess.run([run_script], cwd=self.folderPath)
        self.markAsUnoccupied()