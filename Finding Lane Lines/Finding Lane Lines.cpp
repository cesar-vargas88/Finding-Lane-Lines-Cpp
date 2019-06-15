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
	source = imread(imagesNames[3]);

	// Verify that image opened correctly
	if (!source.data)
	{
		cout << "Could not open or find the image" << std::endl;
		return -1;
	}

	// Image processing
	resize(source, source, Size(imageWidht, imageHeight));			// Resize the image						resize(InputArray, OutputArray, KernelSize)

	// Create interest area image
	Mat interestArea(Size(source.cols, source.rows), CV_8UC3, Scalar(0, 0, 0));
	vector<Point> points;
	points.push_back(Point(source.cols *.4, source.rows * 0.5));
	points.push_back(Point(source.cols * 0.6, source.rows * 0.5));
	points.push_back(Point(source.cols, source.rows * 0.8));
	points.push_back(Point(source.cols, source.rows));
	points.push_back(Point(0, source.rows));
	points.push_back(Point(0, source.rows * 0.8));
	fillConvexPoly(interestArea, points, Scalar(255, 255, 255));
	cvtColor(interestArea, interestArea, COLOR_BGR2GRAY);

	// Finding lane lines pipeline
	GaussianBlur(source, blur, Size(5, 5), 0);						// Blur the image						GaussianBlur(InputArray, OutputArray , borderType)
	cvtColor(blur, gray, COLOR_BGR2GRAY);							// Convert BGR to gray scale			cvtColor(InputArray,OutputArray,ColorConversionCodes)
	Canny(gray, edges, 140, 210, 3);								// Edge detection						Canny(InputArray OutputArray, minThreshold, maxThreshold, apertureSize)	
	bitwise_and(interestArea, edges, edges);						// Mask edge and interestArea images	bitwise_and(InputArray, InputArray, OutputArray)

	// Line detection
	vector<Vec4i> vlines;
	//cvtColor(edges, lines, COLOR_GRAY2BGR);							// Convert gray to BGR scale			cvtColor(InputArray,OutputArray,ColorConversionCodes)
	HoughLinesP(edges, vlines, 1, CV_PI / 180, 80, 100, 20);		// Dtect lines							HoughLinesP(InputArray, OutputArray, rho, theta, threshold, minLineLength, maxLineGap)

	// Draw the lines
	lines = source.clone();
	for (size_t i = 0; i < vlines.size(); i++)
		line(lines, Point(vlines[i][0], vlines[i][1]), Point(vlines[i][2], vlines[i][3]), Scalar(0, 0, 255), 3, 8);

	// Show the images 
	source.copyTo(windowColor(Rect(0, 0, imageWidht, imageHeight)));
	blur.copyTo(windowColor(Rect(640, 0, imageWidht, imageHeight)));
	lines.copyTo(windowColor(Rect(1280, 0, imageWidht, imageHeight)));

	gray.copyTo(windowGray(Rect(0, 0, imageWidht, imageHeight)));
	interestArea.copyTo(windowGray(Rect(640, 0, imageWidht, imageHeight)));
	edges.copyTo(windowGray(Rect(1280, 0, imageWidht, imageHeight)));

	namedWindow("Color", WINDOW_NORMAL);
	namedWindow("Gray", WINDOW_NORMAL);
	imshow("Color", windowColor);
	imshow("Gray", windowGray);

	waitKey(0);
	return 0;
}


