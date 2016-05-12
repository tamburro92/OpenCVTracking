#include "Obj.hpp"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include "MatrixSimilarity.hpp"

using namespace cv;
using namespace std;
using std::vector;

Obj::Obj(int name) :
		name(name), ghostFrame(0), toDelete(false)  {
	associatedBlob.clear();
	RNG rng(rand());
	color.push_back(rng.uniform(0, 255));
	color.push_back(rng.uniform(0, 255));
	color.push_back(rng.uniform(0, 255));
}

void Obj::associateBlob(vector<Point> b) {
	oldBlob = b; // si assegna il blob corrente al vecchio blob prima dell'aggiornamento
	associatedBlob = b; // aggiornamento del blob corrente
	positions.push_back(centerBlob(b));
}

void Obj::deassociateBlob() {
	//oldBlob = associatedBlob; //mantengo come ultimo blob il piu' recente
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

bool Obj::isAssociated() {
	return associatedBlob.empty() == false;
}

int Obj::getGhostFrame() {
	return ghostFrame;
}

int Obj::getName() {
	return name;
}
vector<Point> Obj::getOldBlob() {
	return oldBlob;
}

vector<Point> Obj::getAssociatedBlob() {
	return associatedBlob;
}

void Obj::setOldBlob(vector<Point> b) {
	oldBlob = b;
}
void Obj::setToDelete(bool b) {
	toDelete = b;
}
bool Obj::getToDelete() {
	return toDelete;
}

vector<Point> Obj::getPositions(){
	return positions;
}

vector<int> Obj::getColor(){
	return color;
}

