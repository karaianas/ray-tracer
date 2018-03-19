#include "Img.h"

#define sfactor 0.00392156862f

using namespace cv;
using namespace std;

Img::Img(int h, int w)
{
	height = h;
	width = w;

	V_e = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	V_a = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	V_b = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));

	A = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));
	B = Mat(height, width, CV_8UC3, Scalar(0, 0, 0));

	P = Mat(height, width, CV_8UC1, Scalar(0));
}

void Img::displayImg(Mat & M, bool isNorm, bool isScale)
{
	Mat temp = M.clone();
	if(isNorm)
		normalize(M, temp, 0, 1, NORM_MINMAX);
	if (isScale)
		temp *= 255.0f;
	namedWindow("Display", CV_WINDOW_AUTOSIZE);
	imshow("Display", M);

	waitKey(0);
}

void Img::saveImg(string filename, Mat & M, bool isColormap)
{
	string filepath = "D://Github//temp//" + filename;
	if (isColormap)
	{
		Mat temp; 
		//normalize(M, temp, 0, 1, NORM_MINMAX);
		M.convertTo(temp, CV_8UC1, 255.0f);
		applyColorMap(temp, temp, COLORMAP_JET);
		imwrite(filepath, temp);
	}
	else
		imwrite(filepath, M);
}

void Img::sendToA(int i, int j, glm::vec3 variance, glm::vec3 color)
{
	Vec3f newValue = Vec3f(A.at<Vec3b>(i, j)) * float(P.at<uchar>(i, j)) * 0.5f \
		+ Vec3f(color[2], color[1], color[0])* 255.0f * float(P_.at<uchar>(i, j)) * 0.5f;
	newValue /= float(P.at<uchar>(i, j)) * 0.5f + float(P_.at<uchar>(i, j)) * 0.5f;
	
	A.at<Vec3b>(i, j) = Vec3b(newValue);
	V_a.at<Vec3b>(i, j) = Vec3b(Vec3f(variance[2], variance[1], variance[0]) * 255.0f);
}

void Img::sendToB(int i, int j, glm::vec3 variance, glm::vec3 color)
{
	Vec3f newValue = Vec3f(B.at<Vec3b>(i, j)) * float(P.at<uchar>(i, j)) * 0.5f \
		+ Vec3f(color[2], color[1], color[0])* 255.0f * float(P_.at<uchar>(i, j)) * 0.5f;
	newValue /= float(P.at<uchar>(i, j)) * 0.5f + float(P_.at<uchar>(i, j)) * 0.5f;
	
	B.at<Vec3b>(i, j) = Vec3b(newValue);
	V_b.at<Vec3b>(i, j) = Vec3b(Vec3f(variance[2], variance[1], variance[0]) * 255.0f);
}

void Img::sendToV(int i, int j, glm::vec3 variance)
{
	V_e.at<Vec3b>(i, j) = Vec3b(Vec3f(variance[2], variance[1], variance[0]) * 255.0f);
}

void Img::initVarEst()
{
	absdiff(A, B, V);

	#pragma omp parallel
	for (int i = 0; i < height; i++)
	{
		#pragma omp for schedule(dynamic,1)
		for (int j = 0; j < width; j++)
		{
			uchar b = V.at<Vec3b>(i, j).val[0];
			uchar g = V.at<Vec3b>(i, j).val[1];
			uchar r = V.at<Vec3b>(i, j).val[2];

			float bf = float(b) * sfactor;
			float gf = float(g) * sfactor;
			float rf = float(r) * sfactor;

			bf = bf * bf * 0.5f;
			gf = gf * gf * 0.5f;
			rf = rf * rf * 0.5f;

			V.at<Vec3b>(i, j).val[0] = uchar(round(bf * 255.0f));
			V.at<Vec3b>(i, j).val[1] = uchar(round(gf * 255.0f));
			V.at<Vec3b>(i, j).val[2] = uchar(round(rf * 255.0f));
		}
	}
}

void Img::computeError(int niter)
{
	Mat dE;
	absdiff(A, B, dE);

	Mat E_a(height, width, CV_32FC1, 0.0);
	Mat E_b(height, width, CV_32FC1, 0.0);

	// Update spp count
	add(P, P_, P);

	#pragma omp parallel
	for (int i = 0; i < height; i++)
	{
		#pragma omp for schedule(dynamic,1)
		for (int j = 0; j < width; j++)
		{
			float b = float(dE.at<Vec3b>(i, j).val[0]) * sfactor;
			float g = float(dE.at<Vec3b>(i, j).val[1]) * sfactor;
			float r = float(dE.at<Vec3b>(i, j).val[2]) * sfactor;

			float ba = float(A.at<Vec3b>(i, j).val[0]) * sfactor;
			float ga = float(A.at<Vec3b>(i, j).val[1]) * sfactor;
			float ra = float(A.at<Vec3b>(i, j).val[2]) * sfactor;

			float bb = float(B.at<Vec3b>(i, j).val[0]) * sfactor;
			float gb = float(B.at<Vec3b>(i, j).val[1]) * sfactor;
			float rb = float(B.at<Vec3b>(i, j).val[2]) * sfactor;

			float error = b*b + g*g + r*r;

			// Error map for A
			float e_a = ba*ba + ga*ga + ra*ra;
			if (e_a < 0.000001f)
				e_a = error;
			else
				e_a = error / e_a;

			E_a.at<float>(i, j) = e_a * W_a.at<float>(i, j) /( 1 + (float)P.at<uchar>(i, j));

			// Error map for B
			float e_b = bb*bb + gb*gb + rb*rb;
			if (e_b < 0.000001f)
				e_b = error;
			else
				e_b = error / e_b;
			E_b.at<float>(i, j) = e_b * W_b.at<float>(i, j) /(1 + (float)P.at<uchar>(i, j));
		}
	}

	combineBuffers(F_a, F_b);

	// Sum of weighted error maps
	add(E_a, E_b, E_map);
	GaussianBlur(E_map, E_map, Size(3, 3), 0.8);

	// Comment this part out for proper time measurement
	string fname = "ErrorMap";
	fname += "_";
	fname += to_string(niter);
	fname += ".png";
	saveImg((char*)fname.c_str(), E_map, 1);

	// Normalize
	float s = sum(E_map)[0];
	E_map = float(budget * height * width) / s * E_map;//(2.0f * s) * result;

	// Clamp the values
	int threshold = 4;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (E_map.at<float>(i, j) > threshold)
				E_map.at<float>(i, j) = threshold;

			int extraRayNum = int(E_map.at<float>(i, j));

			if (extraRayNum % 2 != 0)
				P_.at<uchar>(i, j) = uchar(extraRayNum + 1);
			else
				P_.at<uchar>(i, j) = uchar(extraRayNum);
		}
	}

	/*
	int maxRayNum = 0;
	int maxERayNum = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int val = (int)P.at<uchar>(i, j);
			int eval = (int)P_.at<uchar>(i, j);
			if (val > maxRayNum)
				maxRayNum = val;
			if (eval > maxERayNum)
				maxERayNum = eval;
		}
	}

	cout << "Maximum nspp so far is: " << maxRayNum << endl;
	cout << "Maximum to be added nspp is: " << maxERayNum << endl;

	int totalNumSamples = 0;
	for(int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
			totalNumSamples += int(P.at<uchar>(i, j));

	cout << "Total number of samples drawn so far: " << totalNumSamples << endl;

	int numdraw = 0;
	for (int i = 0; i < height; i++)
		for (int j = 0; j < width; j++)
		{
			if (int(P_.at<uchar>(i, j)) > 0)
				numdraw++;
		}

	cout << "Total number of pixels with more than 0 rays at next iter: " << numdraw << endl;
	*/
}

void Img::combineBuffers(Mat & M0, Mat & M1)
{
	addWeighted(M0, 0.5, M1, 0.5, 0.0, C);
}

void Img::Filter(int mode)
{
	clock_t t;
	t = clock();

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

	t = clock() - t;
	float seconds = ((double)t) / CLOCKS_PER_SEC;
	printf("Filtering Time: %2.2f seconds\n", seconds);
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
	//#pragma omp parallel
	for (int offset = -r; offset <= r; offset++)
	{
		//#pragma omp for schedule(dynamic,1)
		for (int offset1 = -r; offset1 <= r; offset1++)
		{
			float D = getDistPatch(i, j, i + offset, j + offset1, M1, mode);
			float w = getWeight(D);
			if (w < 0.05f)
				w = 0.0f;
			else
			{
				totalW += w;
				Vec3b q = M1.at<Vec3b>(i + offset, j + offset1);
				sum[0] += w * float(q.val[0]) * sfactor;
				sum[1] += w * float(q.val[1]) * sfactor;
				sum[2] += w * float(q.val[2]) * sfactor;
			}
		}
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

	for (int di = -f; di <= f; di++)
		for (int dj = -f; dj <= f; dj++)
			dist += getModDistPix(pi + di, pj + dj, qi + di, qj + dj, M, mode);

	float D = dist[0] + dist[1] + dist[2];
	D /= normc;// float(3 * pow(2 * f + 1, 2));

	return D;
}

Vec3f Img::getDistPix(int pi, int pj, int qi, int qj, Mat & M)
{
	uchar & pb = M.at<Vec3b>(pi, pj).val[0];
	uchar & qb = M.at<Vec3b>(qi, qj).val[0];
	float diffb = (float(pb) - float(qb)) * sfactor;

	uchar & pg = M.at<Vec3b>(pi, pj).val[1];
	uchar & qg = M.at<Vec3b>(qi, qj).val[1];
	float diffg = (float(pg) - float(qg)) * sfactor;

	uchar & pr = M.at<Vec3b>(pi, pj).val[2];
	uchar & qr = M.at<Vec3b>(qi, qj).val[2];
	float diffr = (float(pr) - float(qr)) * sfactor;

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
	float threshold = 0.0000000001f;//0.0001f;

	glm::vec3 pf(float(pvar.val[0]), float(pvar.val[1]), float(pvar.val[2]));
	glm::vec3 qf(float(qvar.val[0]), float(qvar.val[1]), float(qvar.val[2]));
	pf *= sfactor;
	qf *= sfactor;

	glm::vec3 div = k * (pf + qf);
	glm::vec3 pcolor((float)M.at<Vec3b>(pi, pj).val[0], (float)M.at<Vec3b>(pi, pj).val[1], (float)M.at<Vec3b>(pi, pj).val[2]);
	glm::vec3 qcolor((float)M.at<Vec3b>(qi, qj).val[0], (float)M.at<Vec3b>(qi, qj).val[1], (float)M.at<Vec3b>(qi, qj).val[2]);
	glm::vec3 diff = (pcolor - qcolor) * sfactor;
	glm::vec3 powered = diff * diff;

	float diffb = powered.x;
	if (div.x < threshold)
		diffb *= 0.0000000001f;
	else
		//diffb = diffb / div.x - a / k;
		diffb = (diffb - a * (pf.x + min(pf.x, qf.x))) / (div.x);

	float diffg = powered.y;
	if (div.y < threshold)
		diffg *= 0.0000000001f;
	else
		//diffg = diffg / div.y - a / k;
		diffg = (diffg - a * (pf.y + min(pf.y, qf.y))) / (div.y);

	float diffr = powered.z;
	if (div.z < threshold)
		diffr *= 0.0000000001f;
	else
		//diffr = diffb / div.z - a / k;
		diffr = (diffr - a * (pf.z + min(pf.z, qf.z))) / (div.z);

	return Vec3f(diffb, diffg, diffr);
}

void Img::printResult()
{
	displayImg(C, 0, 0);
	saveImg("FinalResult.png", C, 0);
}

