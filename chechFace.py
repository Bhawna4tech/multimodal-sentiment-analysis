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
from facepp import API, File
import os
from pprint import pformat
def print_result(hint, result):
    def encode(obj):
        if type(obj) is unicode:
            return obj.encode('utf-8')
        if type(obj) is dict:
            return {encode(k): encode(v) for (k, v) in obj.iteritems()}
        if type(obj) is list:
            return [encode(i) for i in obj]
        return obj
    print (hint)
    result = encode(result)
    print ('\n'.join(['  ' + i for i in pformat(result, width = 75).split('\n')]))

# First import the API class from the SDK
# 首先，导入SDK中的API类
from facepp import API

api = API(API_KEY, API_SECRET)

# Here are the person names and their face images
# 人名及其脸部图片


# Step 3: Train the group.
# Note: this step is required before performing recognition in this group,
# since our system needs to pre-compute models for these persons
# 步骤3：训练这个group
# 注：在group中进行识别之前必须执行该步骤，以便我们的系统能为这些person建模


# Because the train process is time-consuming, the operation is done
# asynchronously, so only a session ID would be returned.
# 由于训练过程比较耗时，所以操作必须异步完成，因此只有session ID会被返回
# Now, wait before train completes
# 等待训练完成


#也可以通过调用api.wait_async(session_id)函数完成以上功能


# Step 4: recognize the unknown face image
# 步骤4：识别未知脸部图片
result = api.detection.detect(img = File(os.path.join('uploads/bhaw.jpg'))) #to detect   local images
face_ids = result['face'][0]['face_id']
result2 = api.detection.landmark(face_id = face_ids) #to detect   local images
print_result('Recognize result:', result2)



# Finally, delete the persons and group because they are no longer needed
# 最终，删除无用的person和group


# Congratulations! You have finished this tutorial, and you can continue
# reading our API document and start writing your own App using Face++ API!
# Enjoy :)
# 恭喜！您已经完成了本教程，可以继续阅读我们的API文档并利用Face++ API开始写您自
# 己的App了！
# 旅途愉快 :)
