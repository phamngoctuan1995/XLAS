#include "Header.h"

int main() {

	// Phan chung

	string s;  char t = 0; bool gray = 0, video = 0;
	cout << "Input the link: ";
	getline(cin, s);

	Mat img = imread(s, CV_LOAD_IMAGE_GRAYSCALE);

	if (img.data == NULL)
		return 1;
	
	LinhTinh(img);
	Mat img1 = readImg("file.txt", 1500);
	Mat mean;

	cout << img1.rows << " " << img1.cols;
	Mat tmp = PCA_CV(img1, mean, 700);
	cout << tmp.rows << " " << tmp.cols;
	Mat y = tmp.t() * img1;
	imshow("Image", img);
	Mat z = tmp * y;
	for (int i = 0; i < z.cols; ++i)
		z.col(i) += mean;
	cout << z.rows << "  " << z.cols << endl;
	writeImg("file.txt", z, 30, 50);

	waitKey(0);
	destroyAllWindows();
	return 0;
}