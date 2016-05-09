#include "Obj.hpp"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

using namespace cv;
using namespace std;
using std::vector;

Obj::Obj(int name) :
		name(name) {
	ghostFrame = 0;
	blob.clear();
}

void Obj::associateBlob(vector<Point> b) {
	blob = b;
}

void Obj::deassociateBlob(){
	blob.clear();
}

int Obj::incremGhostFrame() {
	ghostFrame++;
	return ghostFrame;
}

int Obj::resetGhostFrame() {
	ghostFrame = 0;
	return ghostFrame;
}

bool Obj::isAssociated(){
	return blob.empty()==true;
}

int Obj::getGhostFrame(){
	return ghostFrame;
}

int Obj::getName(){
	return name;
}
