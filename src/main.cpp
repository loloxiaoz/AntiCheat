#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "simplifyLine.h"

using namespace std;
using namespace cv;

void draw(Segments &segments)
{
    IplImage * img = cvCreateImage(cvSize(1000,1000),IPL_DEPTH_8U,3); //创建一张图片
    cvZero(img);//初始化图片
    //画直线  cvLine cvPoint与cvPoint是线段的起点和终点
    //CV_RGB：线段的颜色 5：线段的粗细 CV_AA：线段的类型
    for(int i=0; i<segments.size();i++){
        Segment segment = segments[i];
        LTPoint startPoint = segment.points[0];
        LTPoint endPoint = segment.points.back();
        cvLine(img,cvPoint(startPoint.x,startPoint.y),cvPoint(endPoint.x,endPoint.y),CV_RGB(0,255,255),2,CV_AA,0);

        for(int j=0; j<segment.points.size(); j++){
            CvPoint center(segment.points[j].x,segment.points[j].y);
            cvCircle(img,center,0.1,Scalar(0, 0, 255),2);
        }
    }

    cvNamedWindow("Lines",1); //创建窗体
    cvShowImage("Lines",img);//显示图片
    cvWaitKey(0);
    cvReleaseImage(&img);
    cvDestroyWindow("Lines");
}

int readRecord(RunRecord* pRunRecord)
{
   // char* path = "../../data/lp_20170811.json";
    char* path = "../../data/20km.json";

    FILE* fp = fopen(path, "r");
    if(fp==NULL){
        cerr<<"error: cannot open json file"<<endl;
        return -1;
    }

    fseek(fp, 0L, SEEK_END);
    int len = ftell(fp);
    if(len==-1){
        cerr<<"error: cannot get length of file"<<endl;
        return -1;
    }

    fseek(fp, 0L, SEEK_SET);
    char* content = new char[len+1];
    int n = fread(content, len, 1, fp);
    if(n!=1){
        cerr<<"error: cannot read from file"<<endl;
        return -1;
    }

    RecordLoader loader;
    int ret = loader.load(content,pRunRecord);

    if(ret!=1){
        cerr<<"error: load file error"<<endl;
        return -1;
    }
    delete content;
    return 1;
}

int main(int argc, char * argv[])
{
    RunRecord* pRunRecord = new RunRecord;
    int ret = readRecord(pRunRecord);

    SimplifyLine simplifyLine;
    BBox2D box = simplifyLine.findBoundingBox2D(pRunRecord->trackPoints);
    vector<LTPoint> inputPoints;
    for(int i=0; i<pRunRecord->trackPoints.size(); i++){
        TPoint  tPoint = pRunRecord->trackPoints[i];
        LTPoint lTPoint = simplifyLine.world2Local(box,tPoint);
        inputPoints.push_back(lTPoint);
    }

    Segments segments;
    rgConfig config;
    simplifyLine.simplifyTrack(config,inputPoints,segments);

    //绘制
    draw(segments);

    delete pRunRecord;
    return 0;
}
