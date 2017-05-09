import arff
from ffmpy import FFmpeg
import os
import csv

def featuresToCsv():
	print "Hello attributes.py"
	#To read data from arff file
	arffData = arff.load(open('openSMILE-2.1.0\output.arff', 'rb'))
	#print (arffData)
	dataset = arffData['data']

	writer = csv.writer(open('openSMILE-2.1.0\\attributes.csv', 'w'))
	
	for col in dataset:
		result = [col[18],col[23],col[31],col[54],col[67],col[93],col[105],col[143],col[160],col[181],col[200],col[217],col[238]]
		#result = [col[17],col[22],col[30],col[53],col[66],col[92],col[104],col[142],col[159],col[180],col[199],col[216],col[237],col[989]]
		writer.writerow(result)