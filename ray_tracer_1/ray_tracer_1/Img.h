#pragma once

#include "Core.h"
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
using namespace std;

class Img {
public:
	Img();
	
	void readImg();
	void showImg();
	void getBoundary();
	

	float getDistPatch(int pi, int pj, int qi, int qj);
	Vec3f getDistPix(int pi, int pj, int qi, int qj);

	void setConstants(int r_, int f_) { r = r_; f = f_; };

private:
	Mat I;
	int r, f;
};