#!/usr/bin/env python2
# -*- coding: utf-8 -*-
# $File: hello.py

# In this tutorial, you will learn how to call Face ++ APIs and implement a
# simple App which could recognize a face image in 3 candidates.
# 在本教程中，您将了解到Face ++ API的基本调用方法，并实现一个简单的App，用以在3
# 张备选人脸图片中识别一个新的人脸图片。

# You need to register your App first, and enter you API key/secret.
# 您需要先注册一个App，并将得到的API key和API secret写在这里。
API_KEY = '0ec41f73620f6ec21ad5c986208aa328'
API_SECRET = '3eCGX3rj5Rj3xAejIHGDjJZxCuZIa7Rx'

# Import system libraries and define helper functions
# 导入系统库并定义辅助函数
import time
import math
import csv
from pprint import pformat
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
    print '\n'.join(['  ' + i for i in pformat(result, width = 75).split('\n')])

# First import the API class from the SDK
# 首先，导入SDK中的API类
from facepp import API, File

api = API(API_KEY, API_SECRET)
# Here are the person names and their face images
# 人名及其脸部图片
url = "http://blogs.reuters.com/great-debate/files/2013/07/obama-best.jpg"
result = api.detection.detect(img = File('C:/Users/Bhawana/Desktop/bhaw.jpg'))
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
'Distance b/w left eye and nose' : math.sqrt(((eye_left_x-nose_x)*(eye_left_x-nose_x))+((eye_left_y-nose_y)*(eye_left_y-nose_y))),
'distance b/w right eye and nose' : math.sqrt(((eye_right_x-nose_x)*(eye_right_x-nose_x))+((eye_right_y-nose_y)*(eye_right_y-nose_y))),
'distance b/w nose and left side of mouth' : math.sqrt(((nose_x-mouth_left_x)*(nose_x-mouth_left_x))+((nose_y-mouth_left_y)*(nose_y-mouth_left_y))),
'distance b/w nose and right side of mouth' : math.sqrt(((nose_x-mouth_right_x)*(nose_x-mouth_right_x))+((nose_y-mouth_right_y)*(nose_y-mouth_right_y)))
}]

fields = ['Distance b/w left eye and nose' , 'distance b/w right eye and nose', 'distance b/w nose and left side of mouth', 'distance b/w nose and right side of mouth']
fileName = "trainedData.csv"

with open(fileName,'w') as csvfile:
	writer = csv.DictWriter(csvfile,fieldnames = fields)
	writer.writeheader()
	writer.writerows(csv_details)

print_result(result) 
print_result(csv_details)
#print_result(face_id)
