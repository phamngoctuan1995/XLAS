#include "Header.h"

void Morphology(const Mat &src, Mat &dst, const Mat &element, int type, Point anchor)
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
	case MORPH_TEXTUAL_SEGMENTATION:
		Morphology(src, temp, element, MORPH_CLOSE, anchor);
		Morphology(temp, dst, element, MORPH_OPEN, anchor);
		break;
	}
}

void Skeleton(const Mat &src, Mat &dst, const Mat &element)
{
	double min, max;
	Mat ero;
	cv::normalize(src, ero, 0, 1, cv::NORM_MINMAX);
	Mat open;
	morphologyEx(ero, open, MORPH_OPEN, element);
	open = 1 - open;
	dst = ero & open;

	while (true)
	{
		erode(ero, ero, element);
		minMaxLoc(ero, &min, &max);
		if (max == 0)
			break;
		morphologyEx(ero, open, MORPH_OPEN, element);
		open = 1 - open;
		dst = dst | (ero & open);
	}
}

void hitmiss(Mat& src, Mat& dst, const Mat& kernel)
{
	CV_Assert(src.type() == CV_8U && src.channels() == 1);

	cv::Mat k1 = (kernel == 1) / 255;
	cv::Mat k2 = (kernel == -1) / 255;

	cv::normalize(src, src, 0, 1, cv::NORM_MINMAX);

	cv::Mat e1, e2;
	cv::erode(src, e1, k1);
	cv::erode(1 - src, e2, k2);
	dst = e1 & e2;
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

void TextualSegmentationOpenCV(Mat src, Mat &des, const Mat &element, Point anchor)
{
	if (anchor.x < 0 || anchor.x >= element.rows || anchor.y < 0 || anchor.y >= element.cols)
	{
		anchor.x = element.rows / 2;
		anchor.y = element.cols / 2;
	}

	morphologyEx(src, des, MORPH_CLOSE, element, anchor);
	morphologyEx(des, des, MORPH_OPEN, element, anchor);
}