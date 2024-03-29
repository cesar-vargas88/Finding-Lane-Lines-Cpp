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

Mat getBGR(Mat source);
Mat getGray(Mat source);
Mat getBlur(Mat source, int kernelSize, int borderType);
Mat getEdge(Mat source, double minThreshold, double maxThreshold, int apertureSize);
Mat getInterestRegion(Mat source, vector<Point> vertices);
Mat getLines(Mat source, double rho, double theta, int threshold, double minLineLength, double maxLineGap);
Mat drawLines(Mat source, vector<Vec4i> vlines, cv::Scalar color, int thickness);
Mat getWeighted(Mat source1, Mat source2, double α, double β, double γ);
pair<double, double> simpleLinearRegression(vector<Point> vPoints);

int main()
{
	Mat window(Size(imageWidht * 3, imageHeight * 2), CV_8UC3);

	string sourcePath = "Videos/";
	vector<String> imagesNames;

	// Get all images in the source folder
	glob(sourcePath, imagesNames);
	//Mat source = imread(imagesNames[0]);
	// Verify that image opened correctly
	//if (!source.data)
	//{
	//	cout << "Could not open or find the image" << std::endl;
	//	return -1;
	//}

	VideoCapture cap(imagesNames[2]);

	// Check if camera opened successfully
	if (!cap.isOpened()) 
	{
		cout << "Error opening video stream or file" << endl;
		return -1;
	}

	Mat source;

	while (true)
	{
		cap >> source;

		// If the frame is empty, break immediately
		if (source.empty())
			break;
		
		// Image processing
		resize(source, source, Size(imageWidht, imageHeight));			// Resize the image						resize(InputArray, OutputArray, KernelSize)

		vector<Point> vertices;
		vertices.push_back(Point(source.cols * 0.10 , source.rows * 0.90));
		vertices.push_back(Point(source.cols * 0.46 , source.rows * 0.60));
		vertices.push_back(Point(source.cols * 0.54 , source.rows * 0.60));
		vertices.push_back(Point(source.cols * 0.90 , source.rows * 0.90));
	
		Mat gray = getGray(source);
		Mat blur = getBlur(gray, 7, 0);
		Mat edge = getEdge(blur, 100, 200, 3);
		Mat interestRegion = getInterestRegion(edge, vertices);
		Mat lines = getLines(interestRegion, 10, CV_PI / 180, 25, 5, 5);
		Mat weighted = getWeighted(lines, getBGR(edge), 0.8, 1, 0);
	
		/*
		source.copyTo(window(Rect(0, 0, imageWidht, imageHeight)));
		getBGR(gray).copyTo(window(Rect(imageWidht * 1,0, imageWidht, imageHeight)));
		getBGR(interestRegion).copyTo(window(Rect(imageWidht * 2, 0, imageWidht, imageHeight)));
		lines.copyTo(window(Rect(0, imageHeight, imageWidht, imageHeight)));
		weighted.copyTo(window(Rect(imageWidht * 1, imageHeight, imageWidht, imageHeight)));
		weighted.copyTo(window(Rect(imageWidht * 2, imageHeight, imageWidht, imageHeight)));

		namedWindow("Pipeline", WINDOW_NORMAL);
		imshow("Pipeline", window);
		*/

		// Display the resulting frame
		imshow("Frame", weighted);

		// Press  ESC on keyboard to exit
		char c = (char)waitKey(25);

		if (c == 27)
			break;
	}

	return 0;
}

Mat getBGR(Mat source) 
{
	Mat BGR;

	cvtColor(source, BGR, COLOR_GRAY2BGR);

	return BGR;
}

Mat getGray(Mat source)
{
	Mat gray;

	cvtColor(source, gray, COLOR_BGR2GRAY);

	return gray;
}

Mat getBlur(Mat source, int kernelSize, int borderType)
{
	Mat blur;

	GaussianBlur(source, blur, Size(kernelSize, kernelSize), borderType);

	return blur;
}

Mat getEdge(Mat source, double minThreshold, double maxThreshold, int apertureSize)
{ 
	Mat edge;

	Canny(source, edge, minThreshold, maxThreshold, apertureSize);	

	return edge;
}

Mat getInterestRegion(Mat source, vector<Point> vertices)
{
	Mat interestRegion = source.clone();
	// Create interest area image
	Mat mask(Size(source.cols, source.rows), CV_8UC1, Scalar(0, 0, 0));
	fillConvexPoly(mask, vertices, Scalar(255, 255, 255));
	bitwise_and(mask, source, interestRegion);

	return interestRegion;
}

Mat getLines(Mat source, double rho, double theta, int threshold, double minLineLength, double maxLineGap)
{
	vector<Vec4i> vlines;						
	HoughLinesP(source, vlines, rho, theta, threshold, minLineLength, maxLineGap);
	Mat lines = drawLines(source, vlines, Scalar(0, 0, 255), 2);

	return lines;
}

Mat drawLines(Mat source, vector<Vec4i> vlines, cv::Scalar color, int thickness)
{
	Mat lines(Size(source.cols, source.rows), CV_8UC3, Scalar(0, 0, 0));

	vector<Point> vPointsLeft;
	vector<Point> vPointsRight;

	for (size_t i = 0; i < vlines.size(); i++)
	{
		//line(lines, Point(vlines[i][0], vlines[i][1]), Point(vlines[i][2], vlines[i][3]), color, thickness, 8);

		// Gets the midpoint of a line
		int PosX = (vlines[i][0] + vlines[i][2]) * 0.5;
		int PosY = (vlines[i][1] + vlines[i][3]) * 0.5;

		circle(lines, Point(PosX, PosY), 1, Scalar(0, 255, 0), thickness, 8, 0);

		// Determines whether the midpoint is loaded on the right or left side of the image and classifies it
		if (PosX < imageWidht * 0.5)
			vPointsLeft.push_back(Point(PosX, PosY));	
		else
			vPointsRight.push_back(Point(PosX, PosY));
	}

	// Get m and b from linear regression
	pair<double, double> lineLeft	= simpleLinearRegression(vPointsLeft);
	pair<double, double> lineRight	= simpleLinearRegression(vPointsRight);

	// Define the points of left line     x = (y - b) / m 
	Point leftButton(((imageHeight * 1.0) - lineLeft.second) / lineLeft.first, imageHeight * 1.0);
	Point leftTop(((imageHeight * 0.6) - lineLeft.second) / lineLeft.first, imageHeight * 0.6);

	// Define the points of right line   x = (y - b) / m 
	Point rightButton(((imageHeight * 1.0) - lineRight.second) / lineRight.first, imageHeight * 1.0);
	Point rightTop(((imageHeight * 0.6) - lineRight.second) / lineRight.first, imageHeight * 0.6);

	// Draw the lines
	line(lines, leftButton, leftTop, color, thickness, 8);
	line(lines, rightButton, rightTop, color, thickness, 8);

	return lines;
}

Mat getWeighted(Mat source1, Mat source2, double α, double β, double γ)
{
	Mat weighted;
	addWeighted(source1, α, source2, β, 0.0, weighted);

	return weighted;
}

pair<double, double> simpleLinearRegression(vector<Point> vPoints)
{
	int n = vPoints.size();
	double SumX		= 0;
	double SumY		= 0;
	double SumXY	= 0;
	double SumXX	= 0;
	double m		= 0;
	double b		= 0;

	for (int index = 0; index < n; index++)
	{
		SumX += vPoints[index].x;
		SumY += vPoints[index].y;
		SumXX += vPoints[index].x * vPoints[index].x;
		SumXY += vPoints[index].x * vPoints[index].y;
	}
	
	m = ((n * SumXY) - (SumX * SumY)) / ((n * SumXX) - (SumX * SumX));
	b = ((SumY * SumXX) - (SumX * SumXY)) / ((n * SumXX) - (SumX * SumX));

	return make_pair(m, b);
}
