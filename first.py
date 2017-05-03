import os
import time
import subprocess
import time


def extractFeatures():
	print "Hello first.py"

	#print subprocess.call('cd C:\Users\Popoye\Downloads\openSMILE-2.1.0\openSMILE-2.1.0',shell=True)
	#print subprocess.call('SMILExtract_Release -C config/emobase.conf -I rec.wav -O output.arff -classes {happy,sad,neutral} -classlabel neutral',shell=True)
	path = 'C:\Users\Bhawana\Desktop\Research_code\openSMILE-2.1.0\\dataset-audios'
	path2= 'C:\Users\Bhawana\Desktop\Research_code\openSMILE-2.1.0\\traindata'

	for filename in os.listdir(path2):
	#         fn= filename.partition('.')[0]
	#         print subprocess.Popen('sox '+path+'\\'+filename+' '+path2+'\\'+fn.strip('0123456789')+'.wav')

			var = 'SMILExtract_Release -C config/emobase.conf -I '+'traindata\\'+filename +' -O output.arff -classes {h,s,n} -classlabel '+ 'h'
			print subprocess.Popen(var,cwd='C:\Users\Bhawana\Desktop\Research_code\openSMILE-2.1.0')
			time.sleep(3)
			
#extractFeatures()