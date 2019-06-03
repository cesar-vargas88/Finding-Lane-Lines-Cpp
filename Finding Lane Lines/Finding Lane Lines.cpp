// Finding Lane Lines.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#define imageWidht  640
#define imageHeight 480

int main()
{
	Mat source;
	Mat gray;
	Mat blur;
	Mat edges;
	Mat lines;
	Mat windowColor(Size(imageWidht * 3, imageHeight), CV_8UC3);
	Mat windowGray(Size(imageWidht * 3, imageHeight), CV_8UC1);

	string sourcePath = "Images/";
	vector<String> imagesNames;

	// Get all images in the source folder
	glob(sourcePath, imagesNames);
	source = imread(imagesNames[0]);

	// Verify that image opened correctly
	if (!source.data)
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// Process the image
	//resize(source, source, Size(imageWidht, imageHeight));			// Resize the image				resize(InputArray, OutputArray, KernelSize)
	cvtColor(source, gray, COLOR_BGR2GRAY);							// Convert to gray scale		cvtColor(InputArray,OutputArray,ColorConversionCodes)
	GaussianBlur(gray, blur, Size(3, 3), 0);						// Blur the image				GaussianBlur(InputArray, OutputArray , borderType)
	Canny(blur, edges, 50, 150, 3);									// Edge detection				Canny(InputArray OutputArray, minThreshold, maxThreshold, apertureSize)	
	// Line detection
	vector<Vec4i> vlines;
	cvtColor(edges, lines, COLOR_GRAY2BGR);
	HoughLinesP(edges, vlines, 1, CV_PI / 180, 80, 10, 1);			// HoughLinesP(InputArray, OutputArray, rho, theta, threshold, minLineLength, maxLineGap)

	for (size_t i = 0; i < vlines.size(); i++)
		line(lines, Point(vlines[i][0], vlines[i][1]), Point(vlines[i][2], vlines[i][3]), Scalar(0, 0, 255), 3, 8);



	// Show the images 
	/*source.copyTo(windowColor(Rect(0	, 0	, imageWidht, imageHeight)));
	lines.copyTo(windowColor(Rect(640 , 0 , imageWidht, imageHeight)));
	source.copyTo(windowColor(Rect(1280 , 0 , imageWidht, imageHeight)));

	gray.copyTo(windowGray(Rect(0 , 0	, imageWidht, imageHeight)));
	blur.copyTo(windowGray(Rect(640 , 0	, imageWidht, imageHeight)));
	edges.copyTo(windowGray(Rect(1280 , 0 , imageWidht, imageHeight)));

	namedWindow("Color", WINDOW_NORMAL);
	namedWindow("Gray", WINDOW_NORMAL);
	imshow("Color", windowColor);
	imshow("Gray", windowGray);
	*/
	
	imshow("source", source);
	imshow("gray", gray);
	imshow("blur", blur);
	imshow("canny",edges);
	imshow("lines", lines);
	

	waitKey(0);
	return 0;
}
