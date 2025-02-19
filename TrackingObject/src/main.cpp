#define MAIN //utilizzata per selezionare se usare il codicediTEST o il MAIN;=MAIN,TEST_MATRIX
#ifdef MAIN

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include "MatrixSimilarity.hpp"
#include "Obj.hpp"

using namespace cv;
using namespace std;
#define MINAREA 500
void findDrawBlobs(InputOutputArray& image, InputOutputArray& drawing, vector<vector<Point> >& blobs);
void tracking(vector<Obj>& oggetti, vector<vector<Point> >& blobs);
static int name = 0;

int main(int argc, char** argv) {
	Mat frame;
	Mat fgMaskMOG2, fgMaskKNN;
	Mat out;
	vector<Obj> oggetti;
	Mat kernelDilate = getStructuringElement(MORPH_RECT, Size(5, 5)); // kernel per la dilatazione, MORPH_RECT indica che e' un rettangolo
	Mat kernelErode = getStructuringElement(MORPH_RECT, Size(3, 3)); // kernel per l'erosione, MORPH_RECT indica che e' un rettangolo

	int keyboard;

	VideoCapture src("atrium.avi"); // si suppone che il video stia nella cartella del progetto
	//VideoCapture src("video.avi"); // si suppone che il video stia nella cartella del progetto

	namedWindow("Frame");
	namedWindow("FG Mask MOG 2");
	moveWindow("FG Mask MOG 2", 0, 500);
	namedWindow("FG Mask MOG 2 Filtered");
	moveWindow("FG Mask MOG 2 Filtered", 500, 0);

	if (!src.isOpened()) { // se il video non e' aperto, il programma termina
		exit(EXIT_FAILURE);
	}

	// BackgroundSubtractor viene usato per generare la foreground mask
	int history = 100; // numero di frame usati per aggiornare il background
	float varThreshold = 100; // indica la distanza di Mahalanobis di un pixel dal modello di background
	bool bShadowDetection = true; // true se deve essere rilevata anche l'ombra degli oggetti

	/*learningRate The value between 0 and 1 that indicates how fast the background model is
	 learnt. Negative parameter value makes the algorithm to use some automatically chosen learning
	 rate. 0 means that the background model is not updated at all, 1 means that the background model
	 is completely reinitialized from the last frame. */
	float learningRate = -1; //tra 0 e 1; -1 indica che l'algoritmo lo autocalcola
	Ptr<BackgroundSubtractor> backGroundSubMOG2 = new BackgroundSubtractorMOG2(
			history, varThreshold, bShadowDetection);

	while ((char) keyboard != 'q') { // passa al frame successivo quando si preme q
		if (!src.read(frame)) { // read ritorna il frame, in caso negativo ritorna false
			cout << "error_in_read_frame" << endl;
			exit(EXIT_FAILURE);
		}


		backGroundSubMOG2->operator ()(frame, fgMaskMOG2, learningRate); // aggiorna il modello del background e calcola la foreground mask

		imshow("FG Mask MOG 2", fgMaskMOG2);
		//Threshold vado ad eliminare le ombre ed altri pixel...
		threshold(fgMaskMOG2, fgMaskMOG2, 252, 255, 0); // Threshold Type 0: Binary
		imshow("FG Mask MOG 2 Threshold", fgMaskMOG2);

		dilate(fgMaskMOG2, fgMaskMOG2, kernelDilate); //Applico Dilatazione
		erode(fgMaskMOG2, fgMaskMOG2, kernelErode); //Applico Erosione

		imshow("FG Mask MOG 2 Filtered", fgMaskMOG2);

		Mat drawing = Mat::zeros(fgMaskMOG2.size(), CV_8UC3);
		vector<vector<Point> > blobs;
		findDrawBlobs(fgMaskMOG2, drawing, blobs);
		tracking(oggetti, blobs);

		vector<Point> contours_poly;
		Rect boundRect;
		for (auto o : oggetti) {
			for (int i = 0; i < o.getPositions().size(); i++) {
				circle(drawing,
						Point(o.getPositions()[i].x, o.getPositions()[i].y), 1,
						Scalar(o.getColor()[0], o.getColor()[1],
								o.getColor()[2]), 1, 8);
				//disegna rettangolo
				approxPolyDP(Mat(o.getOldBlob()), contours_poly, 3, true); //approssima il contorno in un polinomio, il 3 indica l'accuratezza dell'approssimazione, true indica che la linea e' chiusa
				boundRect = boundingRect(Mat(contours_poly));
				rectangle(drawing, boundRect,
						Scalar(o.getColor()[0], o.getColor()[1],
								o.getColor()[2]), 2, 8, 0);
				//disegno nell'originale
				circle(frame,
						Point(o.getPositions()[i].x, o.getPositions()[i].y), 1,
						Scalar(o.getColor()[0], o.getColor()[1],
								o.getColor()[2]), 1, 8);
				rectangle(frame, boundRect,
						Scalar(o.getColor()[0], o.getColor()[1],
								o.getColor()[2]), 2, 8, 0);

				putText(frame, to_string(o.getName()), boundRect.tl(),
						FONT_ITALIC, 1, cvScalar(255));
				putText(drawing, to_string(o.getName()), boundRect.tl(),
						FONT_ITALIC, 1, cvScalar(255));

			}
		}

		imshow("FG Mask MOG 2 blobs", drawing);
		imshow("Frame", frame);
		waitKey();

		keyboard = waitKey(30);

	}

}

void findDrawBlobs(InputOutputArray& image, InputOutputArray& drawing,
		vector<vector<Point> >& blobs) {
	RNG rng(12345); // generatore di un numero casuale
	vector<Point> contours_poly;
	Point2f center;
	float radius;
	vector<vector<Point> > blobtemp;

	//trova tutti i contorni dei BLOBS
	// contours: immagine in output con i contorni rilevati, ogni contorno e' memorizzato in un vettore
	// RETR_TREE: modalita' in cui vengono memorizzati i contorni
	// CHAIN_APPROX_SIMPLE: comprime segmenti orizzontali, verticali e diagonali e lascia solo i loro punti finali
	// Point(0, 0): eventuale offset
	findContours(image, blobtemp, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	// approssima i controni a dei poligoni
	for (size_t i = 0; i < blobtemp.size(); i++) {
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
				rng.uniform(0, 255));

		//funzioni per calcolare il centro dei blob
		approxPolyDP(Mat(blobtemp[i]), contours_poly, 3, true); //approssima il contorno in un polinomio, il 3 indica l'accuratezza dell'approssimazione, true indica che la linea e' chiusa
		minEnclosingCircle((Mat) contours_poly, center, radius); // realizza un cerchio, vengono passati i punti, il centro, il raggio

		if (contourArea(blobtemp[i]) > MINAREA) { //FUNZIONE per calcolare l'area dei BLOBs
			drawContours(drawing, blobtemp, (int) i, color, 2, 8, noArray(), 0,
					Point());
			blobs.push_back(blobtemp[i]); // vengono aggiunti solo i blob che rispettano il vincolo su MINAREA
		}

	}
}
void tracking(vector<Obj>& oggetti, vector<vector<Point> >& blobs) {
	float THRESHOLD = 0.4;
	int GHOST_FRAME = 5;
	if (oggetti.empty()) { //se gli oggetti sono vuoti inizializzali a blobs
		for (int i = 0; i < blobs.size(); i++) {
			Obj obj(name);
			obj.associateBlob(blobs[i]);
			obj.setOldBlob(blobs[i]);
			oggetti.push_back(obj);
		}
		return;
	}

	for (auto o : oggetti) //ad inizio iterazione deassocio tutto
		o.deassociateBlob();
	cout << "blobs.size :" << blobs.size() << endl;
	MatrixSimilarity m(blobs.size(), oggetti.size());
	m.calculateMatrix(oggetti, blobs);
	vector<float> max = m.maxMatrix();
	while (max[2] > THRESHOLD) {
		oggetti[max[1]].associateBlob(blobs[max[0]]);
		m.deleteFromMatrix(max[0], max[1]);  //cancella
		max = m.maxMatrix();
	}

	vector<int> indexRemainObj = m.remainObjects();
	vector<int> indexToRemove;
	for (auto i : indexRemainObj) {
		oggetti[i].incremGhostFrame();
		if (oggetti[i].getGhostFrame() > GHOST_FRAME) {
			oggetti[i].setToDelete(true);
		}
	}
	vector<Obj> temp;
	for (auto obj : oggetti) {
		if (obj.getToDelete() == false) {
			temp.push_back(obj);
		}
	}
	oggetti = temp;
	vector<int> indexRemainBlob = m.remainBlobs();

	for (auto indexBlob : indexRemainBlob) {
		if (!oggetti.empty()) {
			name = oggetti.back().getName();
		}
		name++;
		Obj obj(name++);
		obj.associateBlob(blobs[indexBlob]);
		oggetti.push_back(obj);
	}
}

#endif
