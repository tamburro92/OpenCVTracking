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
	associatedBlob.clear();
}

void Obj::associateBlob(vector<Point> b) {
	oldBlob=associatedBlob; // si assegna il blob corrente al vecchio blob prima dell'aggiornamento
	associatedBlob = b; // aggiornamento del blob corrente
}

void Obj::deassociateBlob(){
	associatedBlob.clear(); // viene eliminato il contenuto di associatedBlob
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
	return associatedBlob.empty()==false;
}

int Obj::getGhostFrame(){
	return ghostFrame;
}

int Obj::getName(){
	return name;
}
