#include "json.h"
#include "recordLoader.h"

RecordLoader::RecordLoader()
{
}

int RecordLoader::load(const char * str,RunRecord* pRunRecord)
{
    string json_str(str);
    Json::Reader reader;
    Json::Value root;
    if (!reader.parse(json_str, root)){
        return -1;
    }
    pRunRecord->altitude_down = root["altitude_down"].asDouble();
    pRunRecord->altitude_up = root["altitude_up"].asDouble();
    pRunRecord->avr_pace = root["avr_pace"].asDouble();
    pRunRecord->avr_step_freq = root["avr_step_freq"].asDouble();
    pRunRecord->avr_step_length = root["avr_step_length"].asDouble();

    pRunRecord->disabled_duration = root["disabled_duration"].asDouble();
    pRunRecord->halted_duration = root["halted_duration"].asDouble();
    pRunRecord->faint_distance = root["faint_distance"].asDouble();
    pRunRecord->faint_duration = root["faint_duration"].asDouble();

    pRunRecord->distance = root["distance"].asDouble();
    pRunRecord->duration = root["duration"].asDouble();
    pRunRecord->total_step_num = root["total_step_num"].asInt();
    pRunRecord->start_time = root["start_time"].asInt();
    pRunRecord->end_time = root["end_time"].asInt();
    pRunRecord->init_time = root["init_time"].asInt();

    pRunRecord->max_altitude = root["max_altitude"].asDouble();
    pRunRecord->max_pace = root["max_pace"].asDouble();
    pRunRecord->max_step_freq = root["max_step_freq"].asDouble();
    pRunRecord->max_step_length = root["max_step_length"].asDouble();
    pRunRecord->min_altitude = root["min_altitude"].asDouble();
    pRunRecord->min_pace = root["min_pace"].asDouble();
    pRunRecord->min_step_freq = root["min_step_freq"].asDouble();
    pRunRecord->min_step_length = root["min_step_length"].asDouble();

    pRunRecord->sensor_cheat_flag = root["sensor_cheat_flag"].asInt();
    pRunRecord->cheat_flag = root["cheat_flag"].asInt();
    pRunRecord->version = root["version"].asInt();

    TPoint tPoint;
    Json::Value trackPoints = root["trackpoints"];
    for(int i = 0; i < trackPoints.size(); i++){
        Json::Value point = trackPoints[i];
        tPoint.seqNo        = point["seq_no"].asInt();
        tPoint.timestamp    = point["timestamp"].asDouble();
        tPoint.longitude    = point["longitude"].asDouble();
        tPoint.latitude     = point["latitude"].asDouble();
        tPoint.altitude     = point["altitude"].asDouble();
        tPoint.speed        = point["speed"].asDouble();
        tPoint.status       = point["status"].asInt();
        pRunRecord->trackPoints.push_back(tPoint);

    }
    return 0;
}
