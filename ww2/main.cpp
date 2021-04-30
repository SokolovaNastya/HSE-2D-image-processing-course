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

void GammaCorrection(cv::Mat& src, cv::Mat& dst, float fGamma)
{
	unsigned char lut[256];
	for (int i = 0; i < 256; i++)
	{
		lut[i] = cv::saturate_cast<uchar>(pow((float)(i / 255.0), fGamma) * 255.0f);
	}
	dst = src.clone();

	const int channels = dst.channels();
	switch (channels)
	{
	case 1:
	{
		cv::MatIterator_<uchar> it, end;
		for (it = dst.begin<uchar>(), end = dst.end<uchar>(); it != end; it++)
			*it = lut[(*it)];
		break;
	}
	case 3:
	{
		cv::MatIterator_<cv::Vec3b> it, end;
		for (it = dst.begin<cv::Vec3b>(), end = dst.end<cv::Vec3b>(); it != end; it++)
		{
			(*it)[0] = lut[((*it)[0])];
			(*it)[1] = lut[((*it)[1])];
			(*it)[2] = lut[((*it)[2])];
		}
		break;
	}
	}
}

cv::Mat getHistogram(cv::Mat& img)
{
	int histSize = 256;
	float range[] = { 0, 256 };
	const float* ranges[] = { range };
	bool uniform = true, accumulate = false;

	cv::Mat hist;
	cv::calcHist(&img, 1, 0, cv::Mat(), hist, 1, &histSize, ranges, uniform, accumulate);

	int hist_w = 1024; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	cv::Mat histImage(hist_h, hist_w, CV_8UC1, cv::Scalar(0, 0, 0));
	normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

	for (int i = 1; i < histSize; i++)
	{
		line(histImage, cv::Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
			cv::Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
			cv::Scalar(255, 0, 0), 2, 8, 0);
	}

	return histImage;
}


////*** HSV ***
void demo_HSV(cv::Mat& img)
{
	cv::Mat HSV;
	cv::cvtColor(img, HSV, cv::COLOR_BGR2HSV);

	vector<cv::Mat> hsv_list(CHANNEL_NUM);
	cv::split(HSV, hsv_list);

	vector<cv::Mat> hsv_list_corrected(CHANNEL_NUM);
	for (int channel = 0; channel < CHANNEL_NUM; ++channel)
	{
		double minVal, maxVal = 0;
		cv::minMaxLoc(hsv_list[channel], &minVal, &maxVal);

		cv::Mat corrected;
		hsv_list[channel].convertTo(corrected, CV_8U, 255.0 / (maxVal - minVal), -minVal * 255.0 / (maxVal - minVal));
		hsv_list_corrected[channel] = corrected;
	}

	cv::Mat HSV_out;
	cv::merge(hsv_list_corrected, HSV_out);
	cv::cvtColor(HSV_out, HSV_out, cv::COLOR_HSV2BGR);

	show("HSV_image", HSV_out);
}


////*** Grayscale ***
void demo_Grayscale(cv::Mat& img)
{
	cv::Mat grayMat;
	cvtColor(img, grayMat, cv::COLOR_BGR2GRAY);

	// If you read a jpg file, a 3 channel image is created by default.If you need a grayscale image, use:
	// grayMat = cv::imread("example_img.jpg", cv::IMREAD_GRAYSCALE);

	show("gray_image", grayMat);
}


////*** Normalization ***
void demo_Normalization(cv::Mat& img)
{
	cv::Mat normMat;

	// NORM_INF example
	cv::normalize(img, normMat, 128, 255, cv::NORM_INF);
	show("normInf_image", normMat);

	// NORM_L1 example
	cv::normalize(img, normMat, 1, 0, cv::NORM_L1);
	show("normL1_image", normMat);
	cout << "normL1 = " << endl << " " << normMat(cv::Rect(1, 1, 20, 20)) << endl << endl;

	// NORM_L2 example
	cv::normalize(img, normMat, 1, 0, cv::NORM_L2);
	show("normL2_image", normMat);

	// NORM_MINMAX example
	cv::normalize(img, normMat, 128, 255, cv::NORM_MINMAX);
	show("normMinMax_image", normMat);
}


////*** Gamma Correction ***
void demo_GammaCorrection(cv::Mat& img)
{
	cv::Mat gammaMat(img.rows, img.cols, img.type());
	double gammaValue = 1.3;

	// Direct function call example
	GammaCorrection(img, gammaMat, gammaValue);
	show("gammaCorrection_image", gammaMat);

	// HSV + GammaCorrection example
	cv::Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = cv::saturate_cast<uchar>(pow((float)(i / 255.0), gammaValue) * 255.0f);

	cv::Mat HSV;
	cvtColor(img, HSV, cv::COLOR_BGR2HSV);

	vector<cv::Mat> hsv_list(CHANNEL_NUM);
	cv::split(HSV, hsv_list);

	vector<cv::Mat> hsv_list_corrected(CHANNEL_NUM);
	for (int channel = 0; channel < CHANNEL_NUM; ++channel) {
		cv::Mat corrected;
		cv::LUT(hsv_list[channel], lookUpTable, corrected);
		hsv_list_corrected[channel] = corrected;
	}

	cv::Mat HSV_out;
	cv::merge(hsv_list_corrected, HSV_out);
	cvtColor(HSV_out, HSV_out, cv::COLOR_HSV2BGR);
	show("gammaCorrection_HSV_image", HSV_out);

	// Gray image example
	cv::Mat grayMat;
	cvtColor(img, grayMat, cv::COLOR_BGR2GRAY);

	cv::Mat gray_out;
	LUT(grayMat, lookUpTable, gray_out);
	show("gammaCorrection_Gray_image", gray_out);
}


////*** Histograms ***
void demo_Histograms(cv::Mat& img)
{
	cv::Mat grayMat;
	cvtColor(img, grayMat, cv::COLOR_BGR2GRAY);

	cv::Mat histogram = getHistogram(grayMat);
	show("histogram_image", histogram);

	// Direct function call example
	cv::Mat eqHistogram;
	cv::equalizeHist(grayMat, eqHistogram);
	show("eqHistogram_image", eqHistogram);

	// HSV + Histogram Equalization example
	cv::Mat HSV;
	cvtColor(img, HSV, cv::COLOR_BGR2HSV);

	vector<cv::Mat> hsv_list(CHANNEL_NUM);
	cv::split(HSV, hsv_list);

	vector<cv::Mat> hsv_list_equalized(CHANNEL_NUM);
	for (int channel = 0; channel < CHANNEL_NUM; ++channel) {
		cv::Mat equalized;
		cv::equalizeHist(hsv_list[channel], equalized);
		hsv_list_equalized[channel] = equalized;
	}

	cv::Mat HSV_out;
	cv::merge(hsv_list_equalized, HSV_out);
	cvtColor(HSV_out, HSV_out, cv::COLOR_HSV2BGR);
	show("eqHistogram_HSV_image", HSV_out);
}


////*** Binarization ***
void demo_Binarization(cv::Mat& img)
{
	cv::Mat grayMat;
	cvtColor(img, grayMat, cv::COLOR_BGR2GRAY);

	cv::Mat binImage, gaussianMat;
	cv::GaussianBlur(grayMat, gaussianMat, cv::Size(5, 5), 0, 0);
	double maxVal = 255;

	// THRESH_BINARY example
	cv::threshold(gaussianMat, binImage, 128, maxVal, cv::THRESH_BINARY);
	show("bin_image", binImage);

	// THRESH_BINARY_INV example
	cv::threshold(gaussianMat, binImage, 128, maxVal, cv::THRESH_BINARY_INV);
	show("binInv_image", binImage);

	// ADAPTIVE_THRESH example
	cv::threshold(gaussianMat, binImage, 64, maxVal, cv::ADAPTIVE_THRESH_GAUSSIAN_C);
	show("binAdaptive_image", binImage);

	// THRESH_OTSU example
	cv::threshold(gaussianMat, binImage, 0, maxVal, cv::THRESH_OTSU);
	show("binOtsu_image", binImage);

	// niBlackThreshold example
	// Need opencv_contrib (opencv_ximgproc451d.lib)
	cv::ximgproc::niBlackThreshold(gaussianMat, binImage, maxVal, cv::THRESH_BINARY, 35, 0, cv::ximgproc::BINARIZATION_NIBLACK);
	show("binNiBlackThreshold_image", binImage);
}

void chooseFunction(string funcName, cv::Mat& img)
{
	if (funcName.compare("hsv") == 0)
		demo_HSV(img);
	else if (funcName.compare("grayscale") == 0)
		demo_Grayscale(img);
	else if (funcName.compare("normalization") == 0)
		demo_Normalization(img);
	else if (funcName.compare("gammacorrection") == 0)
		demo_GammaCorrection(img);
	else if (funcName.compare("histogram") == 0)
		demo_Histograms(img);
	else if (funcName.compare("binarization") == 0)
		demo_Binarization(img);
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