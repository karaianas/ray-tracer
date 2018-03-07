#include "Img.h"

using namespace cv;
using namespace std;

Img::Img(int h, int w)
{
	height = h;
	width = w;

	V_e = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	V_a = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	V_b = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	P = Mat(height, width, CV_32FC1, 4.0);
	A = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	B = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
}

void Img::displayImg(Mat & M, bool isNorm, bool isScale)
{
	Mat temp = M.clone();
	if(isNorm)
		normalize(M, temp, 0, 1, NORM_MINMAX);
	if (isScale)
		temp *= 255.0f;
	namedWindow("Display", CV_WINDOW_AUTOSIZE);
	imshow("Display", temp);

	waitKey(0);
}

void Img::saveImg(string filename, Mat & M)
{
	string filepath = "D://Github//temp//" + filename;
	imwrite(filepath, M);
}

void Img::sendToA(int i, int j, glm::vec3 variance, glm::vec3 color)
{
	A.at<Vec3b>(i, j).val[0] = uchar(color[2] * 255.0f);
	A.at<Vec3b>(i, j).val[1] = uchar(color[1] * 255.0f);
	A.at<Vec3b>(i, j).val[2] = uchar(color[0] * 255.0f);

	V_a.at<Vec3b>(i, j).val[0] = uchar(variance[2] * 255.0f);
	V_a.at<Vec3b>(i, j).val[1] = uchar(variance[1] * 255.0f);
	V_a.at<Vec3b>(i, j).val[2] = uchar(variance[0] * 255.0f);
}

void Img::sendToB(int i, int j, glm::vec3 variance, glm::vec3 color)
{
	B.at<Vec3b>(i, j).val[0] = uchar(color[2] * 255.0f);
	B.at<Vec3b>(i, j).val[1] = uchar(color[1] * 255.0f);
	B.at<Vec3b>(i, j).val[2] = uchar(color[0] * 255.0f);

	V_b.at<Vec3b>(i, j).val[0] = uchar(variance[2] * 255.0f);
	V_b.at<Vec3b>(i, j).val[1] = uchar(variance[1] * 255.0f);
	V_b.at<Vec3b>(i, j).val[2] = uchar(variance[0] * 255.0f);
}

void Img::sendToV(int i, int j, glm::vec3 variance)
{
	V_e.at<Vec3b>(i, j).val[0] = uchar(variance[2] * 255.0f);
	V_e.at<Vec3b>(i, j).val[1] = uchar(variance[1] * 255.0f);
	V_e.at<Vec3b>(i, j).val[2] = uchar(variance[0] * 255.0f);
}

void Img::initVarEst()
{
	absdiff(A, B, V);
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			uchar b = V.at<Vec3b>(i, j).val[0];
			uchar g = V.at<Vec3b>(i, j).val[1];
			uchar r = V.at<Vec3b>(i, j).val[2];

			float bf = float(b) / 255.0f;
			float gf = float(g) / 255.0f;
			float rf = float(r) / 255.0f;

			bf = bf * bf * 0.5f;
			gf = gf * gf * 0.5f;
			rf = rf * rf * 0.5f;

			V.at<Vec3b>(i, j).val[0] = uchar(round(bf * 255.0f));
			V.at<Vec3b>(i, j).val[1] = uchar(round(gf * 255.0f));
			V.at<Vec3b>(i, j).val[2] = uchar(round(rf * 255.0f));
		}
	}

	//pow(V, 2, V);
	//V *= 10.0f;
}

void Img::computeError()
{
	Mat dE;
	absdiff(A, B, dE);

	Mat E_a(height, width, CV_32FC1, 0.0);
	Mat E_b(height, width, CV_32FC1, 0.0);

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			float b = float(dE.at<Vec3b>(i, j).val[0]) / 255.0f;
			float g = float(dE.at<Vec3b>(i, j).val[1]) / 255.0f;
			float r = float(dE.at<Vec3b>(i, j).val[2]) / 255.0f;

			float ba = float(A.at<Vec3b>(i, j).val[0]) / 255.0f;
			float ga = float(A.at<Vec3b>(i, j).val[1]) / 255.0f;
			float ra = float(A.at<Vec3b>(i, j).val[2]) / 255.0f;

			float bb = float(B.at<Vec3b>(i, j).val[0]) / 255.0f;
			float gb = float(B.at<Vec3b>(i, j).val[1]) / 255.0f;
			float rb = float(B.at<Vec3b>(i, j).val[2]) / 255.0f;

			float error = b*b + g*g + r*r;

			// Error map for A
			float e_a = ba*ba + ga*ga + ra*ra;
			if (e_a < 0.000001f)
				e_a = error;
			else
				e_a = error / e_a;

			E_a.at<float>(i, j) = e_a * W_a.at<float>(i, j) /( 1 + P.at<float>(i, j));

			// Error map for B
			float e_b = bb*bb + gb*gb + rb*rb;
			if (e_b < 0.000001f)
				e_b = error;
			else
				e_b = error / e_b;
			E_b.at<float>(i, j) = e_b * W_b.at<float>(i, j) /(1 + P.at<float>(i, j));
		}
	}

	combineBuffers(F_a, F_b);
	displayImg(C, 0, 0);
	saveImg("Combined.png", C);

	// Sum of weighted error maps
	Mat combined;
	add(E_a, E_b, combined);

	Mat result;
	GaussianBlur(combined, result, Size(3, 3), 0.8);
	displayImg(result, 1, 1);
	//imwrite("D://Github//temp//weightedError.png", combined);
}

void Img::combineBuffers(Mat & M0, Mat & M1)
{
	addWeighted(M0, 0.5, M1, 0.5, 0.0, C);
}

void Img::Filter(int mode)
{
	// Image filter
	if (mode == 0)
		filter(A, B, mode);
	// Variance filter
	else
	{
		initVarEst();
		absdiff(V_a, V_b, V_d);
		filter(V, V_e, mode);
	}
}

void Img::filter(Mat& M0, Mat& M1, int mode)
{
	if (mode == 0)
	{
		F_a = M0.clone();
		F_b = M1.clone();
		W_a = Mat(height, width, CV_32FC1, 0.0);
		W_b = Mat(height, width, CV_32FC1, 0.0);
	}
	else
		F_v = M0.clone();

	#pragma omp parallel
	for (int i = r + f; i < height - (r + f); i++)
	{
		#pragma omp for schedule(dynamic,1)
		for (int j = r + f; j < width - (r + f); j++)
		{
			// Filter each pixel
			if (mode == 0)
			{
				float w_a = filterPixel(i, j, F_a, M1, mode);
				float w_b = filterPixel(i, j, F_b, M0, mode);
				W_a.at<float>(i, j) = w_a;
				W_b.at<float>(i, j) = w_b;
			}
			else
				filterPixel(i, j, F_v, M1, mode);

		}
	}
}

float Img::filterPixel(int i, int j, Mat& M0, Mat& M1, int mode)
{
	float totalW = 0.0f;
	Vec3f sum(0.0f);

	// Iterate all neighboring patches in buffer M1
	for (int offset = -r; offset <= r; offset++)
	{
		float D = getDistPatch(i, j, i + offset, j + offset, M1, mode);
		float w = getWeight(D);
		if (w < 0.05f)
			w = 0.0f;
		totalW += w;
		Vec3b q = M1.at<Vec3b>(i + offset, j + offset);
		sum[0] += w * float(q.val[0]) / 255.0f;
		sum[1] += w * float(q.val[1]) / 255.0f;
		sum[2] += w * float(q.val[2]) / 255.0f;
	}

	sum /= totalW;

	// Modify values in buffer M0
	M0.at<Vec3b>(i, j).val[0] = uchar(sum[0] * 255.0f);
	M0.at<Vec3b>(i, j).val[1] = uchar(sum[1] * 255.0f);
	M0.at<Vec3b>(i, j).val[2] = uchar(sum[2] * 255.0f);

	return totalW;
}

float Img::getDistPatch(int pi, int pj, int qi, int qj, Mat & M, int mode)
{
	Vec3f dist(0.0f);

	for (int di = -f; di <= f; di += 1)
		for (int dj = -f; dj <= f; dj += 1)
			dist += getModDistPix(pi + di, pj + dj, qi + di, qj + dj, M, mode);

	float D = dist[0] + dist[1] + dist[2];
	D /= float(3 * pow(2 * f + 1, 2));

	return D;
}

Vec3f Img::getDistPix(int pi, int pj, int qi, int qj, Mat & M)
{
	uchar & pb = M.at<Vec3b>(pi, pj).val[0];
	uchar & qb = M.at<Vec3b>(qi, qj).val[0];
	float diffb = (float(pb) - float(qb)) / 255.0f;

	uchar & pg = M.at<Vec3b>(pi, pj).val[1];
	uchar & qg = M.at<Vec3b>(qi, qj).val[1];
	float diffg = (float(pg) - float(qg)) / 255.0f;

	uchar & pr = M.at<Vec3b>(pi, pj).val[2];
	uchar & qr = M.at<Vec3b>(qi, qj).val[2];
	float diffr = (float(pr) - float(qr)) / 255.0f;

	return Vec3f(diffb * diffb, diffg * diffg, diffr * diffr);
}

Vec3f Img::getModDistPix(int pi, int pj, int qi, int qj, Mat & M, int mode)
{
	Vec3b pvar, qvar;
	if (mode == 0)
	{
		pvar = F_v.at<Vec3b>(pi, pj);
		qvar = F_v.at<Vec3b>(qi, qj);
	}
	else
	{
		pvar = V_d.at<Vec3b>(pi, pj);
		qvar = V_d.at<Vec3b>(qi, qj);
	}

	// Smaller gives sharper but artifacted images(0.01f)
	float threshold = 0.0001f;

	glm::vec3 pf(float(pvar.val[0]), float(pvar.val[1]), float(pvar.val[2]));
	glm::vec3 qf(float(qvar.val[0]), float(qvar.val[1]), float(qvar.val[2]));
	pf /= 255.0f;
	qf /= 255.0f;

	uchar & pb = M.at<Vec3b>(pi, pj).val[0];
	uchar & qb = M.at<Vec3b>(qi, qj).val[0];
	float diffb = (float(pb) - float(qb)) / 255.0f;

	float div = k * k * (pf.x + qf.x);
	if (div < threshold)
		diffb = (diffb*diffb);
	else
		diffb = (diffb*diffb - a * (pf.x + max(pf.x, qf.x))) / (div);

	uchar & pg = M.at<Vec3b>(pi, pj).val[1];
	uchar & qg = M.at<Vec3b>(qi, qj).val[1];
	float diffg = (float(pg) - float(qg)) / 255.0f;

	div = k * k * (pf.y + qf.y);
	if (div < threshold)
		diffg = (diffg*diffg);
	else
		diffg = (diffg*diffg - a * (pf.y + max(pf.y, qf.y))) / (div);

	uchar & pr = M.at<Vec3b>(pi, pj).val[2];
	uchar & qr = M.at<Vec3b>(qi, qj).val[2];
	float diffr = (float(pr) - float(qr)) / 255.0f;

	div = k * k * (pf.z + qf.z);
	if (div < threshold)
		diffr = (diffr*diffr);
	else
		diffr = (diffr*diffr - a * (pf.z + max(pf.z, qf.z))) / (div);

	return Vec3f(diffb, diffg, diffr);
}
