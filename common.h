#pragma once
#include <opencv2\opencv.hpp>
#include <iostream>

#include "seamCarving.h"

cv::Mat transpose(const cv::Mat &src)
{
	cv::Mat dst;
	cv::transpose(src, dst);
	return dst;
}

cv::Mat mine(const cv::Mat &image)
{
	cv::Mat gray_image;
	cv::cvtColor(image, gray_image, CV_BGR2GRAY);
	cv::Mat x;
	cv::Mat y;

	cv::Mat  matx= cv::Mat::zeros(3, 3, CV_32F);
	matx.at<float>(1, 0) = -1;
	matx.at<float>(1, 2) = 1;

	cv::Mat maty = cv::Mat::zeros(3, 3, CV_32F);
	maty.at<float>(0, 1) = -1;
	maty.at<float>(2, 1) = 1;

	cv::filter2D(gray_image, x, CV_16S, matx);
	cv::filter2D(gray_image, y, CV_16S, maty);

	cv::Mat img;
	cv::addWeighted(abs(x), 0.5, abs(y), 0.5, 0.0, img);
	return img;
}

cv::Mat sobel(const cv::Mat &image)
{
	cv::Mat gray_image;
	cv::cvtColor(image, gray_image, CV_BGR2GRAY);
	cv::Mat x;
	cv::Mat y;
	cv::Mat absx;
	cv::Mat absy;
	cv::Sobel(gray_image, x, CV_16S, 1, 0, 3);
	cv::Sobel(gray_image, y, CV_16S, 0, 1, 3);
	cv::convertScaleAbs(x, absx);
	cv::convertScaleAbs(y, absy);
	
	cv::Mat img;
	cv::addWeighted(absx, 0.5, absy, 0.5, 0.0, img);
	return img;
}

cv::Mat scharr(const cv::Mat &image)
{
	cv::Mat gray_image;
	cv::cvtColor(image, gray_image, CV_BGR2GRAY);
	cv::Mat x;
	cv::Mat y;
	cv::Mat absx;
	cv::Mat absy;
	cv::Sobel(gray_image, x, CV_16S, 1, 0, CV_SCHARR);
	cv::Sobel(gray_image, y, CV_16S, 0, 1, CV_SCHARR);
	cv::convertScaleAbs(x, absx);
	cv::convertScaleAbs(y, absy);

	cv::Mat img;
	cv::addWeighted(absx, 0.5, absy, 0.5, 0.0, img);
	return img;
}

cv::Mat laplace(const cv::Mat &image)
{
	cv::Mat gray_image;
	cv::cvtColor(image, gray_image, CV_BGR2GRAY);
	cv::Mat gradient;
	cv::Mat img;
	cv::Laplacian(gray_image, gradient, CV_16S, 3);
	cv::convertScaleAbs(gradient, img);

	return img;
}

function getFunc(std::string& name)
{
	if (name == "sobel")
		return &sobel;
	else if (name == "scharr")
		return &scharr;
	else if (name == "laplace")
		return &laplace;
	else if (name == "mine")
		return &mine;
}

cv::Mat1i applyMask(const cv::Mat1i &energy, const cv::Mat1b &mask)
{
	cv::Mat1i rst = cv::Mat::zeros(mask.size[0], mask.size[1], CV_8U);
	rst.forEach([&](int& pixel, const int *pos)
	{
		if (mask.at<uchar>(pos) == 1)
			pixel = INF;
		else if (mask.at<uchar>(pos) == 2)
			pixel = -INF;
		else
			pixel = energy.at<int>(pos);
	});
	return rst;
}

int searchMin(int *a, int l)
{
	int temp = INF;
	for (int i = 0; i < l; i++)
		if (a[i] < temp)
			temp = a[i];
	return temp;
}

int* searchMinPtr(int *a, int l)
{
	int temp = INF;
	int *ans = a;
	for (int i = 0; i < l; i++)
		if (a[i] < temp)
		{
			ans = &a[i];
			temp = a[i];
		}
	return ans;
}

cv::Mat print(cv::Mat src, const std::vector<std::vector<int>> &seams, const cv::Mat &mask, bool trans)
{
	for (int k = 0; k < seams.size(); ++k)
		for (int i = 0; i < seams[k].size(); ++i)
			src.at<cv::Vec3b>(i, seams[k][i]) = cv::Vec3b(0, 0, (uchar)(255 - k * 200 / seams.size()));

	src.forEach<cv::Vec3b>([&](cv::Vec3b& pixel, const int *pos)
	{
		if (mask.at<uchar>(pos) == 1)
			pixel = pixel * 0.5 + cv::Vec3b(0, 255, 0) * 0.5;
		else if (mask.at<uchar>(pos) == 2)
			pixel = pixel * 0.5 + cv::Vec3b(0, 0, 255) * 0.5;
	});
	if (trans)
		src = transpose(src);
	return src;
}


cv::Mat print(cv::Mat src, const std::vector<std::vector<int>> &seams, bool trans)
{
	for (int k = 0; k < seams.size(); ++k)
		for (int i = 0; i < seams[k].size(); ++i)
			src.at<cv::Vec3b>(i, seams[k][i]) = cv::Vec3b(0, 0, (uchar)(255 - k * 200 / seams.size()));

	if (trans)
		src = transpose(src);
	return src;
}

cv::Mat print(const cv::Mat &image, cv::Mat mask, int row, int col, function f)
{
	auto mat = f(image);
	auto imat = applyMask(mat, mask);
	auto seams1 = searchMinSeam(imat, col);
	imat = transpose(imat);
	auto seams2 = searchMinSeam(imat, row);
	auto show = print(image, seams1, mask, false);
	show = print(transpose(show), seams2, true);
	return show;
}

cv::Mat print(const cv::Mat &image, int row, int col, function f)
{
	auto imat = f(image);
	auto seams1 = searchMinSeam(imat, col);
	imat = transpose(imat);
	auto seams2 = searchMinSeam(imat, row);
	auto show = print(image, seams1, false);
	show = print(transpose(show), seams2, true);
	return show;
}
