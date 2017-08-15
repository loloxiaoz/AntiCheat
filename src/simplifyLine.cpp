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
    lTPoint.x       = (point.longitude-box.xMin)*50000;
    lTPoint.y       = (point.latitude-box.yMin)*50000;
    return lTPoint;
}

TPoint SimplifyLine::local2world(BBox2D box, LTPoint lTPoint)
{
    TPoint tPoint;
    tPoint.seqNo       = lTPoint.seqNo;
    tPoint.longitude   = lTPoint.x/100000+box.xMin;
    tPoint.latitude    = lTPoint.y/100000+box.yMin;
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
        sum += distance*distance;
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

void SimplifyLine::simplifyTrack(rgConfig config,vector<LTPoint>& inputPoints,Segments& segments)
{
    //当前线段
    Segment segment;
    LTPoint lastPoint;
    for(int i=0; i<inputPoints.size(); i++){
        LTPoint point = inputPoints[i];
        //如果不是第一个点
        if(segment.points.size()>=1){
            LineParam lParam = calcLine(segment.points[0],point);
            double maxDist   = calcMaxDistance(lParam,segment.points);
            double variance  = calcVariance(lParam,segment.points);
            if(maxDist>config.maxDistance||variance>config.maxVariance){
                //如果距离大于阈值或距离方差大于阈值,则重开一个线段
                segments.push_back(segment);
                segment.reset();
                segment.points.push_back(lastPoint);
                segment.points.push_back(point);
                continue;
            }else{
                segment.points.push_back(point);
                lParam.maxDist  = maxDist;
                lParam.variance = variance;
                segment.lParam  = lParam;
            }
        }else{
            segment.points.push_back(point);
        }
        lastPoint = point;
    }
}
