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

app = Flask(__name__)

app.config['UPLOAD_FOLDER'] = 'uploads'
app.config['ALLOWED_EXTENSIONS'] = set(['jpg','png'])


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


@app.route('/')
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
		file.save(os.path.join(app.config['UPLOAD_FOLDER'], filename))
		#return send_from_directory(app.config['UPLOAD_FOLDER'],filename)
		#filename is the real file name like "bhawna.jpg"
		return redirect(url_for('processing',name=filename))

@app.route('/upload/processing/<name>')		
def processing(name):
	api = API(API_KEY, API_SECRET)
	#url = "http://blogs.reuters.com/great-debate/files/2013/07/obama-best.jpg"
	
	#result = api.detection.detect(img = File(os.path.join('uploads/',str(name)))) #to detect   local images
	#result = api.detection.detect(url=url)  to detect online images
	#return attributeExtractionToCsv(result)
	
	fields = ['File Name' , 'Face Id' , 'Distance b/w left eye and nose' , 'Distance b/w right eye and nose' , 'Distance b/w nose and left side of mouth' , 'Distance b/w nose and right side of mouth']
	initializeTrainedCSV(fields)
	
	for filename in os.listdir('Training dataset'):
		if filename.endswith(".jpg") or filename.endswith(".png"):
			result = api.detection.detect(img = File(os.path.join('Training dataset', filename))) #to detect   local images
			attributeExtractionToCsv(result,fields,filename)
		else:
			continue
	return uploaded_file()
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
	
	writeToCSV("trainedData.csv", fields, csv_details)
	
	#return render_template('csvToTable.html',result = csv_details)

def writeToCSV(fileName, fields, csv_details):
	with open(fileName,'a') as csvfile:
		writer = csv.DictWriter(csvfile,fieldnames = fields)
		writer.writerows(csv_details)
		
def initializeTrainedCSV(fields):
	with open("trainedData.csv",'w') as csvfile:
		writer = csv.DictWriter(csvfile,fieldnames = fields)
		writer.writeheader()
		
def uploaded_file():
	reader = csv.reader(open("trainedData.csv"))
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