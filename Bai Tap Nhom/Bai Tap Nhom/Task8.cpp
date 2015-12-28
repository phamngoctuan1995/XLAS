#include "Header.h"

void Morphology(Mat src, Mat &dst, const Mat &element, int type, Point anchor)
{
	dst = src.clone();

	if (anchor.x < 0 || anchor.x >= element.rows || anchor.y < 0 || anchor.y >= element.cols)
	{
		anchor.x = element.rows / 2;
		anchor.y = element.cols / 2;
	}

	Mat temp;
	int val = -1, tempx, tempy;
	switch (type)
	{
	case MORPH_DILATE:
	case MORPH_ERODE:
		for (int i = 0; i < src.rows; ++i)
		for (int j = 0; j < src.cols; ++j)
		{
			val = -1;
			for (int h = 0; h < element.rows; ++h)
			for (int k = 0; k < element.cols; ++k)
			if (element.at<uchar>(h, k))
			{
				tempx = i + h - anchor.x;
				tempy = j + k - anchor.y;

				if (tempx < 0 || tempx >= src.rows || tempy < 0 || tempy >= src.cols)
					continue;
				if (val == -1 || (src.at<uchar>(tempx, tempy) > val && type == MORPH_DILATE) ||
					(src.at<uchar>(tempx, tempy) < val && type == MORPH_ERODE))
					val = src.at<uchar>(tempx, tempy);
			}
			dst.at<uchar>(i, j) = val;
		}
		break;
	case MORPH_OPEN:
		Morphology(src, temp, element, MORPH_ERODE, anchor);
		Morphology(temp, dst, element, MORPH_DILATE, anchor);
		break;
	case MORPH_CLOSE:
		Morphology(src, temp, element, MORPH_DILATE, anchor);
		Morphology(temp, dst, element, MORPH_ERODE, anchor);
		break;
	case MORPH_GRADIENT:
		Morphology(src, dst, element, MORPH_DILATE, anchor);
		Morphology(src, temp, element, MORPH_ERODE, anchor);
		dst -= temp;
		break;
	case MORPH_TOPHAT:
		Morphology(src, temp, element, MORPH_OPEN, anchor);
		dst -= temp;
		break;
	case MORPH_BLACKHAT:
		Morphology(src, dst, element, MORPH_CLOSE, anchor);
		dst -= src;
		break;
	case MORPH_SMOOTH:
		Morphology(src, temp, element, MORPH_OPEN, anchor);
		Morphology(temp, dst, element, MORPH_CLOSE, anchor);
		break;
	}
}

Mat TexturalOpenCV(const Mat &img, int firstRad, int secondRad, int thirdRad)
{
	Mat des, one, two;

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(firstRad, firstRad));
	morphologyEx(img, des, MORPH_CLOSE, element);

	element = getStructuringElement(MORPH_ELLIPSE, Size(secondRad, secondRad));
	morphologyEx(des, des, MORPH_OPEN, element);

	element = getStructuringElement(MORPH_ELLIPSE, Size(thirdRad, thirdRad));
	morphologyEx(des, one, MORPH_DILATE, element);
	morphologyEx(des, two, MORPH_ERODE, element);
	multiply(img, (255 - one + two) / 255, des);
	return des;
}

Mat Textural(const Mat &img, int firstRad, int secondRad, int thirdRad)
{
	Mat des, one, two;

	Mat element = getStructuringElement(MORPH_ELLIPSE, Size(firstRad, firstRad));
	Morphology(img, des, element, MORPH_CLOSE);

	element = getStructuringElement(MORPH_ELLIPSE, Size(secondRad, secondRad));
	Morphology(des, des, element, MORPH_OPEN);

	element = getStructuringElement(MORPH_ELLIPSE, Size(thirdRad, thirdRad));
	Morphology(des, one, element, MORPH_DILATE);
	Morphology(des, two, element, MORPH_ERODE);
	multiply(img, (255 - one + two) / 255, des);
	return des;
}

void SmoothOperatorOpenCV(Mat src, Mat &des, const Mat &element, Point anchor)
{
	if (anchor.x < 0 || anchor.x >= element.rows || anchor.y < 0 || anchor.y >= element.cols)
	{
		anchor.x = element.rows / 2;
		anchor.y = element.cols / 2;
	}

	morphologyEx(src, des, MORPH_OPEN, element, anchor);
	morphologyEx(des, des, MORPH_CLOSE, element, anchor);
}

Mat GranulometryOpenCV(const Mat& img, int threshold)
{
	if (threshold < 4)
		return Mat();

	Mat des = Mat::zeros(1, threshold - 2, CV_32FC1), ele;
	int mul = img.rows*img.cols;
	for (int i = 3; i <= threshold; ++i)
	{
		ele = getStructuringElement(MORPH_ELLIPSE, Size(i, i));
		Mat tmp; morphologyEx(img, tmp, MORPH_OPEN, ele);
		float all; all = (float)sum(tmp).val[0];
		des.at<float>(0, i - 3) = all / mul;
	}

	Mat hist = Mat::zeros(1, threshold - 3, CV_32FC1);
	for (int i = threshold - 4; i >= 0; --i)
		hist.at<float>(0, i) = des.at<float>(0, i) - des.at<float>(0, i + 1);

	return hist;
}

Mat Granulometry(const Mat& img, int threshold)
{
	if (threshold < 4)
		return Mat();

	Mat des = Mat::zeros(1, threshold - 2, CV_32FC1), ele;
	int mul = img.rows*img.cols;
	for (int i = 3; i <= threshold; ++i)
	{
		ele = getStructuringElement(MORPH_ELLIPSE, Size(i, i));
		Mat tmp; Morphology(img, tmp, ele, MORPH_OPEN);
		float all; all = (float)sum(tmp).val[0];
		des.at<float>(0, i - 3) = all / mul;
	}

	Mat hist = Mat::zeros(1, threshold - 3, CV_32FC1);
	for (int i = threshold - 4; i >= 0; --i)
		hist.at<float>(0, i) = des.at<float>(0, i) - des.at<float>(0, i + 1);

	return hist;
}

Mat DilateRecOpenCV(const Mat& mask, const Mat& marker)
{
	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat tmp = marker.clone();

	double minn, maxx;
	while (true)
	{
		Mat dil; dilate(tmp, dil, ele);
		Mat des; min(mask, dil, des);

		minMaxLoc(des == tmp, &minn, &maxx);
		if (minn == 255)
			break;

		des.copyTo(tmp);
	}

	return tmp;
}

Mat DilateRec(const Mat& mask, const Mat& marker)
{
	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat tmp = marker.clone();

	double minn, maxx;
	while (true)
	{
		Mat dil; Morphology(tmp, dil, ele, MORPH_DILATE);
		Mat des; min(mask, dil, des);

		minMaxLoc(des == tmp, &minn, &maxx);
		if (minn == 255)
			break;

		des.copyTo(tmp);
	}

	return tmp;
}

Mat ErodeRecOpenCV(const Mat& mask, const Mat& marker)
{
	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat tmp = marker.clone();

	double minn, maxx;
	while (true)
	{
		Mat ero; erode(tmp, ero, ele);
		Mat des; max(mask, ero, des);

		minMaxLoc(des == tmp, &minn, &maxx);
		if (minn == 255)
			break;

		des.copyTo(tmp);
	}

	return tmp;
}

Mat ErodeRec(const Mat& mask, const Mat& marker)
{
	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat tmp = marker.clone();

	double minn, maxx;
	while (true)
	{
		Mat ero; Morphology(tmp, ero, ele, MORPH_ERODE);
		Mat des; max(mask, ero, des);

		minMaxLoc(des == tmp, &minn, &maxx);
		if (minn == 255)
			break;

		des.copyTo(tmp);
	}

	return tmp;
}

Mat OpenRecOpenCV(const Mat& img, int num)
{
	if (num <= 0)
		return Mat();

	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat ero = img.clone();

	for (int i = 0; i < num; ++i)
		erode(ero, ero, ele);

	return DilateRecOpenCV(img, ero);
}

Mat OpenRec(const Mat& img, int num)
{
	if (num <= 0)
		return Mat();

	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat ero = img.clone();

	for (int i = 0; i < num; ++i)
		Morphology(ero, ero, ele, MORPH_ERODE);

	return DilateRec(img, ero);
}

Mat CloseRecOpenCV(const Mat& img, int num)
{
	if (num <= 0)
		return Mat();

	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat dil = img.clone();

	for (int i = 0; i < num; ++i)
		dilate(dil, dil, ele);

	return ErodeRecOpenCV(img, dil);
}

Mat CloseRec(const Mat& img, int num)
{
	if (num <= 0)
		return Mat();

	Mat ele = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat dil = img.clone();

	for (int i = 0; i < num; ++i)
		Morphology(dil, dil, ele, MORPH_DILATE);

	return ErodeRec(img, dil);
}

Mat drawHist(Mat hist, int hist_w, int hist_h)
{
	int histSize = hist.cols;
	int bin_w = int(hist_w*1.0F / histSize + 0.5F);
	Mat pic(hist_h, hist_w, CV_8UC1, Scalar(255)), des;
	normalize(hist, des, 0, hist_h, NORM_MINMAX);

	for (int i = 1; i < histSize; ++i)
	{
		int one = int(des.at<float>(0, i - 1) + 0.5F),
			two = int(des.at<float>(0, i) + 0.5F);
		line(pic, Point(bin_w*(i - 1), hist_h - one), Point(bin_w*i, hist_h - two), Scalar(0), 2);
	}
	return pic;
}