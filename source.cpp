#define _CRT_SECURE_NO_WARNINGS
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv/cv.h>
#include <windows.h>
#include <opencv2/opencv.hpp>
using namespace cv;

#include <fstream>
#include <string>
#include <algorithm>

using namespace std;


int n = 1080, m = 960, pix;
int d = 33; //distance between lines
int nums = 8; //nums of duplicate frames;
double fontScale = 1.9;
vector<vector<bool>> mp;
int q = 10; // cnt of squares disappearing per sec


void replaceAll(string &s, string x, string y) {
	int index = 0;
	while (index < s.size()) {
		index = s.find(x, index);
		if (index == -1) break;

		s.erase(s.begin() + index, s.begin() + index + x.size());

		s.insert(index, y);

		index += y.size();
	}
	return;
}

void rsize() {
	mp.resize(n / pix + 1);
	for (size_t i = 0; i < mp.size(); i++) {
		mp[i].assign(m / pix + 1, true);
	}
}

int main() {
	freopen("text.txt", "r", stdin);

	string line;

	Mat image;
	image = imread("image.jpg", 1);
	if (!image.data) return -1;

	n = image.cols, m = image.rows;
	cin >> pix;

	rsize();

	int k = 0;
	while (getline(cin, line)) {
		replaceAll(line, "\"", "\\\"");
		replaceAll(line, "\t", "    ");
		if (line.empty()) {
			k += d / 2;
			continue;
		}
		line = "      " + line;
		putText(image, line, Point(5, 10 + k), FONT_HERSHEY_PLAIN, fontScale, Scalar(0, 0, 0), 2, 4); //4th - fontFace, 5th - fontScale
		k += d;
	}
	
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compression_params.push_back(95);

	imwrite("out.jpg", image, compression_params);
	int blured = (n / pix + 1) * (m / pix + 1);

	int cnt = 0;

	vector<Mat> frames;

	while (blured > 0) {
		Mat curimage = imread("out.jpg", 1);
		vector<pair<int, int>> active;
		for (int i = 0; i < (int)mp.size(); i++) {
			for (int j = 0; j < (int)mp[i].size(); j++) {
				if (mp[i][j]) {
					int h = min(pix - 1, n - i * pix), w = min(pix - 1, m - j * pix);
					Rect region(i * pix, j * pix, h, w);
					GaussianBlur(curimage(region), curimage(region), Size(0, 0), 10);
					active.push_back({ i, j });
				}
			}
		}
		for (int i = 0; i < nums; i++) 	frames.push_back(curimage);

		random_shuffle(active.begin(), active.end());

		for (int i = 0; i < min(q, (int)active.size()); i++) {
			int x = active[i].first, y = active[i].second;
			mp[x][y] = 0;
		}
		blured -= q; cnt++;
	}

	Mat curimage = imread("out.jpg", 1);
	frames.push_back(curimage);
	VideoWriter writer("output.avi", CV_FOURCC('M', 'J', 'P', 'G'), 10.0, Size(n, m));

	for (auto frame : frames) {
		assert(m == frame.rows && n == frame.cols);
		writer.write(frame);
	}

	writer.release();
	destroyAllWindows();
	return 0;
}