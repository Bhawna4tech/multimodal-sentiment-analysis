import arffread
import convert
import first
import attributes
import time

def audioFeatures(filename):
	print " in audio features"
	arffread.videoToAudio(filename)
	convert.convertSox()
	time.sleep(3)
	first.extractFeatures(filename)
	time.sleep(3)
	#time.sleep(5000)
	attributes.featuresToCsv()

#audioFeatures()