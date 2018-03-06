#include "Img.h"

using namespace cv;
using namespace std;

Img::Img()
{
}

void Img::readImg()
{
	// Testing error map
	//Mat T(2, 2, CV_64FC1, 0.0);
	//cout << T.at<double>(0, 0) << endl;
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
		Mat M;
		absdiff(F_a, F_b, M);
		namedWindow("Filtered", CV_WINDOW_AUTOSIZE);
		imshow("Filtered", M);
		imwrite("D://Github//temp//FilteredVarResult.png", M);
	}
	else if (c == 'g')
	{
		namedWindow("Filtered2", CV_WINDOW_AUTOSIZE);
		imshow("Filtered2", F_b);
	}
	else if (c == 'c')
	{
		namedWindow("Combined", CV_WINDOW_AUTOSIZE);
		imshow("Combined", C);
	}
	else if (c == 'e')
	{
		Mat dst = W_a.clone();
		normalize(W_a, dst, 0, 1, NORM_MINMAX);
		namedWindow("Computed Variance", CV_WINDOW_AUTOSIZE);
		imshow("Computed Variance", dst);
		//imwrite("D://Github//temp//Result//wth.png", 255* dst);
	}

	waitKey(0);
}

void Img::computeError()
{
	P = Mat(height, width, CV_32FC1, 4.0);

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

			//if (i > 900 && j > 1200)
				//cout << error << endl;
				//cout << b << " " << g << " " << r << endl;
			// Error map for A
			float e_a = ba*ba + ga*ga + ra*ra;
			if (e_a < 0.000001f)
				e_a = error;
			else
				e_a = error / e_a;

			E_a.at<float>(i, j) = e_a * W_a.at<float>(i, j) /( 1 + P.at<float>(i, j));
			//if (i > 900 && j > 1200)
			//	cout << e_a * W_a.at<float>(i, j) / (1 + P.at<float>(i, j)) << endl;

			// Error map for B
			float e_b = bb*bb + gb*gb + rb*rb;
			if (e_b < 0.000001f)
				e_b = error;
			else
				e_b = error / e_b;
			E_b.at<float>(i, j) = e_b * W_b.at<float>(i, j) /(1 + P.at<float>(i, j));
		}
	}

	cout << "Error map constructed" << endl;
	Mat combined;
	add(E_a, E_b, combined);
	normalize(combined, combined, 0, 1, NORM_MINMAX);
	namedWindow("Error map", CV_WINDOW_AUTOSIZE);
	imshow("Error map", 255 * combined);
	waitKey(0);
}

void Img::setBuffers(const char * filepathA, const char * filepathB)
{
	A = imread(filepathA);
	B = imread(filepathB);

	width = A.cols;
	height = A.rows;
}

void Img::Filter(int mode)
{
	if (mode == 0)
		filter(A, B, mode);
	else
		filter(V, V_e, mode);
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

			//if(i > 400 && j > 400)
			//	cout << bf << " " << gf << " " << rf << endl;
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

void Img::empVar(int h, int w)
{
	//V_e = V.clone(); // Mat::zeros(height, width, CV_32F);
	V_e = Mat(h, w, CV_8UC3, Scalar(0, 0, 0));
}

void Img::setEmpV(int i, int j, glm::vec3 value)
{
	V_e.at<Vec3b>(i, j).val[0] = uchar(value.z * 255.0f);
	V_e.at<Vec3b>(i, j).val[1] = uchar(value.y * 255.0f);
	V_e.at<Vec3b>(i, j).val[2] = uchar(value.x * 255.0f);
}

void Img::setVbuffer(Mat & M, int mode)
{
	if (mode == 0)
		V_a = M.clone();
	else if (mode == 1)
		V_b = M.clone();
	else
		V_e = M.clone();
}

void Img::combineBuffers(Mat & M0, Mat & M1)
{
	addWeighted(M0, 0.5, M1, 0.5, 0.0, C);

}

void Img::filter(Mat& M0, Mat& M1, int mode)
{
	// This function filters buffer M0 with M1
	// Weights are calculated from M1 and applied on M0

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
				//if (i > 900 && j > 1200)
				W_a.at<float>(i, j) = w_a;
				W_b.at<float>(i, j) = w_b;
			}
			else
				filterPixel(i, j, F_v, M1, mode);

		}
	}

	if (mode == 0)
	{
		combineBuffers(F_a, F_b);
		imwrite("D://Github//temp//test_result.png", C);
	}
	else
		imwrite("D://Github//temp//test_result_V.png", F_v);

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
	{
		for (int dj = -f; dj <= f; dj += 1)
		{
			if(mode == 0)	
				dist += getModDistPix(pi + di, pj + dj, qi + di, qj + dj, M);
			else
				dist += getModDistPix2(pi + di, pj + dj, qi + di, qj + dj, M);
		}
	}

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

Vec3f Img::getModDistPix(int pi, int pj, int qi, int qj, Mat & M)
{
	//Vec3b pvar = V.at<Vec3b>(pi, pj);
	//Vec3b qvar = V.at<Vec3b>(qi, qj);
	Vec3b pvar = F_v.at<Vec3b>(pi, pj);
	Vec3b qvar = F_v.at<Vec3b>(qi, qj);

	glm::vec3 pf(float(pvar.val[0]), float(pvar.val[1]), float(pvar.val[2]));
	glm::vec3 qf(float(qvar.val[0]), float(qvar.val[1]), float(qvar.val[2]));
	pf /= 255.0f;
	qf /= 255.0f;

	uchar & pb = M.at<Vec3b>(pi, pj).val[0];
	uchar & qb = M.at<Vec3b>(qi, qj).val[0];
	float diffb = (float(pb) - float(qb)) / 255.0f;

	float div = k * k * (pf.x + qf.x);
	if (div < 0.03f)
		diffb = (diffb*diffb);
	else
		diffb = (diffb*diffb - a * (pf.x + max(pf.x, qf.x))) / (div);

	uchar & pg = M.at<Vec3b>(pi, pj).val[1];
	uchar & qg = M.at<Vec3b>(qi, qj).val[1];
	float diffg = (float(pg) - float(qg)) / 255.0f;

	div = k * k * (pf.y + qf.y);
	if (div < 0.03f)
		diffg = (diffg*diffg);
	else
		diffg = (diffg*diffg - a * (pf.y + max(pf.y, qf.y))) / (div);

	uchar & pr = M.at<Vec3b>(pi, pj).val[2];
	uchar & qr = M.at<Vec3b>(qi, qj).val[2];
	float diffr = (float(pr) - float(qr)) / 255.0f;

	div = k * k * (pf.z + qf.z);
	if (div < 0.03f)
		diffr = (diffr*diffr);
	else
		diffr = (diffr*diffr - a * (pf.z + max(pf.z, qf.z))) / (div);

	return Vec3f(diffb, diffg, diffr);

	/*
	uchar & pb = M.at<Vec3b>(pi, pj).val[0];
	uchar & qb = M.at<Vec3b>(qi, qj).val[0];
	float diffb = (float(pb) - float(qb)) / 255.0f;

	float div = k * k * (pf.x + qf.x);
	float e_ = 0.0f;
	if (div < 0.01f)
		e_ = 1.0f;// 0.1f;
	diffb = (diffb*diffb - a * (pf.x + max(pf.x, qf.x))) / (e_ + div);

	uchar & pg = M.at<Vec3b>(pi, pj).val[1];
	uchar & qg = M.at<Vec3b>(qi, qj).val[1];
	float diffg = (float(pg) - float(qg)) / 255.0f;

	div = k * k * (pf.y + qf.y);
	e_ = 0.0f;
	if (div < 0.01f)
		e_ = 1.0f;//0.1f;
	diffg = (diffg*diffg - a * (pf.y + max(pf.y, qf.y))) / (e_ + div);

	uchar & pr = M.at<Vec3b>(pi, pj).val[2];
	uchar & qr = M.at<Vec3b>(qi, qj).val[2];
	float diffr = (float(pr) - float(qr)) / 255.0f;

	div = k * k * (pf.z + qf.z);
	e_ = 0.0f;
	if (div < 0.01f)
		e_ = 1.0f;// 0.1f;
	diffr = (diffr*diffr - a * (pf.z + max(pf.z, qf.z))) / (e_ + div);

	return Vec3f(diffb, diffg, diffr);
	*/
}

Vec3f Img::getModDistPix2(int pi, int pj, int qi, int qj, Mat & M)
{

	Vec3b pvar = V_d.at<Vec3b>(pi, pj);
	Vec3b qvar = V_d.at<Vec3b>(qi, qj);

	glm::vec3 pf(float(pvar.val[0]), float(pvar.val[1]), float(pvar.val[2]));
	glm::vec3 qf(float(qvar.val[0]), float(qvar.val[1]), float(qvar.val[2]));
	pf /= 255.0f;
	qf /= 255.0f;

	uchar & pb = M.at<Vec3b>(pi, pj).val[0];
	uchar & qb = M.at<Vec3b>(qi, qj).val[0];
	float diffb = (float(pb) - float(qb)) / 255.0f;

	float div = k * k * (pf.x + qf.x);
	if (div < 0.01f)//0.1f
		diffb = (diffb*diffb);
	else
		diffb = (diffb*diffb - a * (pf.x + max(pf.x, qf.x))) / (div);

	uchar & pg = M.at<Vec3b>(pi, pj).val[1];
	uchar & qg = M.at<Vec3b>(qi, qj).val[1];
	float diffg = (float(pg) - float(qg)) / 255.0f;

	div = k * k * (pf.y + qf.y);
	if (div < 0.01f)
		diffg = (diffg*diffg);
	else
		diffg = (diffg*diffg - a * (pf.y + max(pf.y, qf.y))) / (div);

	uchar & pr = M.at<Vec3b>(pi, pj).val[2];
	uchar & qr = M.at<Vec3b>(qi, qj).val[2];
	float diffr = (float(pr) - float(qr)) / 255.0f;

	div = k * k * (pf.z + qf.z);
	if (div < 0.01f)
		diffr = (diffr*diffr);
	else
		diffr = (diffr*diffr - a * (pf.z + max(pf.z, qf.z))) / (div);

	return Vec3f(diffb, diffg, diffr);
}
