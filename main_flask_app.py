#!/usr/bin/python
import os
import sys
from flask import Flask, render_template, request,send_from_directory, redirect, url_for
from werkzeug import secure_filename
from facepp import API, File
from pprint import pformat
import time
import math
import csv
import operator
import cv2
from math import e, sqrt, pi

app = Flask(__name__)

app.config['UPLOAD_FOLDER'] = 'uploads'
app.config['ALLOWED_EXTENSIONS'] = set(['mp4','MP4'])
app.config['FRAMES'] = 'frames'


API_KEY = '0ec41f73620f6ec21ad5c986208aa328'
API_SECRET = '3eCGX3rj5Rj3xAejIHGDjJZxCuZIa7Rx'

def print_result(result):
	def encode(obj):
		if type(obj) is unicode:
			return obj.encode('utf-8')
		if type(obj) is dict:
			return {encode(k): encode(v) for (k, v) in obj.iteritems()}
		if type(obj) is list:
			return [encode(i) for i in obj]
		return obj

	result = encode(result)
	return '\n'.join(['  ' + i for i in pformat(result, width = 75).split('\n')])


def allowed_file(filename):
    return '.' in filename and filename.rsplit('.', 1)[1] in app.config['ALLOWED_EXTENSIONS']

@app.route('/', methods=['GET', 'POST'])
#@app.route('/')
def index():
    return render_template('index.html')

		
@app.route('/upload',methods=['POST'])		
def uploading():
	file = request.files['file']
	if file.filename == '':
		flash('No selected file')
		return redirect(request.url)
	if file and allowed_file(file.filename):
		filename = secure_filename(file.filename)
		file.save(os.path.join("video", filename))
		#return send_from_directory(app.config['UPLOAD_FOLDER'],filename)
		#filename is the real file name like "bhawna.jpg"
		return redirect(url_for('processing',name=filename))

@app.route('/upload/processing/<name>')		
def processing(name):
	import execute
	print " calling audio features"
	execute.audioFeatures(name)
	print " called audio features"
	api = API(API_KEY, API_SECRET)
	#url = "http://blogs.reuters.com/great-debate/files/2013/07/obama-best.jpg"
	
	#result = api.detection.detect(img = File(os.path.join('uploads/',str(name)))) #to detect   local images
	#result = api.detection.detect(url=url)  to detect online images
	#return attributeExtractionToCsv(result)
	
	print "going to take video"
	vidcap = cv2.VideoCapture(os.path.join("video", name))
	print "video captured"
	count = 0

	while count<6:
		print "image taking"
		success,image = vidcap.read()
		print 'Read a new frame: ', success
		cv2.imwrite(os.path.join("Frames","frame%d.jpg" % count), image)     # save frame as JPEG file
		count += 1
		print "image taken"
		vidcap.set(cv2.CAP_PROP_POS_MSEC,count*600)
		
	fields = ['File Name' , 'Face Id' , 'Distance b/w left eye and nose' , 'Distance b/w right eye and nose' , 'Distance b/w nose and left side of mouth' , 'Distance b/w nose and right side of mouth']
	initializeTestCSV(fields)
	count = 0 
	for filename in os.listdir('Frames'):
		if filename.endswith(".jpg") or filename.endswith(".png") or filename.endswith(".jpeg"):
			result = api.detection.detect(img = File(os.path.join('Frames', filename))) #to detect   local images
			count = count+1
			attributeExtractionToCsv(result,fields,filename)
			print "frame "+ str(count)
		else:
			continue
	return perplexedFinalResult(name)
	#return uploaded_file()
	#return print_result(result)

def attributeExtractionToCsv(result,fields,filename):
	base_height = 48.787879
	face_id = result['face'][0]['face_id']
	eye_left_x = result['face'][0]['position']['eye_left']['x']
	eye_left_y = result['face'][0]['position']['eye_left']['y']
	eye_right_x = result['face'][0]['position']['eye_right']['x']
	eye_right_y = result['face'][0]['position']['eye_right']['y']
	nose_x = result['face'][0]['position']['nose']['x']
	nose_y = result['face'][0]['position']['nose']['y']
	mouth_left_x = result['face'][0]['position']['mouth_left']['x']
	mouth_left_y = result['face'][0]['position']['mouth_left']['y']
	mouth_right_x = result['face'][0]['position']['mouth_right']['x']
	mouth_right_y = result['face'][0]['position']['mouth_right']['y']
	sample_height = result['face'][0]['position']['height']
	
	dle_n = math.sqrt(((eye_left_x-nose_x)*(eye_left_x-nose_x))+((eye_left_y-nose_y)*(eye_left_y-nose_y)))
	dre_n = math.sqrt(((eye_right_x-nose_x)*(eye_right_x-nose_x))+((eye_right_y-nose_y)*(eye_right_y-nose_y)))
	dn_ml = math.sqrt(((nose_x-mouth_left_x)*(nose_x-mouth_left_x))+((nose_y-mouth_left_y)*(nose_y-mouth_left_y)))
	dn_mr = math.sqrt(((nose_x-mouth_right_x)*(nose_x-mouth_right_x))+((nose_y-mouth_right_y)*(nose_y-mouth_right_y)))
	
	csv_details = [{
	'File Name' : filename,
	'Face Id' : face_id,
	'Distance b/w left eye and nose' : dle_n *(base_height/sample_height),
	'Distance b/w right eye and nose' : dre_n *(base_height/sample_height),
	'Distance b/w nose and left side of mouth' : dn_ml *(base_height/sample_height),
	'Distance b/w nose and right side of mouth' : dn_mr *(base_height/sample_height)
	}]
	
	writeToCSV("testData.csv", fields, csv_details)
	
	#return render_template('csvToTable.html',result = csv_details)

def writeToCSV(fileName, fields, csv_details):
	with open(fileName,'a') as csvfile:
		writer = csv.DictWriter(csvfile,fieldnames = fields)
		writer.writerows(csv_details)
		
def initializeTestCSV(fields):
	with open("testData.csv",'w') as csvfile:
		writer = csv.DictWriter(csvfile,fieldnames = fields)
		writer.writeheader()

def perplexVideoResult():
	gaussHappy = 1
	gaussSad = 1
	gaussNeutral = 1
	probHappy = 0.333
	probSad = 0.333
	probNeutral = 0.333
	
	meanHappy = [15.60096197,	15.04479552,	13.14561,	14.30450974]
	meanNeutral = [14.79764948,	14.76956348,	14.92604009,	15.97522181]
	meanSad = [15.73483778,	15.99455832,	15.96757982,	16.0784396]

	
	sdHappy = [4.39062347,	3.778884273,	3.146677271,	3.686385466]
	sdSad = [3.814668676,	3.877983366,	2.61928689,	2.846513152]
	sdNeutral = [3.387115798,	3.619111747,	2.470246471,	2.120212889]

	
	happyCount = 0
	sadCount = 0
	neutralCount = 0
	
	testReader = csv.reader(open("testData.csv"))
	testReader.next()
	for row in testReader :
		for i in range(2,5):
			gaussHappy = gaussHappy * (1/(math.sqrt(2*math.pi)*sdHappy[i-2])*math.e**(-0.5*(float(float(row[i])-meanHappy[i-2])/sdHappy[i-2])**2))
			gaussSad = gaussSad * (1/(math.sqrt(2*math.pi)*sdSad[i-2])*math.e**(-0.5*(float(float(row[i])-meanSad[i-2])/sdSad[i-2])**2))
			gaussNeutral = gaussNeutral * (1/(math.sqrt(2*math.pi)*sdNeutral[i-2])*math.e**(-0.5*(float(float(row[i])-meanNeutral[i-2])/sdNeutral[i-2])**2))
		condProbHappy = math.pow(gaussHappy,0.25) * probHappy
		condProbSad = math.pow(gaussSad,0.25) * probSad
		condProbNeutral = math.pow(gaussNeutral,0.25) * probNeutral
		print "probabilities happy sad and neutral"+ str(condProbHappy)+" "+ str(condProbSad)+ " "+str(condProbNeutral)
		probResult = { 'Happy' : condProbHappy, 'Sad' : condProbSad, 'Neutral' : condProbNeutral}
		#max(probResult.iteritems(), key=operator.itemgetter(1))[0]
		key = max(probResult, key=probResult.get)
		if key == 'Happy':
			happyCount = happyCount + 1
		elif key == 'Sad':
			sadCount = sadCount + 1
		else:
			neutralCount = neutralCount + 1
		
		
	resultDict = {'Sad' : sadCount, 'Neutral' : neutralCount, 'Happy' : happyCount}
	key = max(resultDict, key = resultDict.get)
	#print key
	print "happyCount" + str(happyCount) + " sadCount"+ str(sadCount) + "neutralCount" + str(neutralCount)
	percentage = (float(resultDict[key])/(float(happyCount + sadCount + neutralCount))) * 100
	return key,percentage
		
def perplexedFinalResult(filename):
	key,percentage = perplexVideoResult()
	import videoToSpeech
	speechResult = videoToSpeech.speechKeywords(filename)
	print "This sentence is about: %s" % ", ".join(speechResult)
	return render_template('perplexedVideoResult.html',result = key, percent = percentage, speechWords = speechResult )
	
def uploaded_file():
	reader = csv.reader(open("testData.csv"))
	htmlfile = open("templates/csvpage.html","w")
	rownum = 0
	htmlfile.write('<table>')
	for row in reader: 
		if rownum == 0:
			htmlfile.write('<tr>')
		  	for column in row:
  				htmlfile.write('<th>' + column + '</th>')
		  	htmlfile.write('</tr>')
	  	else:
	  		htmlfile.write('<tr>')	
  			for column in row:
  				htmlfile.write('<td>' + column + '</td>')
	 		htmlfile.write('</tr>')
		rownum += 1
	htmlfile.write('</table>')
	print "Created " + str(rownum) + " row table."
	htmlfile.close()
	return render_template('csvpage.html')
	
if __name__ == '__main__':
    app.run(
        host="0.0.0.0",
        port=int("80"),
        debug=True
    )