#include "MatrixSimilarity.hpp"
#include <opencv2/core/core.hpp>
#include <iostream>
#include "Obj.hpp"

using namespace cv;
using namespace std;
using std::vector;

MatrixSimilarity::MatrixSimilarity(int r, int c) :
		rows(r), cols(c) {
	matrix = new float*[rows];
	for (int i = 0; i < rows; i++) {
		matrix[i] = new float[cols];
	}

	// push_back aggiunge un nuovo elemento alla fine del vettore
	for (int i = 0; i < rows; isBlobsDeleted.push_back(false), i++)
		;
	for (int i = 0; i < cols; isObjectDeleted.push_back(false), i++)
		;
	fillMatrixReset();
}

MatrixSimilarity::~MatrixSimilarity() {
	for (int i = 0; i < rows; ++i) {
		delete[] matrix[i];
	}
	delete[] matrix;
	rows = 0;
	cols = 0;
}

bool MatrixSimilarity::deleteFromMatrix(int blob, int obj) {
	if (blob > rows || obj > cols) {
		return false;
	}
	isBlobsDeleted[blob] = true;
	isObjectDeleted[obj] = true;
	return true;

}

void MatrixSimilarity::calculateMatrix(vector<Obj>& oggetti,
		vector<vector<Point>>& blobs) {
	for (int i = 0; i < blobs.size(); i++) {
		for (int j = 0; j < oggetti.size(); j++) {
			Point2f cBlob = centerBlob(blobs[i]);
			Point2f cObj = centerBlob(oggetti[j].getOldBlob());
			if (cv::norm(cBlob - cObj) < DMAX)
				matrix[i][j] = 1 - cv::norm(cBlob - cObj) / DMAX;
			else
				matrix[i][j] = 0;

		}

	}
}

std::vector<float> MatrixSimilarity::maxMatrix() {
	std::vector<float> maxPoint; // contiene: riga del valore massimo, colonna del valore massimo, valore massimo

	float maxValue = MINVALUE;
	int rowMax = -1;
	int colMax = -1;
	for (int i = 0; i < rows; i++) {
		if (isBlobsDeleted[i]) // il blob e' stato gia' associato
			continue;
		for (int j = 0; j < cols; j++) {
			if (isObjectDeleted[j]) // l'oggetto e' stato gia' associato
				continue;
			if (matrix[i][j] > maxValue) {
				maxValue = matrix[i][j];
				rowMax = i;
				colMax = j;
			}

		}
	}
	maxPoint.push_back(rowMax);
	maxPoint.push_back(colMax);
	maxPoint.push_back(maxValue);
	return maxPoint;
}

std::vector<int> MatrixSimilarity::remainBlobs() {
	std::vector<int> blobs;
	for (int i = 0; i < rows; i++)
		if (!isBlobsDeleted[i])
			blobs.push_back(i);

	return blobs;
}

std::vector<int> MatrixSimilarity::remainObjects() {
	std::vector<int> obj;
	for (int i = 0; i < cols; i++)
		if (!isObjectDeleted[i])
			obj.push_back(i);

	return obj;
}

void MatrixSimilarity::fillMatrixReset() {
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			matrix[i][j] = MINVALUE;

}

Point2f centerBlob(vector<Point> blob) {
	Point2f center;
	vector<vector<Point> > contours_poly;
	float radius;
	approxPolyDP(Mat(blob), contours_poly, 3, true); //approssima il contorno in un polinomio, il 3 indica l'accuratezza dell'approssimazione, true indica che la linea e' chiusa
	minEnclosingCircle((Mat) contours_poly, center, radius); // realizza un cerchio, vengono passati i punti, il centro, il raggio
	return center;
}

ostream &operator<<(ostream &os, const MatrixSimilarity &matrix) {
	for (int i = 0; i < matrix.rows; i++) {
		for (int j = 0; j < matrix.cols; j++) {
			if (matrix.isBlobsDeleted[i] || matrix.isObjectDeleted[j])
				os << "D" << " ";
			else
				os << matrix.matrix[i][j] << " ";
		}
		os << endl;
	}
	return os;
}

