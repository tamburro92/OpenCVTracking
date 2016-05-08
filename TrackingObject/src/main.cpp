#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include "MatrixSimilarity.hpp"

using namespace cv;
using namespace std;

#define MINAREA 120

int main(int argc, char** argv) {
	RNG rng(12345); // generatore di un numero casuale
	Mat frame;
	Mat fgMaskMOG2, fgMaskKNN;
	Mat out;
	Mat kernelErosion = getStructuringElement(MORPH_RECT, Size(3, 3),
			Point(-1, -1)); // kernel per l'erosione, MORPH_RECT indica che e' un rettangolo
	Mat kernelDilatation = getStructuringElement(MORPH_RECT, Size(3, 3),
			Point(-1, -1)); // kernel per la dilatazione, MORPH_RECT indica che e' un rettangolo

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
	Ptr<BackgroundSubtractor> backGroundSubMOG2 = new BackgroundSubtractorMOG2();

	while ((char) keyboard != 'q') { // passa al frame successivo quando si preme q
		if (!src.read(frame)) { // read ritorna il frame, in caso negativo ritorna false
			cout << "error_in_read_frame" << endl;
			exit(EXIT_FAILURE);
		}

		//	GaussianBlur( frame, frame, Size( 3, 3 ), 0, 0 );
		backGroundSubMOG2->operator ()(frame, fgMaskMOG2); // aggiorna il modello del background e calcola la foreground mask

		imshow("FG Mask MOG 2", fgMaskMOG2);
		erode(fgMaskMOG2, fgMaskMOG2, kernelErosion); //Applico Erosione
		dilate(fgMaskMOG2, fgMaskMOG2, kernelDilatation); //Applico Dilatazione
		imshow("FG Mask MOG 2 Filtered", fgMaskMOG2);

		vector<vector<Point> > contours;

		//trova tutti i contorni dei BLOBS
		// contours: immagine in output con i contorni rilevati, ogni contorno e' memorizzato in un vetore
		// RETR_TREE: modalita' in cui vengono memorizzati i contorni
		// CHAIN_APPROX_SIMPLE: comprime segmenti orizzontali, verticali e diagonali e lascia solo i loro punti finali
		// Point(0, 0): eventuale offset
		findContours(fgMaskMOG2, contours, RETR_TREE, CHAIN_APPROX_SIMPLE,
				Point(0, 0));

		Mat drawing = Mat::zeros(fgMaskMOG2.size(), CV_8UC3);

		// approssima i controni a dei poligoni
		vector<vector<Point> > contours_poly(contours.size());
		vector<Point2f> center(contours.size());
		vector<float> radius(contours.size());

		for (size_t i = 0; i < contours.size(); i++) {
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
					rng.uniform(0, 255));

			//funzioni per calcolare il centro dei blob
			approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true); //approssima il contorno in un polinomio, il 3 indica l'accuratezza dell'approssimazione, true indica che la linea e' chiusa
			minEnclosingCircle((Mat) contours_poly[i], center[i], radius[i]); // realizza un cerchio, vengono passati i punti, il centro, il raggio

			cout << "Center : " << center[i] << endl;
			cout << "Area : " << contourArea(contours[i]) << endl;

			if (contourArea(contours[i]) > MINAREA) //FUNZIONE per calcolare l'area dei BLOBs
				drawContours(drawing, contours, (int) i, color, 2, 8, noArray(),
						0, Point());

		}

		MatrixSimilarity m(3, 3);
		m.matrix[0][0] = 2;
		m.matrix[0][1] = 1;
		m.matrix[0][2] = 1.3;
		m.matrix[1][0] = 4;
		m.matrix[1][1] = 2.1;
		m.matrix[1][2] = 1.1;
		cout << "BLOB: " << m.remainBlobs().size() << endl;
		cout << "Obj: " << m.remainObjects().size() << endl;
		cout << m << endl;

		std::vector<float> max = m.maxMatrix();
		cout << max[0] << " " << max[1] << " " << max[2] << endl;
		m.deleteFromMatrix(max[0], max[1]);
		cout << "BLOB: " << m.remainBlobs().size() << endl;
		cout << "Obj: " << m.remainObjects().size() << endl;
		cout << "DOPO" << endl;
		cout << m << endl;

		max = m.maxMatrix();
		cout << max[0] << " " << max[1] << " " << max[2] << endl;
		m.deleteFromMatrix(max[0], max[1]);
		cout << "BLOB: " << m.remainBlobs().size() << endl;
		cout << "Obj: " << m.remainObjects().size() << endl;
		cout << "DOPO" << endl;
		cout << m << endl << endl;

		cout << "BLOB: " << m.remainBlobs().size() << endl;
		cout << "Obj: " << m.remainObjects().size() << endl;

		//m.~MatrixSimilarity();

		imshow("FG Mask MOG 2 blobs", drawing);
		imshow("Frame", frame);
		waitKey();

		//keyboard = waitKey(30);

	}

}

