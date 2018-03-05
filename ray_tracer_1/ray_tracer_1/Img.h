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

	void filterPixel(int i, int j, Mat & A, Mat & B);
	void filter();

	float getWeight(float patchDist) { return exp(-1.0f * max(0.0f, patchDist)); };
	
	float getDistPatch(int pi, int pj, int qi, int qj);
	Vec3f getDistPix(int pi, int pj, int qi, int qj);
	Vec3f getModDistPix(int pi, int pj, int qi, int qj, float vp, float vq);

	void setConstants(int r_, int f_) { r = r_; f = f_; };
	void setConstants2(float a_, float e_, float k_) { a = a_; e = e_; k = k_; };

private:
	Mat I;
	int r, f;
	float a, e, k;
	float** W;
};