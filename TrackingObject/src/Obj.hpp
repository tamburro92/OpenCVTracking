#ifndef OBJ_HPP_
#define OBJ_HPP_

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

using namespace cv;
using namespace std;
using std::vector;

class Obj {
private:
	vector<Point> associatedBlob; // blob corrente associato all'oggetto
	vector<Point> oldBlob; // blob che era associato all'oggetto precedentemente
	int name;
	int ghostFrame;
	bool toDelete;
public:
	/**
	 * Costruttore
	 * name: valore intero che identifica l'ID dell'oggetto
	 */
	Obj(int name);

	/**
	 * Associa all'oggetto il blob b
	 */
	void associateBlob(vector<Point> b);

	/**
	 * Deassocia il blob dal frame
	 */
	void deassociateBlob();

	/**
	 * Incrementa il ghost frame di uno
	 * Ritorna il valore del ghost frame dopo l'incremento
	 */
	int incremGhostFrame();

	/**
	 * Resetta il ghost frame, riportandolo a zero
	 * Ritorna il valore del ghost frame a zero
	 */
	int resetGhostFrame();

	/**
	 * Restituisce true se il blob e' stato associato
	 * Restituisce false in caso contrario
	 */
	bool isAssociated();

	/**
	 * Restituisce il valore del parametro ghostFrame
	 */
	int getGhostFrame();

	/**
	 * Restituisce il nome dell'oggetto
	 */
	int getName();

	vector<Point> getOldBlob();

	vector<Point> getAssociatedBlob();

	void setOldBlob(vector<Point> b);

	void setToDelete(bool b);

	bool getToDelete();

};

#endif
