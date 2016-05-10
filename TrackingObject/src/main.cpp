#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include "MatrixSimilarity.hpp"
#include "Obj.hpp"

using namespace cv;
using namespace std;
#define MINAREA 120
void findDrawBlobs(InputOutputArray& image, InputOutputArray& contours);
void tracking(vector<Obj>& oggetti, vector<vector<Point> >& blobs);

int main(int argc, char** argv) {
	Mat frame;
	Mat fgMaskMOG2, fgMaskKNN;
	Mat out;
	Mat kernelErosion = getStructuringElement(MORPH_ELLIPSE, Size(3, 3)); // kernel per l'erosione, MORPH_RECT indica che e' un rettangolo
	Mat kernelDilatation = getStructuringElement(MORPH_ELLIPSE, Size(5, 5)); // kernel per la dilatazione, MORPH_RECT indica che e' un rettangolo

	int keyboard;

	VideoCapture src("video.avi"); // si suppone che il video stia nella cartella del progeto

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

		//	GaussianBlur( frame, frame, Size( 3, 3 ), 0, 0 );

		backGroundSubMOG2->operator ()(frame, fgMaskMOG2, learningRate); // aggiorna il modello del background e calcola la foreground mask

		imshow("FG Mask MOG 2", fgMaskMOG2);
		//Threshold vado ad eliminare le ombre ed altri pixel...
		threshold(fgMaskMOG2, fgMaskMOG2, 252, 255, 0); // Threshold Type 0: Binary
		imshow("FG Mask MOG 2 Threshold", fgMaskMOG2);

		erode(fgMaskMOG2, fgMaskMOG2, kernelErosion); //Applico Erosione
		dilate(fgMaskMOG2, fgMaskMOG2, kernelDilatation); //Applico Dilatazione
		imshow("FG Mask MOG 2 Filtered", fgMaskMOG2);

		Mat drawing = Mat::zeros(fgMaskMOG2.size(), CV_8UC3);

		findDrawBlobs(fgMaskMOG2, drawing);
		imshow("FG Mask MOG 2 blobs", drawing);
		imshow("Frame", frame);
		waitKey();

		keyboard = waitKey(30);

	}

}

void findDrawBlobs(InputOutputArray& image, InputOutputArray& drawing) {
	RNG rng(12345); // generatore di un numero casuale
	vector<vector<Point> > contours;
	vector<Point> contours_poly;
	Point2f center;
	float radius;

	//trova tutti i contorni dei BLOBS
	// contours: immagine in output con i contorni rilevati, ogni contorno e' memorizzato in un vettore
	// RETR_TREE: modalita' in cui vengono memorizzati i contorni
	// CHAIN_APPROX_SIMPLE: comprime segmenti orizzontali, verticali e diagonali e lascia solo i loro punti finali
	// Point(0, 0): eventuale offset
	findContours(image, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	// approssima i controni a dei poligoni
	for (size_t i = 0; i < contours.size(); i++) {
		Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
				rng.uniform(0, 255));

		//funzioni per calcolare il centro dei blob
		approxPolyDP(Mat(contours[i]), contours_poly, 3, true); //approssima il contorno in un polinomio, il 3 indica l'accuratezza dell'approssimazione, true indica che la linea e' chiusa
		minEnclosingCircle((Mat) contours_poly, center, radius); // realizza un cerchio, vengono passati i punti, il centro, il raggio

		cout << "Center : " << center << endl;
		cout << "Area : " << contourArea(contours[i]) << endl;

		if (contourArea(contours[i]) > MINAREA){} //FUNZIONE per calcolare l'area dei BLOBs
			drawContours(drawing, contours[i], (int) i, color, 2, 8, noArray(), 0,
					Point());

	}
}
void tracking(vector<Obj>& oggetti, vector<vector<Point> >& blobs) {
	float TRHESHOLD=3;
	if (oggetti.empty()) { //se gli oggetti sono vuoti inizializzali a blobs
		for (int i = 0; i < blobs.size(); i++) {
			Obj obj(i);
			obj.associateBlob(blobs[i]);
			oggetti.push_back(obj);
		}
	}
	for(auto o:oggetti) //ad inizio iterazione deassocio tutto
		o.deassociateBlob();

	MatrixSimilarity m(blobs.size(), oggetti.size());
	m.calculateMatrix(oggetti, blobs);
	vector<float> max=m.maxMatrix();
	while(max[2]>TRHESHOLD&&max[0]!=-1){
	   oggetti[max[2]].associateBlob(blobs[max[1]]);//associa max[2]
	   m.deleteFromMatrix(max[0],max[1]);  //cancella
	   max=m.maxMatrix();
	}
	vector<int> indexRemainObj=m.remainObjects();
	for(auto i:indexRemainObj){
		oggetti[i].incremGhostFrame();

		//oggetti.erase(i); //vedere come cancellare l'elemento all'interno del for
	}
}

