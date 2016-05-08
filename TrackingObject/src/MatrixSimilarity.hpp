#ifndef MATRIXSIMILARITY_HPP
#define MATRIXSIMILARITY_HPP

#include <cfloat>
#include <opencv2/core/core.hpp>
#include <iostream>

namespace matrixSimilarity {

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
	MatrixSimilarity(int r, int c);

	/**
	 * Distruttore
	 */
	~MatrixSimilarity();

	/**
	 * Rimuove dalla matrice il blob e l'oggetto una volta fatta l'associazione
	 * Per rimozione si intende mettere a true il relativo blob/oggetto nel rispettivo valore
	 *
	 * @param blob
	 * @param obj
	 * @return risultato dell'operazione
	 */
	bool deleteFromMatrix(int blob, int obj);

	/**
	 * Calcolo della matrice di similarita'
	 *
	 * @param blobs Posizione dei blobs
	 * @param objs  Posizione degli oggetti
	 */
	void calculateMatrix(std::vector<cv::Point2f> blobs, std::vector<cv::Point2f> objs);

	/**
	 * Calcola il massimo della matrice di similarita'
	 * Nel caso si consideri un valore presente isBlobsDeleted o isObjectDeleted esso non viene considerato
	 *
	 * @return vettore di 3 dimensioni contenente la posizione riga,colonna e il valoreMAx
	 */
	std::vector<float> maxMatrix();

	/**
	 *
	 * @return Ritorna i blob di cui non e' stata fatta alcuna associazione
	 */
	std::vector<int> remainBlobs();

	/**
	 *
	 * @return Ritorna gli oggetti di cui non e' stata fatta alcuna associazione
	 */
	std::vector<int> remainObjects();

	/**
	 * Inizializza la matrice assegnando 0 a tutte le celle
	 */
	void fillMatrixReset();

	/**
	 * Overload dell'operatore << usato per stampare l'oggetto matrix,
	 * la funzione è standard per tutti gli overload <<
	 *
	 * @param os
	 * @param matrix
	 * @return ostream concatenato a matrix
	 */
	friend std::ostream &operator<<(std::ostream &os, const MatrixSimilarity &matrix);
};
}

#endif
