#include <iostream>
#include <algorithm>
#include <math.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <opencv2/ximgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
#define CHANNEL_NUM 3

void show(string window_name, cv::Mat img)
{
	cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
	cv::moveWindow(window_name, 700, 150);
	cv::imshow(window_name, img);
	cv::waitKey(0);
};

cv::Mat getSobel(cv::Mat img)
{
	cv::Mat grad;
	cv::Mat grad_x, grad_y;
	cv::Mat abs_grad_x, abs_grad_y;
	int ksize = -1;
	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;

	Sobel(img, grad_x, ddepth, 1, 0, ksize, scale, delta, cv::BORDER_DEFAULT);
	Sobel(img, grad_y, ddepth, 0, 1, ksize, scale, delta, cv::BORDER_DEFAULT);

	convertScaleAbs(grad_x, abs_grad_x);
	convertScaleAbs(grad_y, abs_grad_y);
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	return grad;
}

////*** Smoothing ***
void demo_Smoothing(cv::Mat& img)
{
	cv::Mat smoothMat;

	// BoxFilter example
	cv::boxFilter(img, smoothMat, -1, cv::Size(5, 5));
	show("boxFilter_Image", smoothMat);

	// Gaussian example
	cv::GaussianBlur(img, smoothMat, cv::Size(5, 5), 0);
	show("GaussianBlur_Image", smoothMat);

	// Median example
	cv::medianBlur(img, smoothMat, 5);
	show("MedianBlur_Image", smoothMat);

	// BilateralFilter example
	cv::bilateralFilter(img, smoothMat, 9, 75, 75);
	show("Bilateral_Image", smoothMat);

	// Filter2D example
	cv::Mat filter2DMat;
	cv::Mat kernel = cv::Mat::ones(5, 5, CV_32F) / (float)(5 * 5);
	cv::filter2D(img, filter2DMat, -1, kernel);
	show("filter2D_image", filter2DMat);
}

//*** Morphology ***
void demo_Morphology(cv::Mat& img)
{
	cv::Mat grayMat;
	cvtColor(img, grayMat, cv::COLOR_BGR2GRAY);

	cv::Mat binImage, gaussianMat;
	cv::GaussianBlur(grayMat, gaussianMat, cv::Size(5, 5), 0, 0);
	double maxVal = 255;
	cv::threshold(gaussianMat, binImage, 128, maxVal, cv::THRESH_BINARY);

	cv::Mat strctElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));

	// Erode example
	cv::Mat erodeMat;
	cv::erode(binImage, erodeMat, strctElement);
	show("Erode_image", erodeMat);

	// Dilate example
	cv::Mat dilateMat;
	cv::dilate(binImage, dilateMat, strctElement);
	show("Dilate_image", dilateMat);

	// MorphologyEx example
	cv::Mat morphMat;
	cv::morphologyEx(dilateMat, morphMat, cv::MORPH_ERODE, strctElement);
	show("morphologyEx_erode_image", morphMat);
	cv::morphologyEx(erodeMat, morphMat, cv::MORPH_DILATE, strctElement);
	show("morphologyEx_dilate_image", morphMat);
}

////*** Edge detection ***
void demo_EdgeDetection(cv::Mat& img)
{
	cv::Mat grayMat;
	cvtColor(img, grayMat, cv::COLOR_BGR2GRAY);

	// Sobel example
	cv::Mat sobelMat = getSobel(grayMat);
	show("sobel_image", sobelMat);

	// Scharr example
	cv::Mat scharrMat;
	cv::Scharr(grayMat, scharrMat, cv::FILTER_SCHARR, 0, 1);
	show("scharr_image", scharrMat);

	// Laplacian example
	cv::Mat laplacianMat;
	cv::Laplacian(grayMat, laplacianMat, CV_16S);
	cv::convertScaleAbs(laplacianMat, laplacianMat);
	show("laplacian_image", laplacianMat);

	// Canny example
	cv::Mat cannyMat;
	cv::Canny(grayMat, cannyMat, 100, 200);
	show("Canny_image", cannyMat);
}

////*** FFT ***
void demo_FFT(cv::Mat& img)
{
	cv::Mat grayMat;
	cvtColor(img, grayMat, cv::COLOR_BGR2GRAY);
	grayMat.convertTo(grayMat, CV_64FC1);

	cv::Mat padded;
	int m = cv::getOptimalDFTSize(grayMat.rows);
	int n = cv::getOptimalDFTSize(grayMat.cols);
	copyMakeBorder(grayMat, padded, 0, m - grayMat.rows, 0, n - grayMat.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

	cv::Mat planes[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };
	cv::Mat complexI;
	cv::merge(planes, 2, complexI);
	cv::dft(complexI, complexI);

	cv::split(complexI, planes);
	cv::magnitude(planes[0], planes[1], planes[0]);
	cv::Mat magI = planes[0];
	magI += cv::Scalar::all(1);
	log(magI, magI);
	magI = magI(cv::Rect(0, 0, magI.cols & -2, magI.rows & -2));

	int cx = magI.cols / 2;
	int cy = magI.rows / 2;
	cv::Mat q0(magI, cv::Rect(0, 0, cx, cy));
	cv::Mat q1(magI, cv::Rect(cx, 0, cx, cy));
	cv::Mat q2(magI, cv::Rect(0, cy, cx, cy));
	cv::Mat q3(magI, cv::Rect(cx, cy, cx, cy));

	cv::Mat tmp;
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);
	normalize(magI, magI, 0, 1, cv::NORM_MINMAX);

	show("FFT_image", magI);
}

////*** Hough transform ***
void demo_HoughTransform(cv::Mat& img)
{
	cv::Mat grayMat;
	cvtColor(img, grayMat, cv::COLOR_BGR2GRAY);

	cv::Mat cannyMat;
	cv::Canny(grayMat, cannyMat, 100, 200);

	vector<cv::Vec4i> lines;
	int threshold = 150;
	cv::HoughLinesP(cannyMat, lines, 2, 2 * std::atan(1.0) * 4 / 180, threshold);

	cv::cvtColor(cannyMat, cannyMat, cv::COLOR_GRAY2BGR);
	for (int i = 0; i < lines.size(); i++) {
		cv::Vec4i c = lines[i];
		cv::Point lineStart = cv::Point(c[0], c[1]);
		cv::Point lineEnd = cv::Point(c[2], c[3]);
		cv::line(cannyMat, lineStart, lineEnd, cv::Scalar(0, 255, 0), 3);
	}

	show("line_image", cannyMat);
}

////*** Circle detection ***
void demo_CircleDetection(cv::Mat& img)
{
	cv::Mat grayMat;
	cvtColor(img, grayMat, cv::COLOR_BGR2GRAY);

	vector<cv::Vec3f> circles;
	cv::HoughCircles(grayMat, circles, cv::HOUGH_GRADIENT, grayMat.rows / 16, 170, 30, 20, 25);

	for (int i = 0; i < circles.size(); i++)
	{
		cv::Vec3i c = circles[i];
		cv::Point center = cv::Point(c[0], c[1]);
		int radius = c[2];
		circle(grayMat, center, radius, cv::Scalar(0, 255, 0), 3);
	}

	show("circles_image", grayMat);
}

void chooseFunction(string funcName, cv::Mat& img)
{
	if (funcName.compare("smoothing") == 0)
		demo_Smoothing(img);
	else if (funcName.compare("morphology") == 0)
		demo_Morphology(img);
	else if (funcName.compare("edgedetection") == 0)
		demo_EdgeDetection(img);
	else if (funcName.compare("fft") == 0)
		demo_FFT(img);
	else if (funcName.compare("houghtransform") == 0)
		demo_HoughTransform(img);
	else if (funcName.compare("circledetection") == 0)
		demo_CircleDetection(img);
	else if (funcName.compare("ex") == 0)
		exit(0);
	else
		cout << "Wrong function name!" << endl;
}

int main()
{
	cv::Mat img = cv::imread("example_img.jpg");
	show("original_image", img);

	string func;
	cin >> func;
	chooseFunction(func, img);

	return 0;
}