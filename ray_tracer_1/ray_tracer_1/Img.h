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
	void empVar(int h, int w);
	void setEmpV(int i, int j, glm::vec3 value);

	Mat& getEmpV() { return V_e; };
	void setVbuffer(Mat & M, int mode);
	void setDiffVbuffer() {absdiff(V_a, V_b, V_d);};

	void combineBuffers(Mat & M0, Mat & M1);

	void Filter(int mode);
	void filter(Mat& M0, Mat& M1, int mode);
	void filterPixel(int i, int j, Mat& M0, Mat& M1, int mode);
	
	float getDistPatch(int pi, int pj, int qi, int qj, Mat & M, int mode);
	Vec3f getDistPix(int pi, int pj, int qi, int qj, Mat & M);
	Vec3f getModDistPix(int pi, int pj, int qi, int qj, Mat & M);
	Vec3f getModDistPix2(int pi, int pj, int qi, int qj, Mat & M);

	float getWeight(float patchDist) { return exp(-1.0f * max(0.0f, patchDist)); };
	void setConstants(int r_, int f_) { r = r_; f = f_; };
	void setConstants2(float a_, float e_, float k_) { a = a_; e = e_; k = k_; };

private:

	// Dual image buffers
	Mat A, B;

	// Sqrd Diff between A and B
	Mat V;
	
	// Empirical variance
	Mat V_e;
	Mat V_a, V_b;
	Mat V_d;

	// Cross-filtered result
	Mat F_a, F_b, F_v;

	// Combined
	Mat C;

	// Constants
	int r, f;
	float a, e, k;

	// Weight
	float** W;

	int width, height;
};