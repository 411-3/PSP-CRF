#include "stdio.h"
#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/legacy/compat.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace cv;
using namespace std;

void elbp(Mat& src, Mat &dst, int radius, int neighbors);

void elbp1(Mat& src, Mat &dst);
