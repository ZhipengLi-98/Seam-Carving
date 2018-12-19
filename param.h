#pragma once

#define INF 1<<20

#define N 10005

int dp[N][N];
int id[N];

typedef cv::Mat (*function)(const cv::Mat &img);
