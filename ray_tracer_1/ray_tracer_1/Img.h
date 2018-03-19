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
	Img(int h, int w);

	// Image manipulation
	void displayImg(Mat & M, bool isNorm, bool isScale);
	void saveImg(string filename, Mat & M, bool isColormap);

	// Information from path tracer
	void setBudget(int b) 
	{ 
		budget = b; 
		P_ = Mat(height, width, CV_8UC1, Scalar(budget));
	};
	void sendToA(int i, int j, glm::vec3 variance, glm::vec3 color);
	void sendToB(int i, int j, glm::vec3 variance, glm::vec3 color);
	void sendToV(int i, int j, glm::vec3 variance);

	// Send information to path tracer
	int getSampleNum(int i, int j) { return (int)P_.at<uchar>(i, j); };

	// Variance computation
	void initVarEst();

	// Post processing
	void computeError(int niter);
	void combineBuffers(Mat & M0, Mat & M1);

	// Filtering
	void Filter(int mode);
	void filter(Mat& M0, Mat& M1, int mode);
	float filterPixel(int i, int j, Mat& M0, Mat& M1, int mode);
	
	float getDistPatch(int pi, int pj, int qi, int qj, Mat & M, int mode);
	Vec3f getDistPix(int pi, int pj, int qi, int qj, Mat & M);
	Vec3f getModDistPix(int pi, int pj, int qi, int qj, Mat & M, int mode);

	float getWeight(float patchDist) { return exp(-1.0f * max(0.0f, patchDist)); };

	// Constants
	void setConstants(int r_, int f_) 
	{
		r = r_; f = f_; normc = float(3 * pow(2 * f + 1, 2));
	};
	void setConstants2(float a_, float e_, float k_) { a = a_; e = e_; k = k_; };

	void printResult();

//private:

	// Dual image buffers
	Mat A, B, V;
	
	// Empirical variance
	Mat V_e;
	Mat V_a, V_b, V_d;

	// Cross-filtered result
	Mat F_a, F_b, F_v;

	// Combined
	Mat C;

	// Error map
	Mat E_map;

	// Constants
	int r, f;
	float a, e, k;

	// Weight
	Mat W_a, W_b;

	// Number of samples per pixel
	Mat P, P_;
	int budget;

	int width, height;

	float normc;
};