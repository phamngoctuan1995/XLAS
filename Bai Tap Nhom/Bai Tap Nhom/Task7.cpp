#include "Header.h"
#include <algorithm>

vector<int> freq;

bool Compare(int a, int b)
{
	return freq[a] > freq[b];
}

Mat KMeans_RIP(const Mat& img, int k, vector<Point3f>& meanReg)
{
	Mat reg = Mat::zeros(img.rows, img.cols, CV_8UC1);

	vector<Point3f> means; double oldSum = 0;
	for (int i = 0; i < k; ++i)
	{
		int r, c; bool flag = true;
		while (flag)
		{
			flag = false;
			r = rand() % img.rows; c = rand() % img.cols;
			Point3f item(r, c, img.at<uchar>(r, c));
			for (int j = means.size() - 1; j >= 0; --j)
			if (flag = (item == means[j]))
				break;
		}
		means.push_back(Point3f(r, c, img.at<uchar>(r, c)));
	}

	int epoch = 0;
	while (true)
	{
		vector<Point3f> tmp(k, Point3f(0, 0, 0));
		vector<int> count(k, 0); double sum = 0;
		for (int i = 0; i < img.rows; ++i)
		for (int j = 0; j < img.cols; ++j)
		{
			double minn = 10.0, minPos;
			for (int l = 0; l < k; ++l)
			{
				double xd = (i - means[l].x) / img.rows, yd = (j - means[l].y) / img.cols,
					zd = (img.at<uchar>(i, j) - means[l].z) / 255, d = xd*xd + yd*yd + zd*zd;
				if (minn > d)
				{
					minn = d;
					minPos = l;
				}
			}
			sum += minn;
			reg.at<uchar>(i, j) = 255 / k * minPos;
			++count[minPos];
			tmp[minPos].x += i;
			tmp[minPos].y += j;
			tmp[minPos].z += img.at<uchar>(i, j);
		}

		/*imshow("reg", reg);
		waitKey(0);*/

		for (int i = 0; i < k; ++i)
		{
			means[i].x = tmp[i].x / count[i];
			means[i].y = tmp[i].y / count[i];
			means[i].z = tmp[i].z / count[i];
		}

		sum /= img.rows*img.cols;
		if (abs(sum - oldSum) < 0.00001) //
			break;
		//cout << epoch << " " << sum - oldSum << endl;
		oldSum = sum;
		++epoch;
	}

	//cout << epoch << endl;
	meanReg = means;
	return reg;
}

Mat RegionGrowing(const Mat& img, float threshold)
{
	int k = 3; vector<int> maxx;
	freq = vector<int>(256, 0);
	for (int i = 0; i < img.rows; ++i)
	for (int j = 0; j < img.cols; ++j)
		freq[img.at<uchar>(i, j)]++;
	vector<int> id(256, 0);
	for (int i = 0; i < 256; ++i)
		id[i] = i;
	sort(id.begin(), id.end(), Compare);
	maxx = vector<int>(id.begin(), id.begin() + k);

	Mat reg = Mat::zeros(img.rows, img.cols, CV_8UC1), res;
	int count = 0;
	vector<Point2f> list_mid;
	vector<Point3f> list_count;
	for (int l = 0; l < k; ++l)
	{
		list_mid.push_back(Point2f(maxx[l], 0));
		for (int i = 0; i < img.rows; ++i)
		for (int j = 0; j < img.cols; ++j)
		if (img.at<uchar>(i, j) == maxx[l])
		{
			reg.at<uchar>(i, j) = list_mid.size();
			++count;
		}
		list_count.push_back(Point3f(count, 1, 0));
		count = 0;
	}

	Mat tmp = reg.clone();
	while (true)
	{
		//imshow("reg", reg * (255 / list_mid.size()));
		//waitKey(0);

		for (int i = 0; i < img.rows; ++i)
		for (int j = 0; j < img.cols; ++j)
		{
			int curval = reg.at<uchar>(i, j); --curval;
			if (curval == -1 || list_count[curval].y == 0)
				continue;

			float& siz = list_count[curval].x;
			float oldSiz = siz;
			Point2f& poi = list_mid[curval];
			for (int k = 0; k < 8; ++k)
			{
				int posY = i + nei[k][0],
					posX = j + nei[k][1];
				if (posX < 0 || posX >= img.cols || posY < 0 || posY >= img.rows)
					continue;

				if (reg.at<uchar>(posY, posX) == 0)
				{
					double val = img.at<uchar>(posY, posX);
					if (abs(val - poi.x) < threshold) //
					{
						poi.y += poi.x*poi.x;
						poi.x *= siz;
						poi.x += val;
						poi.x /= siz + 1;
						poi.y *= siz;
						poi.y += val*val;
						poi.y /= siz + 1;
						poi.y -= poi.x*poi.x;
						++siz; tmp.at<uchar>(posY, posX) = curval + 1;
					}
				}
			}

			if (siz != oldSiz)
				list_count[curval].z = 1;
		}

		reg = tmp;

		bool flag = false;
		for (int i = list_mid.size() - 1; i >= 0; --i)
		{
			flag = (flag || list_count[i].z == 1);
			list_count[i].y = list_count[i].z;
			list_count[i].z = 0;
		}

		if (!flag || count >= 1000) //
			break;
		++count;
	}

	reg *= 255 / list_mid.size();
	//reg = 255 - reg;
	//cout << count << endl;
	return reg;
}