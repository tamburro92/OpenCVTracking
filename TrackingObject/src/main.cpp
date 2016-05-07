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
	RNG rng(12345);
	Mat frame;
	Mat fgMaskMOG2, fgMaskKNN;
	Mat out;
	Mat kernelErosion = getStructuringElement(MORPH_RECT, Size(3, 3),
			Point(-1, -1));
	Mat kernelDilatation = getStructuringElement(MORPH_RECT, Size(3, 3),
			Point(-1, -1));

	int keyboard;

	VideoCapture src("video.avi");

	namedWindow("Frame");
	namedWindow("FG Mask MOG 2");
	moveWindow("FG Mask MOG 2", 0, 500);
	namedWindow("FG Mask MOG 2 Filtered");
	moveWindow("FG Mask MOG 2 Filtered", 500, 0);

	if (!src.isOpened())
		exit(EXIT_FAILURE);

	Ptr<BackgroundSubtractor> backGroundSubMOG2 =
			new BackgroundSubtractorMOG2();

	while ((char) keyboard != 'q') {
		if (!src.read(frame)) {
			cout << "error_in_read_frame" << endl;
			exit(EXIT_FAILURE);
		}

		//	GaussianBlur( frame, frame, Size( 3, 3 ), 0, 0 );
		backGroundSubMOG2->operator ()(frame, fgMaskMOG2);

		imshow("FG Mask MOG 2", fgMaskMOG2);
		erode(fgMaskMOG2, fgMaskMOG2, kernelErosion); //Applico Erosione
		dilate(fgMaskMOG2, fgMaskMOG2, kernelDilatation); //Applico Dilatazione
		imshow("FG Mask MOG 2 Filtered", fgMaskMOG2);

		vector<vector<Point> > contours;
		//trova tutti i contorni dei BLOBS
		findContours(fgMaskMOG2, contours, RETR_TREE, CHAIN_APPROX_SIMPLE,
				Point(0, 0));
		Mat drawing = Mat::zeros(fgMaskMOG2.size(), CV_8UC3);

		/// Approximate contours to polygons + get bounding rects and circles
		vector<vector<Point> > contours_poly(contours.size());
		vector<Point2f> center(contours.size());
		vector<float> radius(contours.size());

		for (size_t i = 0; i < contours.size(); i++) {
			Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255),
					rng.uniform(0, 255));

			///funzioni per calcolare il centro dei blob
			approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true); //approssima il contorno in un polinomio
			minEnclosingCircle((Mat) contours_poly[i], center[i], radius[i]);

			cout << "Center : " << center[i] << endl;
			cout << "Area : " << contourArea(contours[i]) << endl;

			if (contourArea(contours[i]) > MINAREA) //FUNZIONE per calcolare l'area dei BLOBs
				drawContours(drawing, contours, (int) i, color, 2, 8, noArray(),
						0, Point());

		}

		MatrixSimilarity m(3, 3);
		m.matrix[0][0]=2;m.matrix[0][1]=1;m.matrix[0][2]=1.3;
		m.matrix[1][0]=4;m.matrix[1][1]=2.1;m.matrix[1][2]=1.1;
		cout<<"BLOB: "<<m.remainBlobs().size()<<endl;
		cout<<"Obj: "<<m.remainObjects().size()<<endl;
		cout<<m<<endl;

		std::vector<float> max=m.maxMatrix();
		cout<<max[0]<<" "<<max[1]<<" "<<max[2]<<endl;
		m.deleteFromMatrix(max[0],max[1]);
		cout<<"BLOB: "<<m.remainBlobs().size()<<endl;
		cout<<"Obj: "<<m.remainObjects().size()<<endl;
		cout<<"DOPO"<<endl;
		cout<<m<<endl;


		max=m.maxMatrix();
		cout<<max[0]<<" "<<max[1]<<" "<<max[2]<<endl;
		m.deleteFromMatrix(max[0],max[1]);
		cout<<"BLOB: "<<m.remainBlobs().size()<<endl;
		cout<<"Obj: "<<m.remainObjects().size()<<endl;
		cout<<"DOPO"<<endl;
		cout<<m<<endl<<endl;

		cout<<"BLOB: "<<m.remainBlobs().size()<<endl;
		cout<<"Obj: "<<m.remainObjects().size()<<endl;




		//m.~MatrixSimilarity();

		imshow("FG Mask MOG 2 blobs", drawing);
		imshow("Frame", frame);
		waitKey();

		keyboard = waitKey(30);

	}

}

