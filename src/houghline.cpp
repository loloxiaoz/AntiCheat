#include "houghline.h"

#include "stdio.h"
#include <math.h>
#include <vector>
#include "string.h"
#include <algorithm>

#ifndef ROUND
#define ROUND (0.5F)
#endif

#ifndef PI
#define PI (3.1415926535897932384626433832795)
#endif

#ifndef INT_MAX
#define INT_MAX       2147483647
#endif

using namespace std;


HoughLine::HoughLine()
{

}
//*******************************************************************************
//*******************************************************************************
//standard hough
typedef struct houghCmpGtStruct
{
    const int* aux;

    hough_cmp_gt_t(const int* _aux) : aux(_aux)
    {
    }

    bool operator()(int l1, int l2) const
    {
        return aux[l1] > aux[l2] || (aux[l1] == aux[l2] && l1 < l2);
    }
}houghCmpGt;

typedef struct houghConfigStruct
{
    float   rho;
    float   theta;
    int     threshold;
    int     lineLength;
    int     lineGap;
    int     linesMax;
}houghConfig;


static void _houghLinesProbabilistic(houghConfig config,vector<pointFloat> inputLines, vector<pointFloat> outputLines)
{
    int i, j;
    float irho = 1 / rho;
    int offset, offy;

    int numangle = (int)round(PI/config.theta);
    int numrho = (int)round(((w + h) * 2 + 1) / rho);

    int *_accum = new int[numangle*numrho]();
    unsigned char *_mask = new unsigned char[h*w];


    vector<float> trigtab(numangle * 2);

    for (int n = 0; n < numangle; n++)
    {
        offset = n * 2;
        trigtab[offset] = (float)(cos((double)n*theta) * irho);
        trigtab[offset + 1] = (float)(sin((double)n*theta) * irho);
    }
    const float* ttab = &trigtab[0];
    unsigned char* mdata0 = _mask;
    vector<point_int_t> nzloc;

    // stage 1. collect non-zero image points
    for (i = 0; i < h; i++)
    {
        offy = i*w;
        for (j = 0; j < w; j++)
        {
            if (_image[offy + j])
            {
                _mask[offy + j] = (unsigned char)1;
                nzloc.push_back({ j, i });
            }
            else
                _mask[offy + j] = 0;
        }
    }

    int count = (int)nzloc.size();

    // stage 2. process all the points in random order
    for (; count > 0; count--)
    {
        // choose random point out of the remaining ones
        int idx = rand() % count;
        int max_val = threshold - 1, max_n = 0;
        point_int_t point = nzloc[idx];
        point_int_t line_end[2];
        float a, b;
        int* adata = _accum;
        i = point.y, j = point.x;
        int k, x0, y0, dx0, dy0, xflag;
        int good_line;
        const int shift = 16;

        // "remove" it by overriding it with the last element
        nzloc[idx] = nzloc[count - 1];

        // check if it has been excluded already (i.e. belongs to some other line)
        if (!mdata0[i*w + j])
            continue;

        // update accumulator, find the most probable line
        for (int n = 0; n < numangle; n++, adata += numrho)
        {
            int r = (int)round(j * ttab[n * 2] + i * ttab[n * 2 + 1]);
            r += (numrho - 1) / 2;
            int val = ++adata[r];
            if (max_val < val)
            {
                max_val = val;
                max_n = n;
            }
        }

        // if it is too "weak" candidate, continue with another point
        if (max_val < threshold)
            continue;

        // from the current point walk in each direction
        // along the found line and extract the line segment
        a = -ttab[max_n * 2 + 1];
        b = ttab[max_n * 2];
        x0 = j;
        y0 = i;
        if (fabs(a) > fabs(b))
        {
            xflag = 1;
            dx0 = a > 0 ? 1 : -1;
            dy0 = (int)round(b*(1 << shift) / fabs(a));
            y0 = (y0 << shift) + (1 << (shift - 1));
        }
        else
        {
            xflag = 0;
            dy0 = b > 0 ? 1 : -1;
            dx0 = (int)round(a*(1 << shift) / fabs(b));
            x0 = (x0 << shift) + (1 << (shift - 1));
        }

        for (k = 0; k < 2; k++)
        {
            int gap = 0, x = x0, y = y0, dx = dx0, dy = dy0;

            if (k > 0)
                dx = -dx, dy = -dy;

            // walk along the line using fixed-point arithmetics,
            // stop at the image border or in case of too big gap
            for (;; x += dx, y += dy)
            {
                unsigned char* mdata = NULL;
                int i1, j1;

                if (xflag)
                {
                    j1 = x;
                    i1 = y >> shift;
                }
                else
                {
                    j1 = x >> shift;
                    i1 = y;
                }

                if (j1 < 0 || j1 >= w || i1 < 0 || i1 >= h)
                    break;

                mdata = mdata0 + i1*w + j1;

                // for each non-zero point:
                //    update line end,
                //    clear the mask element
                //    reset the gap
                if (*mdata)
                {
                    gap = 0;
                    line_end[k].y = i1;
                    line_end[k].x = j1;
                }
                else if (++gap > lineGap)
                    break;
            }
        }

        good_line = abs(line_end[1].x - line_end[0].x) >= lineLength ||
            abs(line_end[1].y - line_end[0].y) >= lineLength;

        for (k = 0; k < 2; k++)
        {
            int x = x0, y = y0, dx = dx0, dy = dy0;

            if (k > 0)
                dx = -dx, dy = -dy;

            // walk along the line using fixed-point arithmetics,
            // stop at the image border or in case of too big gap
            for (;; x += dx, y += dy)
            {
                unsigned char* mdata = NULL;
                int i1, j1;

                if (xflag)
                {
                    j1 = x;
                    i1 = y >> shift;
                }
                else
                {
                    j1 = x >> shift;
                    i1 = y;
                }

                mdata = mdata0 + i1*w + j1;

                // for each non-zero point:
                //    update line end,
                //    clear the mask element
                //    reset the gap
                if (*mdata)
                {
                    if (good_line)
                    {
                        adata = _accum;
                        for (int n = 0; n < numangle; n++, adata += numrho)
                        {
                            int r = (int)round(j1 * ttab[n * 2] + i1 * ttab[n * 2 + 1]);
                            r += (numrho - 1) / 2;
                            adata[r]--;
                        }
                    }
                    *mdata = 0;
                }

                if (i1 == line_end[k].y && j1 == line_end[k].x)
                    break;
            }
        }

        if (good_line)
        {
            line_float_t _lr = { (float)line_end[0].x, (float)line_end[0].y, (float)line_end[1].x, (float)line_end[1].y };
            _lines.push_back(_lr);
            if ((int)_lines.size() >= linesMax)
                return;
        }
    }

    delete[]_accum;
    delete[]_mask;
}


/*
@function    HoughLineDetector
@param       [in]      HoughRho:                  distance resolution of the accumulator in pixels
@param       [in]      HoughTheta:                angle resolution of the accumulator in radians
@param       [in]      MinThetaLinelength:        standard: for standard and multi-scale hough transform, minimum angle to check for lines.
                                                  propabilistic: minimum line length. Line segments shorter than that are rejected
@param       [in]      MaxThetaGap:               standard: for standard and multi-scale hough transform, maximum angle to check for lines
                                                  propabilistic: maximum allowed gap between points on the same line to link them
@param       [in]      HoughThresh:               accumulator threshold parameter. only those lines are returned that get enough votes ( >threshold ).
@param       [in/out]  lines:                     result
@return：										  0:ok; 1:error
propabilistic:  try (src,w,h,scalex,scaley,70,150, 1, PI/180, 30, 10, 80, HOUGH_LINE_STANDARD, bbox, line)
*/
int HoughLineDetector(float HoughRho, float HoughTheta, float MinThetaLinelength, float MaxThetaGap, int HoughThresh,
    vector<line_float_t> &lines);
{
    _houghLinesProbabilistic(
        (float)rho, (float)theta, threshold, (int)round(minLineLength), (int)round(maxGap), INT_MAX, lines);

}
