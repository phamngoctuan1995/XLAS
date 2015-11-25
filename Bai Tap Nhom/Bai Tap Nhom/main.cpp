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

	waitKey(0);
	destroyAllWindows();
	return 0;
}