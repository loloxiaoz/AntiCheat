#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "simplifyLine.h"
#include "transform.h"

using namespace std;
using namespace cv;

enum LineReason{Distance=-1,Variance=-2,StepLength=-3,NoTrackPoints=-4};

int checkLine(Tracks tracks)
{
    if(tracks.size()>1){
        return Distance;
    }
    Segments segments    = tracks[0];
    for(int i=0; i<segments.size(); i++){
        Segment segment = segments[i];
        vector<LTPoint> points = segment.points;
        LineParam lParam = segment.lParam;
        if(points.size()>2){
            if(lParam.maxDist<0.03){
                return Distance;
            }
            if(lParam.variance<0.02){
                return Variance;
            }
        }else{
            double stepLength = lParam.length/points.size();
            if(stepLength>10){
                return StepLength;
            }
        }
    }
    return 0;
}

void draw(Tracks &tracks)
{
    IplImage * img = cvCreateImage(cvSize(1000,600),IPL_DEPTH_8U,3); //创建一张图片
    cvZero(img);//初始化图片
    //画直线  cvLine cvPoint与cvPoint是线段的起点和终点
    //CV_RGB：线段的颜色 5：线段的粗细 CV_AA：线段的类型
    int scale = 20;
    for(int j=0; j<tracks.size(); j++){
        Segments segments = tracks[j];
        for(int i=0; i<segments.size();i++){
            Segment segment = segments[i];
            LTPoint startPoint = segment.points[0];
            LTPoint endPoint = segment.points.back();
            cvLine(img,cvPoint(startPoint.x/scale,600-startPoint.y/scale),cvPoint(endPoint.x/scale,600-endPoint.y/scale),CV_RGB(0,255,255),2,CV_AA,0);

            for(int j=0; j<segment.points.size(); j++){
                CvPoint center(segment.points[j].x/scale,600-segment.points[j].y/scale);
                cvCircle(img,center,0.5,Scalar(0, 0, 255),2);
            }
        }
    }
    cvNamedWindow("Lines",1); //创建窗体
    cvShowImage("Lines",img);//显示图片
    cvWaitKey(0);
    cvReleaseImage(&img);
    cvDestroyWindow("Lines");
}

int main(int argc, char * argv[])
{
    if(argc!=2)
    {
        printf("CheckLine Usage, ./AntiCheat runRecordPath\n");
        return -1;
    }
    int ret = 0;
    //读跑步数据
    char* path = argv[1];
    RunRecord* pRunRecord = new RunRecord;
    RecordLoader recordLoader;
    recordLoader.read(path,pRunRecord);
    if(pRunRecord->trackPoints.size()<=2){
        ret = NoTrackPoints;
        printf("Checkline ret: %d \n", ret);
        return ret;
    }
    //转化到local坐标系
    SimplifyLine simplifyLine;
    BBox2D box = simplifyLine.findBoundingBox2D(pRunRecord->trackPoints);
    vector<LTPoint> inputPoints;
    for(int i=0; i<pRunRecord->trackPoints.size(); i++){
        TPoint  tPoint = pRunRecord->trackPoints[i];
        LTPoint lTPoint = simplifyLine.world2Local(box,tPoint);
        inputPoints.push_back(lTPoint);
    }
    delete pRunRecord;
    //简化
    Tracks tracks;
    Segments segments;
    rgConfig config;
    simplifyLine.simplifyTrack(config,inputPoints,tracks);
    //绘制
    //draw(tracks);
    ret = checkLine(tracks);
    printf("Checkline ret: %d \n", ret);
    return ret;
}
