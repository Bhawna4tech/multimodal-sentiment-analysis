import arff
from ffmpy import FFmpeg
import os

#To read data from arff file
#data = arff.load(open('C:\Users\Popoye\Downloads\openSMILE-2.1.0\openSMILE-2.1.0\output.arff', 'rb'))
#dataset=data['data']
#for row in dataset:
#    print row[17],row[22],row[30],row[53],row[66],row[92],row[104],row[142],row[159],row[180],row[199],row[216],row[237]


#extracting audio from an video file
def videoToAudio(filename):
	print "Hello arffread.py"
	#path='C:\Users\Bhawana\Desktop\Research_code\openSMILE-2.1.0\\video-audios'
	#path = 'openSMILE-2.1.0\\video-audios'
	pathInput = 'video/'
	pathOutput = 'openSMILE-2.1.0/dataset-audios/'
	#for filename in os.listdir(pathInput):
	filename = filename.partition(".")[0]
	ff = FFmpeg(
	inputs={pathInput + filename + '.mp4': None},
	outputs={pathOutput + filename+ '.wav': None}
	)
	print str(pathInput) + str(filename) + '.avi'
	print str(pathOutput) + str(filename) + '.wav'
	#ff.cmd
	#'ffmpeg -i input.ts output.mp4'
	print "................."
	print filename
	print "................."
	ff.run()
		
#videoToAudio()