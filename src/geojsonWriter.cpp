#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "geojsonWriter.h"

using namespace std;

GeojsonWriter::GeojsonWriter()
{

}

int GeojsonWriter::saveFile(const char* outPathName,Json::Value root)
{
    FILE* pfile = fopen(outPathName,"w+");
    if(pfile==NULL){
        return -1;
    }
    Json::StyledWriter writer;
    string str = writer.write(root);
    fprintf(pfile,"%s",str.c_str());
    fclose(pfile);
    return 1;
}

Json::Value GeojsonWriter::createFrame(const char* outPathName)
{
    Json::Value root;
    root["type"]        = "FeatureCollection";
    root["features"].resize(0);
    return root;
}

void GeojsonWriter::initFile(const char* outPathName)
{
    Json::Value root = createFrame(outPathName);
    saveFile(outPathName,root);
}

int GeojsonWriter::appendLine(const char* outPathName,vector<TPoint>& points)
{
    struct stat fileInfo;
    if(stat(outPathName,&fileInfo)==-1&&errno==ENOENT){
       initFile(outPathName);
    }
    string jsonStr(outPathName);
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(jsonStr, root)){
        return -1;
    }
    Json::Value coordinates;
    for(int i=0; i<points.size();i++){
        Json::Value line;
        line.append(points[i].latitude);
        line.append(points[i].longitude);
        coordinates.append(line);
    }
    Json::Value geometry;
    geometry["type"]        = "LineString";
    geometry["coordinates"] = coordinates;
    Json::Value properties;
    properties["prop0"]     = "value0";
    Json::Value feature;
    feature["type"]         = "Feature";
    feature["geometry"]     = geometry;
    feature["properties"]   = properties;
    root["features"].append(feature);
    saveFile(outPathName,root);
    return 1;
}
