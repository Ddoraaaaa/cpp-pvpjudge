import os
import glob
import shutil
import subprocess
from werkzeug.utils import secure_filename
from collections import deque
from utils import compileFile

class Hub:

    def __init__(self, judge_cnt, judge_dir, p1_dir, p2_dir, log_dir):
        self.judges = []
        self.problems = [] 
        self.judgeCnt = judge_cnt
        self.judgeDir = judge_dir
        self.p1Dir = p1_dir
        self.p2Dir = p2_dir
        self.logDir = log_dir
        self.submission_queue = deque()
        sDir = os.path.join(judge_dir, 'scaffold')
        for i in range(judge_cnt):
            jDir = os.path.join(judge_dir, f'judge{i}')  
            shutil.copy(sDir, jDir)
            self.judges.append(Judge(jDir, self.logDir, self))

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
        player1_file.save(os.path.join(self.p1Dir, f'{submission_id}.{player1_file.filename.split(".")[-1]}'))
        player2_file.save(os.path.join(self.p2Dir, f'{submission_id}.{player2_file.filename.split(".")[-1]}'))
        self.submission_queue.append(submission_id)
        self.runNextSubmission()

    def runNextSubmission(self):
        for judge in self.judges:
            if not judge.isOccupied:
                if self.submission_queue:
                    submission_id = self.submission_queue.popleft()
                    player1_file = glob.glob(os.path.join(self.player1_dir, f'{submission_id}.*'))[0]
                    player2_file = glob.glob(os.path.join(self.player2_dir, f'{submission_id}.*'))[0]
                    judge.saveFiles(player1_file, player2_file)
                    judge.runAndMarkAsUnoccupied()
                    break 
    
class Judge:
    def __init__(self, judge_dir, log_dir, hub):
        self.hub = hub
        self.logDir = log_dir
        self.isOccupied = False
        self.folderPath = judge_dir

    def markAsOccupied(self):
        self.isOccupied = True

    def markAsUnoccupied(self):
        self.isOccupied = False
        self.hub.judgeComplete(self)

    def saveFiles(self, player1_filepath, player2_filepath, submission_id):
        compileFile(player1_filepath, f'{self.folderPath}/p1root/player1')
        compileFile(player2_filepath, f'{self.folderPath}/p2root/player2')
        self.subId = submission_id

    def runAndMarkAsUnoccupied(self):
        compileFile(f'{self.folderPath}/gameMaster.cpp', f'{self.folderPath}/gameMaster')
        compileFile(f'{self.folderPath}/judge.cpp', f'{self.folderPath}/judge')

        subprocess.run([f'{self.folderPath}/gameMaster'], cwd=self.folderPath)
        shutil.copy(f'{self.folderPath}/log.txt', f'{self.logDir}/{self.subId}.txt')
        self.markAsUnoccupied()