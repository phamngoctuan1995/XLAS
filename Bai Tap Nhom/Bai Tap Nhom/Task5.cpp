#include "Header.h"

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

static void Fourier(const Mat& img, Mat& rea, Mat& ima)
{
	float hei = img.rows, wid = img.cols,
		mul = hei*wid;

	for (int i = 0; i < hei; ++i)
	for (int j = 0; j < wid; ++j)
	{
		float sumR = 0, sumI = 0,
			u = 1.0F*i / hei, v = 1.0F*j / wid;
		for (int a = 0; a < hei; ++a)
		for (int b = 0; b < wid; ++b)
		{
			float tem = 2 * PI*(u*a + v*b),
				key = img.at<unsigned char>(a, b) / 255.0F;//
			sumR += key*cos(tem);//
			sumI += key*sin(tem);//
		}

		rea.at<float>(i, j) = sumR / mul;//
		ima.at<float>(i, j) = -sumI / mul;//
	}
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
	Mat sta1;
	if (sub1 > 0)
		hconcat(img, Mat::zeros(hei, sub1, CV_8UC1), sta1);
	int sub2 = DFTSize(hei) - hei;
	if (sub2 > 0)
		vconcat(sta1, Mat::zeros(sub2, wid + sub1, CV_8UC1), img);

	fouR = Mat::zeros(img.rows, img.cols, CV_32FC1),
	fouI = Mat::zeros(img.rows, img.cols, CV_32FC1);
	FourierFast(img, Mat(), fouR, fouI, 1, 0, 1, 0, false);
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

void FourierFilter(const Mat &fouR, const Mat &fouI, bool isHigh)
{
	Mat h, resR, resI, tempR, tempI;
	float D, n;

	cout << "\nIdeal Filter" << endl;
	cout << "Nhap D: ";
	cin >> D;
	h = IdealFilter(fouR.rows, fouR.cols, D, isHigh);
	ElementMultiply(fouR, fouI, resR, resI, h);
	FourierNguoc(resR, resI, tempR, tempI);
	imshow("Ideal Filter", tempR);
	waitKey(0);

	// ButterworthFilter
	cout << "\nButterworth Filter" << endl;
	cout << "Nhap D, n: ";
	cin >> D >> n;
	h = ButterworthFilter(fouR.rows, fouR.cols, D, n - 2 * n * isHigh);
	ElementMultiply(fouR, fouI, resR, resI, h);
	FourierNguoc(resR, resI, tempR, tempI);
	imshow("Butterworth Filter", tempR);
	waitKey(0);

	cout << "\nGaussian Filter" << endl;
	cout << "Nhap var: ";
	cin >> D;
	h = GaussianFilter(fouR.rows, fouR.cols, D, isHigh);
	ElementMultiply(fouR, fouI, resR, resI, h);
	FourierNguoc(resR, resI, tempR, tempI);
	imshow("Gaussian Filter", tempR);
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