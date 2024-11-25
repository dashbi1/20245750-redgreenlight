#include<opencv2/opencv.hpp>

#include<iostream>
#include<string>

#include <windows.h>
#include <psapi.h>

#define SHOW 15

using namespace cv;
using namespace std;


/*
void printMemoryUsage() {
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE hProcess = GetCurrentProcess();
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
		cout << "Memory usage (in bytes): " << pmc.WorkingSetSize << endl;
	}
	else {
		cerr << "Unable to retrieve memory info." << endl;
	}
}

*/



string path = "C:\\D\works\\ai\\trafficlight_ide\\color-main\\video\\move_green.avi";


Mat kernel_dilation = Mat::ones({ 7,7 }, CV_8UC1);
Mat kernel_erode = Mat::ones({ 3,3 }, CV_8UC1);
bool arr[3] = {};

int main() {
	
	//cout << path << endl;
	VideoCapture cap;
	bool cc=cap.open(0);
	cout << cc << endl;
	int num = 0;
	if (!cap.isOpened()) {
		cout << "video not open.\n";
		return -1;
	}
	
	while (1) {
		arr[0] = 0; arr[1] = 0; arr[2] = 0;
		num++;
		//cout << num << endl << endl << endl;
		Mat frame;
		bool ret = cap.read(frame);
		if (!ret) {
			break;
		}

		//resize(frame, frame, { 1920,1080 });
		Mat f2 = frame.clone();
		Mat hsv;
		cvtColor(frame, hsv, COLOR_BGR2HSV);


		//get亮点
		Vec3b lower_v{ 0,0,252 };
		Vec3b upper_v{ 255,255,255 };

		Mat mask_v;
		inRange(hsv, lower_v, upper_v, mask_v);
		medianBlur(mask_v, mask_v, 7);



		erode(mask_v, mask_v, kernel_erode, { -1,-1 },2);
		dilate(mask_v, mask_v, kernel_dilation, { -1,-1 }, 4);
//


		//red识别周围的红色
		Vec3b lower_hsv_red{ 167,230,170 };
		Vec3b upper_hsv_red{ 179,255,255 };
		Mat mask_red;
		inRange(hsv, lower_hsv_red, upper_hsv_red, mask_red);
		Mat red_blur;
		medianBlur(mask_red, red_blur, 7);
		//erode(mask_v, mask_v, kernel_erode, { -1,-1 },4);
		dilate(red_blur, red_blur, kernel_dilation, { -1,-1 }, 4);
		bitwise_and(red_blur, mask_v, red_blur);
		



		//yellow识别周围的
		Vec3b lower_hsv_yellow{ 8,230,170 };
		Vec3b upper_hsv_yellow{ 20,255,255 };
		Mat mask_yellow;
		inRange(hsv, lower_hsv_yellow, upper_hsv_yellow, mask_yellow);
		Mat yellow_blur;
		medianBlur(mask_yellow, yellow_blur, 7);
		dilate(yellow_blur, yellow_blur, kernel_dilation, { -1,-1 }, 4);
		bitwise_and(yellow_blur, mask_v, yellow_blur);

		//green识别周围的
		Vec3b lower_hsv_green{ 45,205,120 };
		Vec3b upper_hsv_green{ 90,255,255 };
		Mat mask_green;
		inRange(hsv, lower_hsv_green, upper_hsv_green, mask_green);
		Mat green_blur;
		medianBlur(mask_green, green_blur, 7);
		dilate(green_blur, green_blur, kernel_dilation, { -1,-1 }, 4);
		bitwise_and(green_blur, mask_v, green_blur);


		vector<vector<Point>>contours1;
		vector<Vec4i>hierarchy1;
		findContours(green_blur, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		vector<vector<Point>>contours2;
		vector<Vec4i>hierarchy2;
		findContours(red_blur, contours2, hierarchy2, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		vector<vector<Point>>contours3;
		vector<Vec4i>hierarchy3;
		findContours(yellow_blur, contours3, hierarchy3, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		int x, y, w, h;
		Rect rect;
		

		//cout << contours1.size() << endl << contours2.size() << endl << contours3.size() << endl;

		for (int i = 0; i < contours1.size(); i++) {
			if (contours1.size() == 0) { break; }
			rect = boundingRect(contours1[i]);
			x = rect.x;
			y = rect.y;
			w = rect.width;
			h = rect.height;
			if (w * h < SHOW*SHOW) { continue; }
			arr[0] |= 1;
			rectangle(f2, rect, { 255,255,255 }, 2);
			putText(f2, "green_light", { x,y - 5 }, FONT_HERSHEY_SIMPLEX, 0.6, { 0,255,0 }, 2);


		}

		for (int i = 0; i < contours2.size(); i++) {
			if (contours2.size() == 0) { break; }
			rect = boundingRect(contours2[i]);
			x = rect.x;
			y = rect.y;
			w = rect.width;
			h = rect.height;
			if (w * h < SHOW*SHOW) { continue; }
			arr[1] |= 1;
			rectangle(f2, rect, { 255,0,0 }, 2);
			putText(f2, "red_light", { x,y - 5 }, FONT_HERSHEY_SIMPLEX, 0.6, { 0,0,255 }, 2);


		}

		for (int i = 0; i < contours3.size(); i++) {
			if (contours3.size() == 0) { break; }
			rect = boundingRect(contours3[i]);
			x = rect.x;
			y = rect.y;
			w = rect.width;
			h = rect.height;
			if (w * h < SHOW*SHOW) { continue; }
			arr[2] |= 1;
			rectangle(f2, rect, { 255,0,255 }, 2);
			putText(f2, "yellow_light", { x,y - 5 }, FONT_HERSHEY_SIMPLEX, 0.6, { 0,255,255 }, 2);


		}

		if (arr[0]) {
			putText(f2, "GREEN", { 20,20 },FONT_HERSHEY_SIMPLEX,0.6,{ 0, 255, 0 }, 2);
		}
		if (arr[1]) {
			putText(f2, "RED", { 20,60}, FONT_HERSHEY_SIMPLEX, 0.6, { 0,0,255 }, 2);
		}
		if (arr[2]) {
			putText(f2, "YELLOW", { 20,100 }, FONT_HERSHEY_SIMPLEX, 0.6, { 0,255,255 }, 2);
		}
		if (!(arr[1] || arr[0])) {
			putText(f2, "NOT", { 20,140 }, FONT_HERSHEY_SIMPLEX, 0.6, { 0,255,255 }, 2);
		}


		imshow("frame", f2);
		resize(red_blur, red_blur, { 300,200 });
		resize(green_blur, green_blur, { 300,200 });
		resize(yellow_blur, yellow_blur, { 300,200 });
		resize(mask_v, mask_v, { 300,200 });

		imshow("red_masked", red_blur);
		imshow("green_masked", green_blur);
		imshow("yellow_masked", yellow_blur);
		imshow("mask_v", mask_v);
		//printMemoryUsage();

		char c = waitKey(10);
		if (c == 27) {
			break;
		}
		
		
	}







	return 0;
}











/*

#include<opencv2/opencv.hpp>

#include<iostream>
#include<string>

#include <windows.h>
#include <psapi.h>



using namespace cv;
using namespace std;



void printMemoryUsage() {
	PROCESS_MEMORY_COUNTERS pmc;
	HANDLE hProcess = GetCurrentProcess();
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
		cout << "Memory usage (in bytes): " << pmc.WorkingSetSize << endl;
	}
	else {
		cerr << "Unable to retrieve memory info." << endl;
	}
}





string path = "C:\\D\works\\ai\\trafficlight_ide\\color-main\\video\\move_green.avi";

int main() {
	cout << path << endl;
	VideoCapture cap;
	bool cc = cap.open(0);
	cout << cc << endl;
	int num = 0;
	if (!cap.isOpened()) {
		cout << "video not open.\n";
		return -1;
	}

	while (1) {
		num++;
		//cout << num << endl << endl << endl;
		Mat frame;
		bool ret = cap.read(frame);
		if (!ret) {
			break;
		}

		resize(frame, frame, { 1920,1080 });
		Mat f2 = frame.clone();
		Mat hsv;
		cvtColor(frame, hsv, COLOR_BGR2HSV);


		//get亮点
		Vec3b lower_v{ 0,0,250 };
		Vec3b upper_v{ 255,255,255 };

		Mat mask_v;
		inRange(hsv, lower_v, upper_v, mask_v);
		medianBlur(mask_v, mask_v, 7);


		Mat kernel_dilation = Mat::ones({ 7,7 }, CV_8UC1);
		dilate(mask_v, mask_v, kernel_dilation, { -1,-1 }, 4);



		//red识别周围的红色
		Vec3b lower_hsv_red{ 168,230,170 };
		Vec3b upper_hsv_red{ 179,255,255 };
		Mat mask_red;
		inRange(hsv, lower_hsv_red, upper_hsv_red, mask_red);
		Mat red_blur;
		medianBlur(mask_red, red_blur, 7);
		bitwise_and(red_blur, mask_v, red_blur);

		//yellow识别周围的红色
		Vec3b lower_hsv_yellow{ 8,230,170 };
		Vec3b upper_hsv_yellow{ 20,255,255 };
		Mat mask_yellow;
		inRange(hsv, lower_hsv_yellow, upper_hsv_yellow, mask_yellow);
		Mat yellow_blur;
		medianBlur(mask_yellow, yellow_blur, 7);
		bitwise_and(yellow_blur, mask_v, yellow_blur);

		//green识别周围的红色
		Vec3b lower_hsv_green{ 45,205,120 };
		Vec3b upper_hsv_green{ 90,255,255 };
		Mat mask_green;
		inRange(hsv, lower_hsv_green, upper_hsv_green, mask_green);
		Mat green_blur;
		medianBlur(mask_green, green_blur, 7);
		bitwise_and(green_blur, mask_v, green_blur);


		vector<vector<Point>>contours1;
		vector<Vec4i>hierarchy1;
		findContours(green_blur, contours1, hierarchy1, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		vector<vector<Point>>contours2;
		vector<Vec4i>hierarchy2;
		findContours(red_blur, contours2, hierarchy2, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		vector<vector<Point>>contours3;
		vector<Vec4i>hierarchy3;
		findContours(yellow_blur, contours3, hierarchy3, RETR_EXTERNAL, CHAIN_APPROX_NONE);

		int x, y, w, h;
		Rect rect;


		//cout << contours1.size() << endl << contours2.size() << endl << contours3.size() << endl;

		for (int i = 0; i < contours1.size(); i++) {
			if (contours1.size() == 0) { break; }
			rect = boundingRect(contours1[i]);
			x = rect.x;
			y = rect.y;
			w = rect.width;
			h = rect.height;
			if (w * h < 50 * 50) { continue; }
			rectangle(f2, rect, { 255,255,255 }, 2);
			putText(f2, "green_light", { x,y - 5 }, FONT_HERSHEY_SIMPLEX, 0.6, { 0,255,0 }, 2);


		}

		for (int i = 0; i < contours2.size(); i++) {
			if (contours2.size() == 0) { break; }
			rect = boundingRect(contours2[i]);
			x = rect.x;
			y = rect.y;
			w = rect.width;
			h = rect.height;
			if (w * h < 50 * 50) { continue; }
			rectangle(f2, rect, { 255,0,0 }, 2);
			putText(f2, "red_light", { x,y - 5 }, FONT_HERSHEY_SIMPLEX, 0.6, { 0,0,255 }, 2);


		}

		for (int i = 0; i < contours3.size(); i++) {
			if (contours3.size() == 0) { break; }
			rect = boundingRect(contours3[i]);
			x = rect.x;
			y = rect.y;
			w = rect.width;
			h = rect.height;
			if (w * h < 50 * 50) { continue; }
			rectangle(f2, rect, { 255,0,255 }, 2);
			putText(f2, "yellow_light", { x,y - 5 }, FONT_HERSHEY_SIMPLEX, 0.6, { 0,255,255 }, 2);


		}


		imshow("frame", f2);
		resize(red_blur, red_blur, { 300,200 });
		resize(green_blur, green_blur, { 300,200 });
		resize(yellow_blur, yellow_blur, { 300,200 });
		resize(mask_v, mask_v, { 300,200 });

		imshow("red_masked", red_blur);
		imshow("green_masked", green_blur);
		imshow("yellow_masked", yellow_blur);
		imshow("mask_v", mask_v);
		printMemoryUsage();

		char c = waitKey(10);
		if (c == 27) {
			break;
		}


	}







	return 0;
}




*/