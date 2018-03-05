#include "Img.h"

using namespace cv;
using namespace std;

Img::Img()
{
}

void Img::readImg()
{
	I = imread(".//Images//cat.jpg");

	// test
	//int i = 0;
	//int col = 0;
	//for (int i = 1; i < 30; i++)
	//{
	//	for (int col = 1; col < 50; col++)
	//	{
	//		Vec3b & color = I.at<Vec3b>(i, col);
	//		color[0] = 255;
	//		color[1] = 0;
	//		color[2] = 0;
	//	}
	//}
}

void Img::showImg()
{
	namedWindow("Image Viewer", CV_WINDOW_AUTOSIZE);
	imshow("Image Viewer", I);
	waitKey(0);
}

void Img::getBoundary()
{
	for (int i = r + f; i < I.rows - (r + f); i++)
	{
		for (int j = r + f; j < I.cols - (r + f); j++)
		{
			float D(0.0f);
			for(int offset = -r; offset <= r; offset++)
				D += getDistPatch(i, j, i + offset, j + offset);
			//cout << D << endl;
			//cout << D[0] << " " << D[1] << " " << D[2] << endl;
		}
		
	}
}

float Img::getDistPatch(int pi, int pj, int qi, int qj)
{
	Vec3f dist(0.0f);
	//int counter = 0;

	for (int di = -f; di <= f; di += 1)
	{
		for (int dj = -f; dj <= f; dj += 1)
		{
			dist += getDistPix(pi + di, pj + dj, qi + di, dj + dj);
			//counter++;
		}
	}

	float D = dist[0] + dist[1] + dist[2];
	D /= float(3 * pow(2 * f + 1, 2));

	return D;

	//return dist;
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