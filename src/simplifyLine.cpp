#include "stdio.h"
#include <math.h>
#include <vector>
#include "string.h"
#include <algorithm>
#include "simplifyLine.h"

#ifndef ROUND
#define ROUND (0.5F)
#endif

#ifndef PI
#define PI (3.1415926535897932384626433832795)
#endif

#ifndef INT_MAX
#define INT_MAX       2147483647
#endif

#ifndef SCALE_TRANS
#define SCALE_TRANS  50000
#endif

#define DOUBLE_EQ_ZERO(x) (fabs(x) < 1e-6)
#define DOUBLE_NEQ_ZERO(x) (fabs(x) > 1e-6)

using namespace std;

SimplifyLine::SimplifyLine()
{
}

BBox2D SimplifyLine::findBoundingBox2D(vector<TPoint> points)
{
    BBox2D box;
    box.xMin = points[0].longitude;
    box.xMax = points[0].longitude;
    box.yMin = points[0].latitude;
    box.yMax = points[0].latitude;
    for(int i=1; i<points.size(); i++){
        TPoint point = points[i];
        box.xMin = point.longitude<box.xMin?point.longitude:box.xMin;
        box.xMax = point.longitude>box.xMax?point.longitude:box.xMax;
        box.yMin = point.latitude<box.yMin?point.latitude:box.yMin;
        box.yMax = point.latitude>box.yMax?point.latitude:box.yMax;
    }
    box.width   = box.xMax-box.xMin;
    box.height  = box.yMax-box.yMin;
    return box;
}

LTPoint SimplifyLine::world2Local(BBox2D box, TPoint point)
{
    LTPoint lTPoint;
    lTPoint.seqNo   = point.seqNo;
    lTPoint.x       = (point.longitude-box.xMin)*SCALE_TRANS;
    lTPoint.y       = (point.latitude-box.yMin)*SCALE_TRANS;
    return lTPoint;
}

TPoint SimplifyLine::local2world(BBox2D box, LTPoint lTPoint)
{
    TPoint tPoint;
    tPoint.seqNo       = lTPoint.seqNo;
    tPoint.longitude   = lTPoint.x/SCALE_TRANS+box.xMin;
    tPoint.latitude    = lTPoint.y/SCALE_TRANS+box.yMin;
    return tPoint;
}

double SimplifyLine::calcDistance(LineParam lParam,LTPoint point)
{
    double distance = 0;
    distance    = fabs(lParam.a*point.x+lParam.b*point.y+lParam.c);
    distance    = distance/sqrt(pow(lParam.a,2)+pow(lParam.b,2));
    return distance;
}

double SimplifyLine::calcMaxDistance(LineParam lParam,vector<LTPoint> points)
{
    if(DOUBLE_EQ_ZERO(lParam.a)&&DOUBLE_EQ_ZERO(lParam.b)){
        return -1;
    }
    double maxDistance = 0;
    for(int i=0; i<points.size(); i++){
        LTPoint point  = points[i];
        double distance    = calcDistance(lParam,point);
        maxDistance = distance>maxDistance?distance:maxDistance;
    }
    return maxDistance;
}

double SimplifyLine::calcVariance(LineParam param,vector<LTPoint> points)
{
    double sum = 0;
    int size = points.size();
    for(int i=0; i<size; i++){
        double distance =  calcDistance(param, points[i]);
        sum += distance;
    }
    return sum/size;
}

LineParam SimplifyLine::calcLine(LTPoint startPoint,LTPoint endPoint)
{
    LineParam lParam;
    lParam.a = (startPoint.y-endPoint.y)/(startPoint.x*endPoint.y-startPoint.y*endPoint.x);
    lParam.b = (startPoint.x-endPoint.x)/(startPoint.y*endPoint.x-startPoint.x*endPoint.y);
    lParam.c = 1;
    return lParam;
}

double SimplifyLine::calcPointDistance(LTPoint startPoint,LTPoint endPoint)
{
    return sqrt(pow((endPoint.x-startPoint.x),2)+pow((endPoint.y-startPoint.y),2));
}

void SimplifyLine::simplifyTrack(rgConfig config,vector<LTPoint>& inputPoints,Tracks& tracks)
{
    //当前轨迹
    Segments segments;
    //当前线段
    Segment segment;
    LTPoint lastPoint;
    for(int i=0; i<inputPoints.size(); i++){
        LTPoint point = inputPoints[i];
        //如果不是第一个点
        if(segment.points.size()>=1){
            LTPoint startPoint = segment.points[0];
            LTPoint endPoint = point;
            LineParam lParam = calcLine(startPoint,endPoint);
            double maxDist   = calcMaxDistance(lParam,segment.points);
            double variance  = calcVariance(lParam,segment.points);
            double distance  = calcPointDistance(segment.points.back(),endPoint);
            double length    = calcPointDistance(startPoint,endPoint);
            if(DOUBLE_EQ_ZERO(distance)){
                //如果两个点距离过近
                continue;
            }
            if(distance>config.maxDist){
                //如果最近两点距离相差太大,则重开一个轨迹
//                printf("非法数据的距离为%f\n",distance);
                if(segments.size()>0){
                    tracks.push_back(segments);
                    segments.clear();
                }
                segment.reset();
            }else{
                if(maxDist>config.maxDeviateDistance||variance>config.maxVariance){
                    //如果距离大于阈值或距离方差大于阈值,则重开一个线段
                    segments.push_back(segment);
                    segment.reset();
                    segment.points.push_back(lastPoint);
                }else{
                    //还在正常的线段上
                    segment.lParam          = lParam;
                    segment.lParam.maxDist  = maxDist;
                    segment.lParam.variance = variance;
                    segment.lParam.distance = distance;
                    segment.lParam.length   = length;
                }
            }
        }
        segment.points.push_back(point);
        lastPoint = point;
    }
    //最后一个线段
    if(segment.points.size()>1){
        segments.push_back(segment);
        tracks.push_back(segments);
    }
}
