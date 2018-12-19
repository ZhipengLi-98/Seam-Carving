#pragma once
#include <opencv2\opencv.hpp>

#include "common.h"
#include "seamCarving.h"


void quicksort(int a[], std::vector<std::vector<int>> seams, int l, int r)
{
	if (l < r)
	{
		int i = l, j = r;
		std::vector<int> temp = seams[l];
		int tempv = seams[l][0];
		int tempi = a[l];
		while (i < j)
		{
			while (i < j && seams[j][0] >= tempv)
				j--;
			if (i < j)
			{
				a[i] = a[j];
				seams[i] = seams[j];
				i++;
			}
			while (i < j && seams[i][0] < tempv)
				i++;
			if (i < j)
			{
				a[j] = a[i];
				seams[j] = seams[i];
				j--;
			}
		}
		a[i] = tempi;
		seams[i] = temp;
		quicksort(a, seams, l, i - 1);
		quicksort(a, seams, i + 1, r);
	}
}

cv::Mat enlargeCarving(const cv::Mat src, const std::vector<std::vector<int>> &seams)
{
	cv::Mat ans = cv::Mat(src.size[0], src.size[1] + seams.size(), src.type());

	for (int i = 0; i < seams.size(); i++)
		id[i] = i;
	quicksort(id, seams, 0, seams.size() - 1);

	std::vector<int> last = std::vector<int>(src.size[0], 0);
	std::vector<int> current = seams[id[0]];
	for (int i = 0; i < seams.size(); i++)
	{
		current = seams[id[i]];
		for (int j = 0; j < src.size[0]; j++)
		{
			memcpy(ans.data + ans.step[0] * j + ans.step[1] * (last[j] + i),
				src.data + src.step[0] * j + src.step[1] * last[j],
				src.step[1] * (current[j] - last[j]));
			if (last[j] != 0)
				ans.at<cv::Vec3b>(j, last[j] + i - 1) = ((src.at<cv::Vec3b>(j, last[j]) + src.at<cv::Vec3b>(j, last[j] - 1)) / 2);
		}
		last = current;
	}

	for (int j = 0; j < src.size[0]; j++)
	{
		memcpy(ans.data + ans.step[0] * j + ans.step[1] * (last[j] + seams.size()),
			src.data + src.step[0] * j + src.step[1] * last[j],
			src.step[1] * (src.size[1] - last[j]));
		if (last[j] != 0)
			ans.at<cv::Vec3b>(j, last[j] + seams.size() - 1) = ((src.at<cv::Vec3b>(j, last[j]) + src.at<cv::Vec3b>(j, last[j] - 1)) / 2);
	}

	return ans;
}

cv::Mat enlarge(const cv::Mat &src, int x, int y, function f)
{
	auto image = src.clone();


	auto mat = f(image);
	auto seams = searchMinSeam(mat, (y - src.size[1]));
	image = enlargeCarving(image, seams);

	image = transpose(image);

	mat = f(image);
	seams = searchMinSeam(mat, (x - src.size[1]));
	image = enlargeCarving(image, seams);

	image = transpose(image);

	return image;
}
