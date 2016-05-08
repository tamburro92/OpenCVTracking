#include <cfloat>
#include <opencv2/core/core.hpp>
#include <iostream>

using namespace cv;
using namespace std;
using std::vector;

#define DMAX 3 //Distanza MAX Algoritmo
#define MINVALUE -1000 //Valore minimo della matrice di similiraità

class MatrixSimilarity {
private:

public:

	int rows;
	int cols;
	float** matrix;
	std::vector<bool> isObjectDeleted;
	std::vector<bool> isBlobsDeleted;

	/**
	 * Costruttore di @class MatrixSimilarity
	 *
	 * @param r numero di Blobs
	 * @param c numero di Oggetti
	 */
	MatrixSimilarity(int r, int c) {
		rows = r;
		cols = c;
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

	/**
	 * Distruttore
	 */
	~MatrixSimilarity() {
		for (int i = 0; i < rows; ++i) {
			delete[] matrix[i];
		}
		delete[] matrix;
		rows = 0;
		cols = 0;
	}

	/**
	 * Rimuove dalla matrice il blob e l'oggetto una volta fatta l'associazione
	 * Per rimozione si intende mettere a true il relativo blob/oggetto nel rispettivo valore
	 *
	 * @param blob
	 * @param obj
	 * @return risultato dell'operazione
	 */
	bool deleteFromMatrix(int blob, int obj) {
		if (blob > rows || obj > cols) {
			return false;
		}
		isBlobsDeleted[blob] = true;
		isObjectDeleted[obj] = true;
		return true;

	}

	/**
	 * Calcolo della matrice di similarita'
	 *
	 * @param blobs Posizione dei blobs
	 * @param objs  Posizione degli oggetti
	 */
	void calculateMatrix(std::vector<Point2f> blobs,
			std::vector<Point2f> objs) {
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				if (cv::norm(blobs[i] - objs[j]) < DMAX)
					matrix[i][j] = 1 - cv::norm(blobs[i] - objs[j]) / DMAX;

	}

	/**
	 * Calcola il massimo della matrice di similarita'
	 * Nel caso si consideri un valore presente isBlobsDeleted o isObjectDeleted esso non viene considerato
	 *
	 * @return vettore di 3 dimensioni contenente la posizione riga,colonna e il valoreMAx
	 */
	std::vector<float> maxMatrix() {
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

	/**
	 *
	 * @return Ritorna i blob di cui non e' stata fatta alcuna associazione
	 */
	std::vector<int> remainBlobs() {
		std::vector<int> blobs;
		for (int i = 0; i < rows; i++)
			if (!isBlobsDeleted[i])
				blobs.push_back(i);

		return blobs;
	}

	/**
	 *
	 * @return Ritorna gli oggetti di cui non e' stata fatta alcuna associazione
	 */
	std::vector<int> remainObjects() {
		std::vector<int> obj;
		for (int i = 0; i < cols; i++)
			if (!isObjectDeleted[i])
				obj.push_back(i);

		return obj;
	}

	/**
	 * Inizializza la matrice assegnando 0 a tutte le celle
	 */
	void fillMatrixReset() {
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				matrix[i][j] = MINVALUE;

	}

	/**
	 * Overload dell'operatore << usato per stampare l'oggetto matrix,
	 * la funzione è standard per tutti gli overload <<
	 *
	 *
	 * @param os
	 * @param matrix
	 * @return ostream concatenato a matrix
	 */
	friend ostream &operator<<(ostream &os, const MatrixSimilarity &matrix) {
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
};


