#pragma once
#include <opencv2/opencv.hpp>

#include "param.h"
#include "common.h"

int searchMin(int *a, int l);
int* searchMinPtr(int *a, int l);

std::vector<std::vector<int>> searchMinSeam(const cv::Mat1i &_mat, int k)
{
	cv::Mat1i mat = _mat.clone();
	int m = mat.size[0];
	int n = mat.size[1];

	std::vector<std::vector<int>> seams;
	while (k--)
	{
		for (int i = 0; i < n; i++)
			dp[0][i] = mat.at<int>(0, i);
		for (int i = 1; i < m; i++)
			for (int j = 0; j < n; j++)
				dp[i][j] = searchMin(&dp[i - 1][std::max(0, j - 1)], std::min(n, j + 2) - std::max(0, j - 1)) + mat.at<int>(i, j);
		
		std::vector<int> cols(m);
		cols[m - 1] = searchMinPtr(dp[m - 1], n) - dp[m - 1];
		if (dp[m - 1][cols[m - 1]] >= INF)
			break;
		for (int i = m - 2; i >= 0; i--)
			cols[i] = searchMinPtr(&dp[i][std::max(0, cols[i + 1] - 1)], std::min(n, cols[i + 1] + 2) - std::max(0, cols[i + 1] - 1)) - dp[i];
		for (int i = 0; i < m; i++)
		{
			mat.at<int>(i, cols[i]) = INF;
			mat.at<int>(i, std::max(0, cols[i] - 1)) = INF;
			mat.at<int>(i, std::min(n - 1, cols[i] + 1)) = INF;
		}
		seams.push_back(cols);
	}
	return seams;
}

std::vector<int> searchMinSeam(cv::Mat1i &mat)
{
	int m = mat.size[0];
	int n = mat.size[1];

	for (int i = 0; i < n; i++)
		dp[0][i] = mat.at<int>(0, i);
	for (int i = 1; i < m; i++)
		for (int j = 0; j < n; j++)
			dp[i][j] = searchMin(&dp[i - 1][std::max(0, j - 1)], std::min(n, j + 2) - std::max(0, j - 1)) + mat.at<int>(i, j);

	std::vector<int> cols(m);
	cols[m - 1] = searchMinPtr(dp[m - 1], n) - dp[m - 1];

	for (int i = m - 2; i >= 0; i--)
		cols[i] = searchMinPtr(&dp[i][std::max(0, cols[i + 1] - 1)], std::min(n, cols[i + 1] + 2) - std::max(0, cols[i + 1] - 1)) - dp[i];
	for (int i = 0; i < m; i++)
	{
		mat.at<int>(i, cols[i]) = INF;
		mat.at<int>(i, std::max(0, cols[i] - 1)) = INF;
		mat.at<int>(i, std::min(n - 1, cols[i] + 1)) = INF;
	}

	return cols;
}