#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "simplifyLine.h"
#include "geojsonWriter.h"
#include "transform.h"

using namespace std;
using namespace cv;

void draw(Tracks &tracks)
{
    IplImage * img = cvCreateImage(cvSize(1000,1000),IPL_DEPTH_8U,3); //创建一张图片
    cvZero(img);//初始化图片
    //画直线  cvLine cvPoint与cvPoint是线段的起点和终点
    //CV_RGB：线段的颜色 5：线段的粗细 CV_AA：线段的类型
    int scale = 10;
    for(int j=0; j<tracks.size(); j++){
        Segments segments = tracks[j];
        for(int i=0; i<segments.size();i++){
            Segment segment = segments[i];
            LTPoint startPoint = segment.points[0];
            LTPoint endPoint = segment.points.back();
           cvLine(img,cvPoint(startPoint.x/scale,startPoint.y/scale),cvPoint(endPoint.x/scale,endPoint.y/scale),CV_RGB(0,255,255),2,CV_AA,0);

            for(int j=0; j<segment.points.size(); j++){
                CvPoint center(segment.points[j].x/scale,segment.points[j].y/scale);
                cvCircle(img,center,0.5,Scalar(0, 0, 255),2);
            }
        }
        break;
    }
    cvNamedWindow("Lines",1); //创建窗体
    cvShowImage("Lines",img);//显示图片
    cvWaitKey(0);
    cvReleaseImage(&img);
    cvDestroyWindow("Lines");
}

int readRecord(char* path,RunRecord* pRunRecord)
{
    FILE* fp = fopen(path, "r");
    if(fp==NULL){
        cerr<<"error: cannot open json file"<<endl;
        return -1;
    }

    fseek(fp, 0L, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char* content = new char[len+1];
    fread(content, len, 1, fp);

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
    if(argc!=3)
    {
        printf("AntiCheat Usage, ./AntiCheat runRecordPath outputPath\n");
        return -1;
    }
    //读跑步数据
    char* path = argv[1];
    RunRecord* pRunRecord = new RunRecord;
    int ret = readRecord(path,pRunRecord);
    if(pRunRecord->trackPoints.size()<=2){
        printf("error,too less track points\n");
        return -1;
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
    //简化
    Tracks tracks;
    Segments segments;
    rgConfig config;
    simplifyLine.simplifyTrack(config,inputPoints,tracks);
    //绘制
//    draw(tracks);
    vector<TPoint> simplifyPoints;
    Transform transform;
    for(int j=0; j<tracks.size(); j++){
        //转化到world坐标系
        segments    = tracks[j];
        LTPoint lTPoint = segments[0].points[0];
        TPoint tPoint = simplifyLine.local2world(box,lTPoint);
        simplifyPoints.push_back(tPoint);
        for(int i=0; i<segments.size(); i++){
            lTPoint = segments[i].points.back();
            tPoint = simplifyLine.local2world(box,lTPoint);
            simplifyPoints.push_back(tPoint);
        }
        //转换到wgs84坐标系
        vector<TPoint> simplifyWGS84Points;
        for(int i=0; i<simplifyPoints.size(); i++){
            TPoint tPoint = simplifyPoints[i];
            double mgLat,mgLon;
            transform.gcj2wgs84(tPoint.latitude,tPoint.longitude,mgLat,mgLon);
            tPoint.latitude     = mgLat;
            tPoint.longitude    = mgLon;
            simplifyWGS84Points.push_back(tPoint);
        }
        //输出为geojson文件
        if(simplifyPoints.size()>1){
            GeojsonWriter writer;
            char* outputPath = argv[2];
            writer.appendLine(outputPath,simplifyWGS84Points);
        }
        simplifyLine.clear();
    }

    // GeojsonWriter writer;
    // char* outputPath = "../../data/lp_simplify.json";
    // writer.appendLine(outputPath,pRunRecord->trackPoints);

    delete pRunRecord;
    return 0;
}
