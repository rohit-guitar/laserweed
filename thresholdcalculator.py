
import argparse
import sys
sys.path.append('/usr/local/lib/python2.7/site-packages')
import cv2
import numpy as np
from matplotlib import pyplot as plt

#To the points clicked on the image LAb HSV YCbCr YIQ
refPt = []
cropping = False
obj=-1

def click_and_crop(event, x, y, flags, param):
	# grab references to the global variables
	global refPt, cropping , obj

	# if the left mouse button was clicked, record the starting
	# (x, y) coordinates and indicate that cropping is being
	# performed
	if event == cv2.EVENT_LBUTTONDOWN:
		refPt = [(x, y)]
		cropping = True

	# check to see if the left mouse button was released
	elif event == cv2.EVENT_LBUTTONUP:
		refPt.append((x, y))
		cropping = False
		if obj==0:
			storeVegetation(refPt[0],refPt[1])

		elif obj==1:
			storeSoil(refPt[0],refPt[1])

		cv2.rectangle(image, refPt[0], refPt[1], (0, 255, 0), 2)
		cv2.imshow("image", image)

def storeVegetation(start,end):
	global veg,clone
	#print start[0],end[0],start[1],end[1]
	for i in range(start[1],end[1]):
		for j in range(start[0],end[0]):
			veg[i,j] = clone[i,j]

def storeSoil(start,end):
	global veg,clone
	for i in range(start[1],end[1]):
		for j in range(start[0],end[0]):
			soil[i,j] = clone[i,j]

def removeBlackPixels(image):
	size = np.count_nonzero((image[:,:,0]>0) & (image[:,:,1]>0) & (image[:,:,2]>0))
	blank=np.zeros((size,1,3), np.uint8);
	height, width, channels = image.shape	

	
#load the image, clone it, and setup the mouse callback function
image = cv2.imread("/Users/Rohit/LaserUf/new/0.jpg")
# hsv_img = cv2.cvtColor(image,cv2.COLOR_BGR2HSV)
# lower_blue = np.array([0, 0, 160], dtype=np.uint8)
# upper_blue = np.array([180,255,255], dtype=np.uint8)
# mask = cv2.inRange(hsv_img, lower_blue, upper_blue)
# cv2.imshow("Threshold",mask);
height, width, channels = image.shape
clone = image.copy()
cv2.namedWindow("image")
cv2.setMouseCallback("image", click_and_crop)
veg= np.zeros((height,width,channels), np.uint8);
soil= np.zeros((height,width,channels), np.uint8);


# keep looping until the 'q' key is pressed
while True:
	# display the image and wait for a keypress
	cv2.imshow("image", image)
	key = cv2.waitKey(1) & 0xFF

	# if the 'r' key is pressed, reset the cropping region
	if key == ord("r"):
		image = clone.copy()

	elif key == ord("v"):
		obj=0
		print "Vegetation mode"

	elif key == ord("s"):
		obj=1
		print "Soil Mode"

	# if the 'c' key is pressed, break from the loop
	elif key == ord("c"):
		break

# if there are two reference points, then crop the region of interest
# from teh image and display it
if len(refPt) == 2:
	roi = clone[refPt[0][1]:refPt[1][1], refPt[0][0]:refPt[1][0]]
	cv2.imshow("ROI", roi)
	cv2.imshow("Soil", soil);
	cv2.imshow("Veg", veg);

#removeBlackPixels(soil)

vegHSV= cv2.cvtColor(veg, cv2.COLOR_BGR2HSV)
soilHSV= cv2.cvtColor(soil, cv2.COLOR_BGR2HSV)

fig = plt.figure('RGB')

fig.add_subplot(311)
plt.hist(veg[...,2].flatten(), 256, range=(1, 255), fc='r' , facecolor='indianred',label='VegetationRed',histtype='step')
plt.hist(soil[...,2].flatten(), 256, range=(1, 255), fc='r' , facecolor='mistyrose',label='Soil Red',histtype='step')
plt.legend(loc='upper right')


fig.add_subplot(312)
plt.hist(veg[...,1].flatten(), 256, range=(1, 255), fc='g' ,facecolor='darkolivegreen',label='Vegetation Green',histtype='step')
plt.hist(soil[...,1].flatten(), 256, range=(1, 255), fc='g' ,facecolor='oldlace',label='Soil Green',histtype='step')
plt.legend(loc='upper right')


fig.add_subplot(313)
plt.hist(veg[...,0].flatten(), 256, range=(1, 255), fc='b' ,facecolor='blueviolet' ,label='Vegetation Blue',histtype='step')
plt.hist(soil[...,0].flatten(), 256, range=(1, 255), fc='b',facecolor='deepskyblue',label='Soil Blue',histtype='step')
plt.legend(loc='upper right')
#plt.show()


vegHSV= cv2.cvtColor(veg, cv2.COLOR_BGR2HSV)
soilHSV= cv2.cvtColor(soil, cv2.COLOR_BGR2HSV)

fig2= plt.figure('HSV')
fig2.add_subplot(311)
plt.hist(vegHSV[...,0].flatten(), 256, range=(1, 255) , facecolor='indianred',label='VegetationHue',histtype='step')
plt.hist(soilHSV[...,0].flatten(), 256, range=(1, 255) , facecolor='mistyrose',label='Soil HUE',histtype='step')
plt.legend(loc='upper right')


fig2.add_subplot(312)
plt.hist(vegHSV[...,1].flatten(), 256, range=(1, 255) ,facecolor='darkolivegreen',label='Vegetation Saturation',histtype='step')
plt.hist(soilHSV[...,1].flatten(), 256, range=(1, 255) ,facecolor='oldlace',label='Soil Saturation',histtype='step')
plt.legend(loc='upper right')


fig2.add_subplot(313)
plt.hist(vegHSV[...,2].flatten(), 256, range=(1, 255),facecolor='blueviolet' ,label='Vegetation Value',histtype='step')
plt.hist(soilHSV[...,2].flatten(), 256, range=(1, 255),facecolor='deepskyblue',label='Soil Value',histtype='step')
plt.legend(loc='upper right')
#plt.show()

vegLab= cv2.cvtColor(veg, cv2.COLOR_BGR2LAB)
soilLab= cv2.cvtColor(soil, cv2.COLOR_BGR2LAB)

fig3= plt.figure('LAb')
fig3.add_subplot(311)
plt.hist(vegLab[...,0].flatten(), 256, range=(1, 255) , facecolor='indianred',label='Vegetation L',histtype='step')
plt.hist(soilLab[...,0].flatten(), 256, range=(1, 255) , facecolor='mistyrose',label='Soil L',histtype='step')
plt.legend(loc='upper right')


fig3.add_subplot(312)
plt.hist(vegLab[...,1].flatten(), 256, range=(1, 255) ,facecolor='darkolivegreen',label='Vegetation A',histtype='step')
plt.hist(soilLab[...,1].flatten(), 256, range=(1, 255) ,facecolor='oldlace',label='Soil A',histtype='step')
plt.legend(loc='upper right')


fig3.add_subplot(313)
plt.hist(vegLab[...,2].flatten(), 256, range=(1, 255),facecolor='blueviolet' ,label='Vegetation B',histtype='step')
plt.hist(soilLab[...,2].flatten(), 256, range=(1, 255),facecolor='deepskyblue',label='Soil B',histtype='step')
plt.legend(loc='upper right')


vegYCC= cv2.cvtColor(veg, cv2.COLOR_BGR2YCR_CB)
soilYCC= cv2.cvtColor(soil, cv2.COLOR_BGR2YCR_CB)

fig4= plt.figure('YCbCr')
fig4.add_subplot(311)
plt.hist(vegYCC[...,0].flatten(), 256, range=(1, 255) , facecolor='indianred',label='Vegetation Y',histtype='step')
plt.hist(soilYCC[...,0].flatten(), 256, range=(1, 255) , facecolor='mistyrose',label='Soil Y',histtype='step')
plt.legend(loc='upper right')


fig4.add_subplot(312)
plt.hist(vegYCC[...,1].flatten(), 256, range=(1, 255) ,facecolor='darkolivegreen',label='Vegetation Cr',histtype='step')
plt.hist(soilYCC[...,1].flatten(), 256, range=(1, 255) ,facecolor='oldlace',label='Soil Cr',histtype='step')
plt.legend(loc='upper right')


fig4.add_subplot(313)
plt.hist(vegYCC[...,2].flatten(), 256, range=(1, 255),facecolor='blueviolet' ,label='Vegetation Cb',histtype='step')
plt.hist(soilYCC[...,2].flatten(), 256, range=(1, 255),facecolor='deepskyblue',label='Soil Cb',histtype='step')
plt.legend(loc='upper right')
plt.show()








cv2.waitKey(0)

# close all open windows
cv2.destroyAllWindows()

