#pragma once
#include <opencv2\opencv.hpp>

#include "common.h"
#include "seamCarving.h"

cv::Mat shrinkCarving(const cv::Mat src, const std::vector<int> &seams)
{
	cv::Mat ans = cv::Mat(src.size[0], src.size[1] - 1, src.type());

	for (int i = 0; i < src.size[0]; i++)
		memcpy(ans.data + ans.step[0] * i, 
			src.data + src.step[0] * i, 
			src.step[1] * seams[i]);

	for (int i = 0; i < src.size[0]; i++)
		memcpy(ans.data + ans.step[0] * i + ans.step[1] * seams[i],
			src.data + src.step[0] * i + src.step[1] * (seams[i] + 1),
			src.step[1] * (src.size[1] - seams[i] - 1));

	return ans;
}

cv::Mat shrink(const cv::Mat &src, int x, int y, cv::Mat &mask, function f)
{
	auto image = src.clone();
	int col = src.size[1] - y > 0 ? src.size[1] - y : 0;
	int row = src.size[0] - x > 0 ? src.size[0] - x : 0;

	while (col--)
	{
		auto tmat = applyMask(f(image), mask);
		auto seams = searchMinSeam(tmat);
		mask = shrinkCarving(mask, seams);
		image = shrinkCarving(image, seams);
	}

	image = transpose(image);
	mask = transpose(mask);

	while (row--)
	{
		auto tmat = applyMask(f(image), mask);
		auto seams = searchMinSeam(tmat);
		mask = shrinkCarving(mask, seams);
		image = shrinkCarving(image, seams);
	}

	image = transpose(image);
	mask = transpose(mask);

	return image;
}