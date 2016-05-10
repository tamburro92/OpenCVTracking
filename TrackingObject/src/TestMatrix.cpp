//#define TEST_MATRIX
#ifdef TEST_MATRIX

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include <vector>
#include "MatrixSimilarity.hpp"
#include "Obj.hpp"


using namespace cv;
using namespace std;
using std::vector;
#define MINAREA 240

int main(int argc, char** argv) {
	vector<vector<Point> > blobs;
	vector<Point> blob;
	vector<Point> blob2;
	vector<Obj>  oggetti;

///INIT BLOBS E OBJECTS///////
	for(int i=0;i<3;i++){
	   Point p(50+i,50+i*2);
	   blob.push_back(p);
	   blobs.push_back(blob);
	   blob.clear();
	}
	for(int i=0;i<3;i++){
		   Point p(60+i,60+i*2);
		   blob2.push_back(p);
		   Obj obj(i);
		   obj.associateBlob(blob2);
		   obj.setOldBlob(blob2);
		   oggetti.push_back(obj);
		   blob2.clear();
		}

//////TESTETSTEST////////////////////
	MatrixSimilarity m(blobs.size(), oggetti.size());
		m.calculateMatrix(oggetti, blobs);
		cout<<"MATRIX "<<endl<<m<<endl;

		vector<float> max=m.maxMatrix();
		cout<<"MAX :"<<max[2]<<endl;
		while(max[2]>0.2){
		   oggetti[max[1]].associateBlob(blobs[max[0]]);
		   m.deleteFromMatrix(max[0],max[1]);  //cancella
		   max=m.maxMatrix();
		   cout<<"MAX: "<<max[2]<<endl;
		   cout<<"MATRIX "<<endl<<m<<endl;
		}
		cout<<"REM OBJ "<<m.remainObjects().size()<<endl;
		cout<<"REM BLOBS "<<m.remainBlobs().size()<<endl;
}

#endif
