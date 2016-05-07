#include <cfloat>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace cv;
using namespace std;
using std::vector;

#define DMAX 3
class MatrixSimilarity {
private:
public:

	int rows;
	int cols;
	float** matrix;

	MatrixSimilarity(int r, int c) {
		rows = r;
		cols = c;
		matrix = new float*[rows];
		for (int i = 0; i < rows; i++)
			matrix[i] = new float[cols];
		fillMatrixReset();
	}
	~MatrixSimilarity() {
		for (int i = 0; i < rows; ++i)
			delete[] matrix[i];
		delete[] matrix;
		rows = 0;
		cols = 0;
	}

	bool deleteFromMatrix(int r, int c) {
		if (r > rows || c > cols)
			return false;
		for (int i = 0; i < rows; i++) {
			matrix[i][c] = FLT_MIN;
		}
		for (int i = 0; i < cols; i++) {
			matrix[r][i] = FLT_MIN;
		}
		return true;

	}
	void calculateMatrix(std::vector<Point2f> blobs, std::vector<Point2f> objs) {
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					if(cv::norm(blobs[i]-objs[j])<DMAX)
					     matrix[i][j] = 1-cv::norm(blobs[i]-objs[j])/DMAX;


		}
	std::vector<float> maxMatrix() {
		std::vector<float> maxPoint;

		float maxValue = FLT_MIN;
		int rowMax = -1;
		int colMax = -1;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (matrix[i][j] > maxValue) {
					maxValue = matrix[i][j];
					rowMax = i;
					colMax = j;
				}

			}
		}
		maxPoint.push_back(maxValue);
		maxPoint.push_back(rowMax);
		maxPoint.push_back(colMax);
		return maxPoint;
	}
	std::vector<int> remainBlobs() {
		std::vector<int> blobs;
		bool deleted = false;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				if (matrix[i][j] == FLT_MIN)
					deleted = true;
			}
			if (!deleted)
				blobs.push_back(i);
			deleted = false;

		}
		return blobs;
	}
	std::vector<int> remainObjects() {
		std::vector<int> blobs;
		bool deleted = false;
		for (int j = 0; j < rows; j++) {
			for (int i = 0; i < cols; i++) {
				if (matrix[i][j] == FLT_MIN)
					deleted = true;
			}
			if (!deleted)
				blobs.push_back(j);
			deleted = false;

		}
		return blobs;
	}
	void fillMatrixReset() {
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				matrix[i][j] = 0;

	}
};
