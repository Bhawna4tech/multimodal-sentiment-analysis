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
	
	result = api.detection.detect(img = File(os.path.join('uploads/',str(name)))) #to detect   local images
	#result = api.detection.detect(url=url)  to detect online images
	return attributeExtractionToCsv(result)
	#return print_result(result)
	
	#return os.path.join('uploads/',name)
	#api.group.delete(group_name = 'test')
	#api.person.delete(person_name = [i[0] for i in PERSONS])

def attributeExtractionToCsv(result):
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
	img_height = result['img_height']
	#result = api.detection.detect(url=url)  to detect online images

	csv_details = [{
	'Face Id' : face_id,
	'Distance b/w left eye and nose' : math.sqrt(((eye_left_x-nose_x)*(eye_left_x-nose_x))+((eye_left_y-nose_y)*(eye_left_y-nose_y))),
	'Distance b/w right eye and nose' : math.sqrt(((eye_right_x-nose_x)*(eye_right_x-nose_x))+((eye_right_y-nose_y)*(eye_right_y-nose_y))),
	'Distance b/w nose and left side of mouth' : math.sqrt(((nose_x-mouth_left_x)*(nose_x-mouth_left_x))+((nose_y-mouth_left_y)*(nose_y-mouth_left_y))),
	'Distance b/w nose and right side of mouth' : math.sqrt(((nose_x-mouth_right_x)*(nose_x-mouth_right_x))+((nose_y-mouth_right_y)*(nose_y-mouth_right_y)))
	}]
	fields = ['Face Id' , 'Distance b/w left eye and nose' , 'Distance b/w right eye and nose' , 'Distance b/w nose and left side of mouth' , 'Distance b/w nose and right side of mouth']
	
	initializeTrainedCSV(fields)
	writeToCSV("trainedData.csv", fields, csv_details)
	return uploaded_file(csv_details)
	
	#return render_template('csvToTable.html',result = csv_details)

def writeToCSV(fileName, fields, csv_details):
	with open(fileName,'a') as csvfile:
		writer = csv.DictWriter(csvfile,fieldnames = fields)
		writer.writerows(csv_details)
		
def initializeTrainedCSV(fields):
	with open("trainedData.csv",'w') as csvfile:
		writer = csv.DictWriter(csvfile,fieldnames = fields)
		writer.writeheader()
		
def uploaded_file(filename):

	reader = csv.reader(open("trainedData.csv"))
	htmlfile = open("templates/csvpage.html","w")
	#htmlfile=os.fdopen(os.open("templates/hello2.html", os.O_WRONLY | os.O_CREAT, 0600), '-rwx') 
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