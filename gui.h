#pragma once
#include <opencv2/opencv.hpp>

#include "common.h"

struct MouseArgs
{
	cv::Mat img;
	cv::Mat mask;
	cv::Mat m;
	cv::Vec3b color;

	MouseArgs(cv::Mat img, cv::Mat mask, cv::Mat m, cv::Vec3b color) 
		: img(img), mask(mask), m(m), color(color) 
	{
		
	}
};

void OnMouse(int event, int x, int y, int flags, void *param)
{
	MouseArgs *args = (MouseArgs *)param;
	if ((event == CV_EVENT_MOUSEMOVE || event == CV_EVENT_LBUTTONDOWN) && (flags & CV_EVENT_FLAG_LBUTTON))
	{
		int brushRadius = 10;    
		int rows = args->img.rows;
		int cols = args->img.cols;

		for (int i = std::max(0, y - brushRadius); i < std::min(rows, y + brushRadius); i++)
		{
			int halfChord = sqrt(pow(brushRadius, 2) - pow(i - y, 2));
			for (int j = std::max(0, x - halfChord); j < std::min(cols, x + halfChord); j++)
				if (args->mask.at<char>(i, j) == 0)
				{
					args->img.at<cv::Vec3b>(i, j) = args->img.at<cv::Vec3b>(i, j)*0.7 + args->color*0.3;
					args->m.at<cv::Vec3b>(i, j) = args->color;
					args->mask.at<char>(i, j) = 1;
				}
		}
	}
}

void ObjectRemove(const cv::Mat image, function f)
{
	cv::Mat showImg = image.clone();
	cv::Mat remove = showImg.clone();
	for(int i = 0; i < image.rows; i++)
		for (int j = 0; j < image.cols; j++)
			remove.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 0);

	cv::Mat removeMask(image.rows, image.cols, CV_8U, cv::Scalar(0, 0, 0));
	cv::Mat remainMask(image.rows, image.cols, CV_8U, cv::Scalar(0, 0, 0));

	cv::Vec3b red(0, 0, 255);
	cv::Vec3b green(0, 255, 0);

	MouseArgs *args0 = new MouseArgs(showImg, removeMask, remove, green);
	MouseArgs *args1 = new MouseArgs(showImg, removeMask, remove, red);
	MouseArgs *args2 = new MouseArgs(showImg, remainMask, remove, green);

	cv::setMouseCallback("Remove & Remain", OnMouse, (void*)args0);
	while (1)
	{
		cv::imshow("Remove & Remain", args0->img);
		if (cv::waitKey(100) == 27)
			break;
	}

	cv::setMouseCallback("Remove & Remain", OnMouse, (void*)args1);
	while (1)
	{
		cv::imshow("Remove & Remain", args1->img);
		if (cv::waitKey(100) == 27)
			break;
	}

	cv::setMouseCallback("Remove & Remain", OnMouse, (void*)args2);
	while (1)
	{
		cv::imshow("Remove & Remain", args2->img);
		if (cv::waitKey(100) == 27)
			break;
	}

	cv::setMouseCallback("Remove & Remain", NULL, NULL);
	cv::imwrite("remove.png", remove);
	cv::imshow("remove", remove);
	cv::waitKey(1);
	delete args1; args1 = NULL;
	delete args2; args2 = NULL;
}