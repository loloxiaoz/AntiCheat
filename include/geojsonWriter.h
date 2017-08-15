#include "json.h"

class GeojsonWriter
{
public:
    GeojsonWriter();
    void appendLine(const char* outPathName,vector<TPoint>& points);

}
