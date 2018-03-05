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
	void showImg(char c);

	void setBuffers(const char* filepathA, const char* filepathB);
	void initVarEst();

	void Filter(int mode);
	void filter(Mat& M0, Mat& M1);
	void filterPixel(int i, int j, Mat& M0, Mat& M1);
	
	float getDistPatch(int pi, int pj, int qi, int qj, Mat & M);
	Vec3f getDistPix(int pi, int pj, int qi, int qj, Mat & M);
	Vec3f getModDistPix(int pi, int pj, int qi, int qj, float vp, float vq);

	float getWeight(float patchDist) { return exp(-1.0f * max(0.0f, patchDist)); };
	void setConstants(int r_, int f_) { r = r_; f = f_; };
	void setConstants2(float a_, float e_, float k_) { a = a_; e = e_; k = k_; };

private:

	// Dual buffers
	Mat A, B;

	// Variance
	Mat V;

	// Filtered
	Mat F;

	// Constants
	int r, f;
	float a, e, k;

	// Weight
	float** W;

	int width, height;
};