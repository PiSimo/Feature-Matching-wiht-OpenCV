#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main(int argc,char* argv[]){
	if(argc < 2){
		cout<<"./out <object>\n";
		return 0;
	}
	VideoCapture cap(-1);
	Mat frame,des,des1,subj;
	vector<Mat > as;
	vector<KeyPoint> vect;
	vector<vector<DMatch> > matches;


	Ptr<FeatureDetector> p = FeatureDetector::create("FAST"); //I used FAST because it's really fast ( if you want you can change FAST with ORB)
	Ptr<DescriptorExtractor> extract = DescriptorExtractor::create("ORB"); //ORB = good perfomance
	BFMatcher matcher(NORM_L2);

	int x  = 0;
	for(int i  = 1;i != argc;i++){
		vector<KeyPoint > cvect;
		subj = imread(argv[i]);
		p->detect(subj,cvect);
		x += cvect.size();
		extract->compute(subj, cvect,des);
		as.push_back(des);
	}

	x = x /  (argc - 1);
	matcher.add(as);
	matcher.train();
	cout<<"Extracted Points: "<<x<<endl;
	for(;;){
		cap >> frame;
		p->detect(frame,vect);
		extract->compute(frame, vect, des1);
		matcher.knnMatch(des1,matches,2);
		vector<DMatch > good_m;
		vector<Point > pnt;
		vector<Point > checked,sq;
		for(int i= 0;i < matches.size();i++){

			if(matches[i][0].distance < 0.6 * matches[i][1].distance){
				good_m.push_back(matches[i][0]);
				pnt.push_back(vect[good_m[good_m.size() -1].queryIdx].pt);
			}

		}


		if(good_m.size() > 0){
			int n = 20;
			vector<Rect > rs;
			for(int i = 0;i != pnt.size();i++){
					Rect r(pnt[i].x - n,pnt[i].y - n,2 * n,2 * n);
					rs.push_back(r);
			}
			for(int i = 0;i != pnt.size();i++){
				Point a = Point(pnt[i].x - n,pnt[i].y - n),b = Point(pnt[i].x + n,pnt[i].y - n),c = Point(pnt[i].x - n,pnt[i].y + n),d = Point(pnt[i].x + n,pnt[i].y + n);
				int conf = 0;
				for(int t = 0;t != rs.size();t++){

					if(t != i){
					if(a.inside(rs[t]) || b.inside(rs[t]) || c.inside(rs[t]) || d.inside(rs[t])){
						conf++;
					}
					if(conf == 2){
						checked.push_back(pnt[t]); //We select only the points near to other (isolated points potentialy wrong 
						sq.push_back(a);
						sq.push_back(b);
						sq.push_back(c);
						sq.push_back(d);
						break;
					}
				}
			}

			}
			cout<<"Good Points : "<<checked.size()<<" | Bad (pnt) Points :"<<pnt.size() - checked.size()<<endl;

			if(checked.size() != 0){
			Point test = checked[0],min = checked[0];
			int max_x = 0,max_y = 0,min_x = min.x,min_y = min.y;
			for(int i = 0;i != checked.size();i++)circle(frame, checked[i], 3,Scalar(0,0,255),1);
			for(int i = 0;i != sq.size();i++){
								if(sq[i].x > test.x){
											max_x = sq[i].x;
								}
								if(sq[i].y > test.y){
									max_y = sq[i].y;
								}
								if(sq[i].x < min.x){
									min_x = sq[i].x;
								}
								if(sq[i].y < min.y){
									min_y = sq[i].y;
								}
								min = Point(min_x,min_y);
								test = Point(max_x,max_y);
			}
			if(checked.size() > 2){
				Rect form(min_x,min_y,(max_x - min_x),(max_y - min_y));
				rectangle(frame,form.br(),form.tl(),Scalar(0,255,0),1);
			}
			//circle(frame,Point(((max_x - min_x)+min_x),((max_y - min_y)+min_y)),(max_x - min_x),Scalar(0,200,30),1);
		/*for(int i = 0;i != checked.size() - 4 && checked.size() > 4;i++){
					line(frame,checked[i],checked[i + 1],Scalar(250,250,250),1);
					line(frame,checked[i],checked[i + 2],Scalar(250,250,250),1);
					line(frame,checked[i],checked[i + 3],Scalar(250,250,250),1);
					line(frame,checked[i],checked[i + 4],Scalar(250,250,250),1);

			}
			int l = checked.size();
			if(checked.size() > 4){
				line(frame,checked[l - 4],checked[l - 3],Scalar(250,250,250),1);
				line(frame,checked[l - 3],checked[l - 2],Scalar(250,250,250),1);
				line(frame,checked[l - 2],checked[l - 4],Scalar(250,250,250),1);
				line(frame,checked[l - 2],checked[l - 1],Scalar(250,250,250),1);
				line(frame,checked[l - 1],checked[l - 3],Scalar(250,250,250),1);
			}*/
		}
	}

		imshow("RESULT",frame);
		if(((char)waitKey(33)) == 27)break;

	}

	return 0;
}