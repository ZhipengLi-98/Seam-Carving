#include <iostream>
#include <vector>
#include <string>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "param.h"
#include "common.h"
#include "shrink.h"
#include "enlarge.h"
#include "gui.h"

int main ()
{
	std::string filename;
	std::string filePath;
	std::cout << "Please input filepath" << std::endl;
	std::cin >> filePath;
	filename = filePath;
    auto image = cv::imread(filePath);
	std::string func;
	std::cout << "Please input function" << std::endl;
	std::cin >> func;
	auto f = getFunc(func);
	bool remove = 0;
	std::cout << "Please input remove or not" << std::endl;
	std::cin >> remove;
	cv::Mat mask;
	if(!remove)
		mask = cv::Mat::zeros(image.size[0], image.size[1], CV_8U);
	else
	{
		cv::Mat c;
		image.copyTo(c);
		ObjectRemove(c, f);
		auto tmask = cv::imread("remove.png");
		mask = cv::Mat::zeros(tmask.size[0], tmask.size[1], CV_8U);
		for (int i = 0; i < tmask.size[0]; i++)
			for (int j = 0; j < tmask.size[1]; j++)
			{
				if (tmask.at<cv::Vec3b>(i, j)[1] > 128)
					mask.at<uchar>(i, j) = 1;
				else if (tmask.at<cv::Vec3b>(i, j)[2] > 128)
					mask.at<uchar>(i, j) = 2;
			}
	}

	auto energy = cv::Mat(applyMask(f(image), mask));
	std::cout << "energy finish" << std::endl;
    cv::imwrite(filename + "_" + func + "_energy.png", energy);

	int x = 0;
	int y = 0;
	std::cout << image.size[0] << std::endl;
	std::cout << image.size[1] << std::endl;
	std::cout << image.size[0] * 0.75 << std::endl;
	std::cout << image.size[1] * 0.75 << std::endl;
	std::cin >> x >> y;
	
	if (x >= image.size[0] && y >= image.size[1])
	{
		filename += "_enlarge";
		auto ti = image.clone();
		image = enlarge(image, x, y, f);
		auto t = image.clone();
		auto seam = print(t, x - ti.size[0], y - ti.size[1], f);
		std::cout << "seams finish" << std::endl;
		cv::imwrite(filename + "_" + func + "_seams.png", seam);
	}
	else
	{
		filename += "_shrink";
		auto ti = image.clone();
		auto seam = print(ti, mask, image.size[0] - x, image.size[1] - y, f);
		std::cout << "seams finish" << std::endl;
		cv::imwrite(filename + "_" + func + "_seams.png", seam);
		image = shrink(image, x, y, mask, f);
	}

    cv::imwrite(filename + "_" + func + "_seamCarving.png", image);
	std::cout << "finish" << std::endl;

	return 0;
}