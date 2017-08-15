#include <vector>
#include "json.h"
#include "recordLoader.h"

using namespace std;

class GeojsonWriter
{
public:
    GeojsonWriter();
    int appendLine(const char* outPathName,vector<TPoint>& points);
    int saveFile(const char* outPathName,Json::Value root);
    void initFile(const char* outPathName);

private:
    Json::Value createFrame(const char* outPathName);
};
