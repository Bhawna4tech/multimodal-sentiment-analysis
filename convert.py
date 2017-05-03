import os
import time
import subprocess

def convertSox():
	print "Hello convert"
	path = 'openSMILE-2.1.0\\dataset-audios'
	path2= 'openSMILE-2.1.0\\traindata'


	for filename in os.listdir(path):
			 fn= filename.partition('.')[0]
			 print subprocess.Popen('sox '+path+'\\'+filename+' '+path2+'\\'+fn+'.wav')		 

#convertSox()