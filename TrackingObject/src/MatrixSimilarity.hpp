#include <cfloat>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace cv;
using namespace std;
using std::vector;

#define DMAX 3
#define MINVALUE -1000
class MatrixSimilarity {
private:
public:

	int rows;
	int cols;
	float** matrix;
	std::vector<bool> isObjectDeleted;
	std::vector<bool> isBlobsDeleted;

	MatrixSimilarity(int r, int c) {
		rows = r;
		cols = c;
		matrix = new float*[rows];
		for (int i = 0; i < rows; i++)
			matrix[i] = new float[cols];

		for (int i = 0; i < rows; isBlobsDeleted.push_back(false), i++)
			;
		for (int i = 0; i < cols; isObjectDeleted.push_back(false), i++)
			;
		fillMatrixReset();
	}
	~MatrixSimilarity() {
		for (int i = 0; i < rows; ++i)
			delete[] matrix[i];
		delete[] matrix;
		rows = 0;
		cols = 0;
	}

	bool deleteFromMatrix(int blob, int obj) {
		if (blob > rows || obj > cols)
			return false;
		isBlobsDeleted[blob] = true;
		isObjectDeleted[obj] = true;
		return true;

	}
	void calculateMatrix(std::vector<Point2f> blobs,
			std::vector<Point2f> objs) {
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				if (cv::norm(blobs[i] - objs[j]) < DMAX)
					matrix[i][j] = 1 - cv::norm(blobs[i] - objs[j]) / DMAX;

	}
	std::vector<float> maxMatrix() {
		std::vector<float> maxPoint;

		float maxValue = MINVALUE;
		int rowMax = -1;
		int colMax = -1;
		for (int i = 0; i < rows; i++) {
			if (isBlobsDeleted[i])
				continue;
			for (int j = 0; j < cols; j++) {
				if (isObjectDeleted[j])
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
	std::vector<int> remainBlobs() {
		std::vector<int> blobs;
		for (int i = 0; i < rows; i++)
			if (!isBlobsDeleted[i])
				blobs.push_back(i);

		return blobs;
	}
	std::vector<int> remainObjects() {
		std::vector<int> obj;
		for (int i = 0; i < cols; i++)
			if (!isObjectDeleted[i])
				obj.push_back(i);

		return obj;
	}
	void fillMatrixReset() {
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				matrix[i][j] = 0;

	}
};

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
