#include "Img.h"

using namespace cv;
using namespace std;

Img::Img()
{
}

void Img::readImg()
{
	I = imread(".//Images//cat.jpg");

	W = new float*[I.rows];
	for (int i = 0; i < I.rows; i++)
		W[i] = new float[I.cols];

	#pragma omp parallel
	for(int i = 0; i < I.rows; i++)
		#pragma omp for schedule(dynamic,1)
		for (int j = 0; j < I.cols; j++)
		{
			W[i][j] = 0.0f;
		}
}

void Img::showImg()
{
	namedWindow("Image Viewer", CV_WINDOW_AUTOSIZE);
	imshow("Image Viewer", I);
	waitKey(0);
}

void Img::filterPixel(int i, int j, Mat & A, Mat & B)
{
	float totalW = 0.0f;
	Vec3f sum(0.0f);

	// Iterate all neighboring patches in buffer B
	for (int offset = -r; offset <= r; offset++)
	{
		float D = getDistPatch(i, j, i + offset, j + offset);
		float w = getWeight(D);
		totalW += w;
		Vec3b q = B.at<Vec3b>(i + offset, j + offset);
		sum[0] += w * float(q.val[0]) / 255.0f;
		sum[1] += w * float(q.val[1]) / 255.0f;
		sum[2] += w * float(q.val[2]) / 255.0f;
	}

	sum /= totalW;

	// Modify values in buffer A
	A.at<Vec3b>(i, j).val[0] = uchar(sum[0] * 255.0f);
	A.at<Vec3b>(i, j).val[1] = uchar(sum[1] * 255.0f);
	A.at<Vec3b>(i, j).val[2] = uchar(sum[2] * 255.0f);
}

void Img::filter()
{
	#pragma omp parallel
	for (int i = r + f; i < I.rows - (r + f); i++)
	{
		#pragma omp for schedule(dynamic,1)
		for (int j = r + f; j < I.cols - (r + f); j++)
		{
			// 
		}
	}

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
}

float Img::getDistPatch(int pi, int pj, int qi, int qj)
{
	Vec3f dist(0.0f);

	for (int di = -f; di <= f; di += 1)
	{
		for (int dj = -f; dj <= f; dj += 1)
		{
			dist += getDistPix(pi + di, pj + dj, qi + di, dj + dj);
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

Vec3f Img::getDistPix(int pi, int pj, int qi, int qj)
{
	uchar & pb = I.at<Vec3b>(pi, pj).val[0];
	uchar & qb = I.at<Vec3b>(qi, qj).val[0];
	float diffb = (float(pb) - float(qb)) / 255.0f;

	uchar & pg = I.at<Vec3b>(pi, pj).val[1];
	uchar & qg = I.at<Vec3b>(qi, qj).val[1];
	float diffg = (float(pg) - float(qg)) / 255.0f;

	uchar & pr = I.at<Vec3b>(pi, pj).val[2];
	uchar & qr = I.at<Vec3b>(qi, qj).val[2];
	float diffr = (float(pr) - float(qr)) / 255.0f;

	return Vec3f(diffb * diffb, diffg * diffg, diffr * diffr);

	//printf("%u ", pcolor);
	//cout << float(pcolor) << endl;

	//return pow((float(pcolor) - float(qcolor)) / 255.0f, 2.0f);

	//Vec3b & pcolor = I.at<Vec3b>(pi, pj);
	//Vec3b & qcolor = I.at<Vec3b>(qi, qj);
	//Vec3f diff(float);

	//return 0.0f;
}

Vec3f Img::getModDistPix(int pi, int pj, int qi, int qj, float vp, float vq)
{
	Vec3f diff = getDistPix(pi, pj, qi, qj);

	// Later

	return diff;
}
