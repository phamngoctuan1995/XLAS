#include "Header.h"

static Mat Shift(const Mat &src)
{
	Mat temp = src(Rect(0, 0, src.cols & -2, src.rows & -2)),
		des = temp.clone();
	int cx = des.cols / 2;
	int cy = des.rows / 2;

	Mat q0(des, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(des, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(des, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(des, Rect(cx, cy, cx, cy)); // Bottom-Right

	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);
	return des;
}

static int MaxDivisor(int x)
{
	if (x < 4)
		return x;
	int xx = (int)sqrt(x*1.0F);
	for (int i = 2; i <= xx; ++i)
	if (x % i == 0)
		return x / i;
	return x;
}

static int DFTSize(int n)
{
	for (int i = n; i < DFT_MAX; ++i)
	{
		if (i % 2)
			continue;
		
		bool flag = false;
		int thres = (int)sqrt(i*1.0F);

		for (int j = 1; j <= thres; ++j)
		if (i % j == 0 && ((j >= 7 && MaxDivisor(j) == j)
			|| MaxDivisor(i / j) == i / j))
		{
			flag = true;
			break;
		}

		if (!flag)
			return i;
	}

	return DFT_MAX;
}

void Fourier(const Mat& img, const Mat& imgI, Mat& rea, Mat& ima,
	int nu, int alpha, int nv, int beta, bool isInv)
{
	int hei = img.rows / nu, wid = img.cols / nv;
	float mul = 1.0F, coef = float(isInv) * 2 - 1;
	if (!isInv)
		mul /= img.rows*img.cols;

	for (int i = 0; i < hei; ++i)
	{
		int nui = nu*i + alpha;
		for (int j = 0; j < wid; ++j)
		{
			int nvj = nv*j + beta;
			float sumR = 0, sumI = 0,
				tI = 1.0F*i / hei, tJ = 1.0F*j / wid;
			for (int a = 0; a < hei; ++a)
			{
				int nua = nu*a + alpha;
				for (int b = 0; b < wid; ++b)
				{
					int nvb = nv*b + beta;
					float tem = 2 * PI*(tI*a + tJ*b), ctem = cos(tem),
						stem = sin(tem)*coef, keyR, keyI;
					if (!isInv)
					{
						keyR = img.at<unsigned char>(nua, nvb) / 255.0F;
						keyI = 0;
					}
					else
					{
						keyR = img.at<float>(nua, nvb);
						keyI = imgI.at<float>(nua, nvb);
					}
					if (!isInv && (nua + nvb) % 2)//
						keyR *= -1;
					sumR += keyR*ctem - keyI*stem;
					sumI += keyR*stem + keyI*ctem;
				}
			}

			rea.at<float>(nui, nvj) = sumR*mul;
			ima.at<float>(nui, nvj) = sumI*mul;
		}
	}
}

void FourierFast(const Mat& img, const Mat& imgI, Mat& rea, Mat& ima,
	int nu, int alpha, int nv, int beta, bool isInv)
{
	int hei = img.rows / nu, wid = img.cols / nv;
	float coef = float(isInv) * 2 - 1;

	int maxHei = MaxDivisor(hei), maxWid = MaxDivisor(wid),
		minHei = hei / maxHei, minWid = wid / maxWid;
	if (maxHei == hei && maxWid == wid)
	{
		Fourier(img, imgI, rea, ima, nu, alpha, nv, beta, isInv);
		return;
	}

	int mulHei = nu*minHei, mulWid = nv*minWid;
	for (int i = (minHei - 1)*nu + alpha; i >= alpha; i -= nu)
	for (int j = (minWid - 1)*nv + beta; j >= beta; j -= nv)
		FourierFast(img, imgI, rea, ima, mulHei, i, mulWid, j, isInv);

	Mat tempR(hei, wid, CV_32FC1), tempI(hei, wid, CV_32FC1);
	for (int i = 0; i < hei; ++i)
	{
		int nui = nu*i + alpha;
		for (int j = 0; j < wid; ++j)
		{
			int nvj = nv*j + beta;
			tempR.at<float>(i, j) = rea.at<float>(nui, nvj);
			tempI.at<float>(i, j) = ima.at<float>(nui, nvj);
		}
	}

	for (int i = 0; i < hei; ++i)
	{
		int nui = nu*i + alpha, x = i % maxHei;
		for (int j = 0; j < wid; ++j)
		{
			int nvj = nv*j + beta, y = j % maxWid;
			float sumR = 0, sumI = 0,
				tI = 1.0F*i / hei, tJ = 1.0F*j / wid;
			for (int a = 0; a < minHei; ++a)
			for (int b = 0; b < minWid; ++b)
			{
				float ang = 2 * PI*(tI*a + tJ*b),
					cosA = cos(ang), sinA = sin(ang)*coef;
				int posX = minHei*x + a, posY = minWid*y + b;
				float txyR = tempR.at<float>(posX, posY),
					txyI = tempI.at<float>(posX, posY);
				sumR += txyR*cosA - txyI*sinA;
				sumI += txyR*sinA + txyI*cosA;
			}

			rea.at<float>(nui, nvj) = sumR;
			ima.at<float>(nui, nvj) = sumI;
		}
	}
}

void FourierThuan(const Mat& img, Mat &fouR, Mat &fouI)
{
	int wid = img.cols, hei = img.rows;
	int sub1 = DFTSize(wid) - wid;
	Mat sta1, img1(img);
	if (sub1 > 0)
		hconcat(img, Mat::zeros(hei, sub1, CV_8UC1), sta1);
	int sub2 = DFTSize(hei) - hei;
	if (sub2 > 0)
		vconcat(sta1, Mat::zeros(sub2, wid + sub1, CV_8UC1), img1);

	fouR = Mat::zeros(img1.rows, img1.cols, CV_32FC1),
	fouI = Mat::zeros(img1.rows, img1.cols, CV_32FC1);
	FourierFast(img1, Mat(), fouR, fouI, 1, 0, 1, 0, false);
}

void FourierNguoc(const Mat &fouR, const Mat &fouI, Mat &resR, Mat &resI)
{
	resR = Mat::zeros(fouR.rows, fouR.cols, CV_32FC1),
	resI = Mat::zeros(fouR.rows, fouR.cols, CV_32FC1);
	FourierFast(fouR, fouI, resR, resI, 1, 0, 1, 0, true);
	for (int i = 0; i < fouR.rows; ++i)
	for (int j = 0; j < fouR.cols; ++j)
	if ((i + j) % 2)
		resR.at<float>(i, j) *= -1;
}

void FourierFilter(const Mat &fouR, const Mat &fouI, const Mat &resO, bool isHigh)
{
	Mat h, resR, resI, tempR, tempI;
	Mat fouO[2];
	float D, n;
	
	Mat res = Shift(resO);
	split(res, fouO);

	cout << "\nIdeal Filter" << endl;
	cout << "Nhap D: ";
	cin >> D;
	h = IdealFilter(fouR.rows, fouR.cols, D, isHigh);
	ElementMultiply(fouR, fouI, resR, resI, h);
	FourierNguoc(resR, resI, tempR, tempI);


	ElementMultiply(fouO[0], fouO[1], fouO[0], fouO[1], h);
	merge(fouO, 2, tempI);
	tempI = Shift(tempI);
	Fourier_OpenCV(tempI, tempI, true);

	imshow("Ideal Filter", tempR);
	imshow("Ideal Filter - OpenCV", tempI);
	if (isHigh)
	{
		imwrite("Hinh\\High-pass Ideal Filter.jpg", tempR * 255);
		imwrite("Hinh\\High-pass Ideal Filter - OpenCV.jpg", tempI);
	}
	else
	{
		imwrite("Hinh\\Low-pass Ideal Filter.jpg", tempR * 255);
		imwrite("Hinh\\Low-pass Ideal Filter - OpenCV.jpg", tempI);
	}
	waitKey(0);

	split(res, fouO);
	// ButterworthFilter
	cout << "\nButterworth Filter" << endl;
	cout << "Nhap D, n: ";
	cin >> D >> n;
	h = ButterworthFilter(fouR.rows, fouR.cols, D, n - 2 * n * isHigh);
	ElementMultiply(fouR, fouI, resR, resI, h);
	FourierNguoc(resR, resI, tempR, tempI);

	ElementMultiply(fouO[0], fouO[1], fouO[0], fouO[1], h);
	merge(fouO, 2, tempI);
	tempI = Shift(tempI);
	Fourier_OpenCV(tempI, tempI, true);

	imshow("Butterworth Filter", tempR);
	imshow("Butterworth Filter - OpenCV", tempI);
	if (isHigh)
	{
		imwrite("Hinh\\High-pass Butterworth Filter.jpg", tempR * 255);
		imwrite("Hinh\\High-pass Butterworth Filter - OpenCV.jpg", tempI);
	}
	else
	{
		imwrite("Hinh\\Low-pass Butterworth Filter.jpg", tempR * 255);
		imwrite("Hinh\\Low-pass Butterworth Filter - OpenCV.jpg", tempI);
	}
	waitKey(0);

	split(res, fouO);
	cout << "\nGaussian Filter" << endl;
	cout << "Nhap var: ";
	cin >> D;
	h = GaussianFilter(fouR.rows, fouR.cols, D, isHigh);
	ElementMultiply(fouR, fouI, resR, resI, h);
	FourierNguoc(resR, resI, tempR, tempI);

	ElementMultiply(fouO[0], fouO[1], fouO[0], fouO[1], h);
	merge(fouO, 2, tempI);
	tempI = Shift(tempI);
	Fourier_OpenCV(tempI, tempI, true);

	imshow("Gaussian Filter", tempR);
	imshow("Gaussian Filter - OpenCV", tempI);
	if (isHigh)
	{
		imwrite("Hinh\\High-pass Gaussian Filter.jpg", tempR * 255);
		imwrite("Hinh\\High-pass Gaussian Filter - OpenCV.jpg", tempI);
	}
	else
	{
		imwrite("Hinh\\Low-pass Gaussian Filter.jpg", tempR * 255);
		imwrite("Hinh\\Low-pass Gaussian Filter - OpenCV.jpg", tempI);
	}
	waitKey(0);
}

Mat IdealFilter(int hei, int wid, float D, bool isHigh)
{
	Mat res = Mat::zeros(hei, wid, CV_32FC1);
	float H = hei / 2, W = wid / 2;
	float d;
	D = pow(D, 2);

	for (int i = 0; i < H; ++i)
	for (int j = 0; j < W; ++j)
	{
		d = pow(H - i, 2.0F) + pow(W - j, 2.0F);
		if ((d <= D && !isHigh) || (d > D && isHigh))
			res.at<float>(i, j) = res.at<float>(hei - i - 1, wid - j - 1)
			= res.at<float>(i, wid - j - 1) = res.at<float>(hei - i - 1, j) = 1;
	}
	return res;
}

Mat ButterworthFilter(int hei, int wid, float D, float n)
{
	Mat res = Mat::zeros(hei, wid, CV_32FC1);
	float H = hei / 2, W = wid / 2;
	float d;
	D = pow(D, 2.0F);

	for (int i = 0; i < H; ++i)
	for (int j = 0; j < W; ++j)
	{
		d = pow(H - i, 2.0F) + pow(W - j, 2.0F);
		res.at<float>(i, j) = res.at<float>(hei - i - 1, wid - j - 1)
			= res.at<float>(i, wid - j - 1) = res.at<float>(hei - i - 1, j) = 1 / (1 + pow(d / D, n));
	}
	return res;
}

Mat GaussianFilter(int hei, int wid, float var, bool isHigh)
{
	Mat res = Mat::zeros(hei, wid, CV_32FC1);
	float H = hei / 2, W = wid / 2;
	float d;
	var = 2 * pow(var, 2.0F);

	for (int i = 0; i < H; ++i)
	for (int j = 0; j < W; ++j)
	{
		d = pow(H - i, 2.0F) + pow(W - j, 2.0F);
		res.at<float>(i, j) = res.at<float>(hei - i - 1, wid - j - 1)
			= res.at<float>(i, wid - j - 1) = res.at<float>(hei - i - 1, j) = exp(-d / var);
	}

	if (isHigh)
		subtract(Mat::ones(hei, wid, CV_32FC1), res, res);
	return res;
}

void ElementMultiply(const Mat &real, const Mat &imag, Mat &resR, Mat &resI, const Mat &h)
{
	multiply(real, h, resR);
	multiply(imag, h, resI);
}

void Fourier_OpenCV(Mat src, Mat &des, bool isInv)
{
	if (!isInv)
	{
		src.convertTo(src, CV_32F);
		int w = getOptimalDFTSize(src.cols);
		int h = getOptimalDFTSize(src.rows);
		copyMakeBorder(src, des, 0, h - src.rows, 0, w - src.cols, BORDER_CONSTANT, Scalar::all(0));
		Mat planes[] = { Mat_<float>(des), Mat::zeros(des.size(), CV_32F) };
		merge(planes, 2, des);

		dft(des, des, DFT_COMPLEX_OUTPUT);
	}
	else
	{
		dft(src, des, DFT_INVERSE | DFT_SCALE | DFT_REAL_OUTPUT);
		des.convertTo(des, CV_8U);
	}
}