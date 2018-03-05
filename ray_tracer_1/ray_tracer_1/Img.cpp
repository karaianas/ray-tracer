#include "Img.h"

using namespace cv;
using namespace std;

Img::Img()
{
}

void Img::readImg()
{
	//I = imread(".//Images//cat.jpg");

	//W = new float*[I.rows];
	//for (int i = 0; i < I.rows; i++)
	//	W[i] = new float[I.cols];

	//#pragma omp parallel
	//for(int i = 0; i < I.rows; i++)
	//	#pragma omp for schedule(dynamic,1)
	//	for (int j = 0; j < I.cols; j++)
	//	{
	//		W[i][j] = 0.0f;
	//	}
}

void Img::showImg(char c)
{
	// Buffer A
	if (c == 'a')
	{
		namedWindow("A", CV_WINDOW_AUTOSIZE);
		imshow("A", A);
	}
	// Buffer B
	else if (c == 'b')
	{
		namedWindow("B", CV_WINDOW_AUTOSIZE);
		imshow("B", B);
	}
	// Variance
	else if (c == 'v')
	{
		namedWindow("Variance", CV_WINDOW_AUTOSIZE);
		imshow("Variance", V);
	}
	// Filtered
	else if (c == 'f')
	{
		namedWindow("Filtered", CV_WINDOW_AUTOSIZE);
		imshow("Filtered", F);
	}
	else if (c == 'e')
	{
		namedWindow("Empirical Variance", CV_WINDOW_AUTOSIZE);
		imshow("Empirical Variance", V_e);
	}

	waitKey(0);
}

void Img::setBuffers(const char * filepathA, const char * filepathB)
{
	A = imread(filepathA);
	B = imread(filepathB);

	width = A.cols;
	height = A.rows;

	// Set weights
	W = new float*[height];
	for (int i = 0; i < height; i++)
		W[i] = new float[width];

#pragma omp parallel
	for (int i = 0; i < height; i++)
#pragma omp for schedule(dynamic,1)
		for (int j = 0; j < width; j++)
		{
			W[i][j] = 0.0f;
		}
}

void Img::Filter(int mode)
{
	if (mode == 0)
		filter(A, B);
	else
	{
		// Filter variance
	}
}

void Img::initVarEst()
{
	/*
	Mat diff, sqr, nor;
	absdiff(A, B, diff);
	namedWindow("Diff", CV_WINDOW_AUTOSIZE);
	imshow("Diff", diff);

	pow(diff, 2, sqr);
	namedWindow("Sqr", CV_WINDOW_AUTOSIZE);
	imshow("Sqr", sqr);

	nor = 0.5f * sqr;
	namedWindow("Norm", CV_WINDOW_AUTOSIZE);
	imshow("Norm", nor);
	waitKey(0);
	*/

	//V = A.clone();
	absdiff(A, B, V);
	//for (int i = 0; i < height; i++)
	//{
	//	for (int j = 0; j < width; j++)
	//	{
	//		uchar b = V.at<Vec3b>(i, j).val[0];
	//		uchar g = V.at<Vec3b>(i, j).val[1];
	//		uchar r = V.at<Vec3b>(i, j).val[2];

	//		float bf = float(b);
	//		float gf = float(g);
	//		float rf = float(r);

	//		bf = bf * bf * 0.5f;
	//		gf = gf * gf * 0.5f;
	//		rf = rf * rf * 0.5f;

	//		V.at<Vec3b>(i, j).val[0] = uchar(bf);
	//		V.at<Vec3b>(i, j).val[1] = uchar(gf);
	//		V.at<Vec3b>(i, j).val[2] = uchar(rf);
	//	}
	//}
	
	pow(V, 2, V);
	V *= 0.5f;
}

void Img::empVar(int h, int w)
{
	//V_e = V.clone(); // Mat::zeros(height, width, CV_32F);
	V_e = Mat(h, w, CV_8UC3, Scalar(0, 0, 0));

	//for(int i = 0; i < height; i++)
	//	for (int j = 0; j < width; j++)
	//	{
	//		setEmpV(i, j, glm::vec3(1.0f, 0.0f, 0.0f));
	//	}
}

void Img::setEmpV(int i, int j, glm::vec3 value)
{
	V_e.at<Vec3b>(i, j).val[0] = uchar(value.z * 255.0f);
	V_e.at<Vec3b>(i, j).val[1] = uchar(value.y * 255.0f);
	V_e.at<Vec3b>(i, j).val[2] = uchar(value.x * 255.0f);
}

void Img::filter(Mat& M0, Mat& M1)
{
	// This function filters buffer M0 with M1
	// Weights are calculated from M1 and applied on M0

	F = M0.clone();//Mat(2, 2, CV_8UC3, Scalar(0, 0, 0));

	#pragma omp parallel
	for (int i = r + f; i < height - (r + f); i++)
	{
		#pragma omp for schedule(dynamic,1)
		for (int j = r + f; j < width - (r + f); j++)
		{
			// Filter each pixel
			filterPixel(i, j, F, M1);
		}
	}

	/*
	// Average weights
	#pragma omp parallel
	for (int i = 0; i < I.rows; i++)
		#pragma omp for schedule(dynamic,1)
		for (int j = 0; j < I.cols; j++)
		{
			W[i][j] /= pow((2 * f + 1), 2.0f);
		}

	for (int i = 30; i < 35; i++)
		for (int j = 30; j < 35; j++)
			cout << W[i][j] << endl;
			*/
}

void Img::filterPixel(int i, int j, Mat& M0, Mat& M1)
{
	float totalW = 0.0f;
	Vec3f sum(0.0f);

	// Iterate all neighboring patches in buffer M1
	for (int offset = -r; offset <= r; offset++)
	{
		float D = getDistPatch(i, j, i + offset, j + offset, M1);
		float w = getWeight(D);
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
}

float Img::getDistPatch(int pi, int pj, int qi, int qj, Mat & M)
{
	Vec3f dist(0.0f);

	for (int di = -f; di <= f; di += 1)
	{
		for (int dj = -f; dj <= f; dj += 1)
		{
			dist += getModDistPix(pi + di, pj + dj, qi + di, dj + dj, M);
		}
	}

	float D = dist[0] + dist[1] + dist[2];
	D /= float(3 * pow(2 * f + 1, 2));

	// Weight all pixels within the patch P as w
	//float w = getWeight(D);
	//for (int di = -f; di <= f; di += 1)
	//{
	//	for (int dj = -f; dj <= f; dj += 1)
	//	{
	//		W[pi + di][pj + dj] += w;
	//	}
	//}

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

	//printf("%u ", pcolor);
	//cout << float(pcolor) << endl;
}

Vec3f Img::getModDistPix(int pi, int pj, int qi, int qj, Mat & M)
{

	Vec3b pvar = V.at<Vec3b>(pi, pj);
	Vec3b qvar = V.at<Vec3b>(qi, qj);

	glm::vec3 pf(float(pvar.val[0]), float(pvar.val[1]), float(pvar.val[2]));
	glm::vec3 qf(float(qvar.val[0]), float(qvar.val[1]), float(qvar.val[2]));
	pf /= 255.0f;
	qf /= 255.0f;

	//if(pf.x > 0)
	//	cout << pf.x << endl;

	//if(pi > 400 && pj > 400)	
	//	cout << (float)pvar.val[0] << endl;

	uchar & pb = M.at<Vec3b>(pi, pj).val[0];
	uchar & qb = M.at<Vec3b>(qi, qj).val[0];
	float diffb = (float(pb) - float(qb)) / 255.0f;

	float div = k * k * (pf.x + qf.x);
	float e_ = 0.0f;
	if (div < 0.01f)
		e_ = 0.1f;

	diffb = (diffb*diffb - a * (pf.x + max(pf.x, qf.x))) / (e_ + div);

	uchar & pg = M.at<Vec3b>(pi, pj).val[1];
	uchar & qg = M.at<Vec3b>(qi, qj).val[1];
	float diffg = (float(pg) - float(qg)) / 255.0f;

	div = k * k * (pf.y + qf.y);
	e_ = 0.0f;
	if (div < 0.01f)
		e_ = 0.1f;
	diffg = (diffg*diffg - a * (pf.y + max(pf.y, qf.y))) / (e_ + div);

	uchar & pr = M.at<Vec3b>(pi, pj).val[2];
	uchar & qr = M.at<Vec3b>(qi, qj).val[2];
	float diffr = (float(pr) - float(qr)) / 255.0f;

	div = k * k * (pf.z + qf.z);
	e_ = 0.0f;
	if (div < 0.01f)
		e_ = 0.1f;
	diffr = (diffr*diffr - a * (pf.z + max(pf.z, qf.z))) / (e_ + div);

	return Vec3f(diffb, diffg, diffr);
}
