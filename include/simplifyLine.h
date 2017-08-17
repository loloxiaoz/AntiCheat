#include <vector>
#include "recordLoader.h"
using namespace std;

typedef struct LocalTrackPoint
{
    int    seqNo;
    double x;
    double y;
}LTPoint;

typedef struct BoundingBox2D
{
    double xMin;
    double xMax;
    double yMin;
    double yMax;
    double width;
    double height;

    BoundingBox2D(){
        xMin = 0;
        xMax = 0;
        yMin = 0;
        yMax = 0;
        width   = 0;
        height  = 0;
    }
}BBox2D;

typedef struct LineParam
{
    double   a;
    double   b;
    double   c;
    double   maxDist;
    double   variance;

    void reset(){
        a           = 0;
        b           = 0;
        c           = 0;
        maxDist     = 0;
        variance    = 0;
    }

}LineParam;

typedef struct regressionConfig
{
    double   minPoints;
    double   maxDistance;
    double   maxVariance;

    regressionConfig(){
        minPoints       = 4;
        maxDistance     = 8;
        maxVariance     = 4;
    }

}rgConfig;

typedef struct SegmentStruct
{
    //线性回归的最大距离与方差
    LineParam        lParam;
    //当前直线的点集
    vector<LTPoint>  points;

    void reset(){
        lParam.reset();
        points.clear();
    }
}Segment;

typedef vector<Segment> Segments;

class SimplifyLine
{
public:
    SimplifyLine();
    BBox2D findBoundingBox2D(vector<TPoint> points);
    LTPoint world2Local(BBox2D box, TPoint point);
    TPoint local2world(BBox2D box, LTPoint lTPoint);
    double calcDistance(LineParam lParam,LTPoint point);
    double calcMaxDistance(LineParam lParam,vector<LTPoint> points);
    double calcVariance(LineParam lParam,vector<LTPoint> points);
    LineParam calcLine(LTPoint startPoint,LTPoint endPoint);
    double calcPointDistance(LTPoint startPoint,LTPoint endPoint);
    bool isValidSegment(Segment& segment);
    void simplifyTrack(rgConfig config,vector<LTPoint>& inputPoints,Segments& segments);
};
