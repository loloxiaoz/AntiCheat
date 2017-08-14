#include <vector>

using namespace std;

typedef struct TrackPoint
{
    int    seqNo;
    double timestamp;
    double altitude;
    double latitude; ///维度
    double longitude; //经度
    double speed;
    int    status;
}TPoint;

typedef struct RunRecord
{
    double altitude_down;
    double altitude_up;
    double avr_pace;
    double avr_step_freq;
    double avr_step_length;

    double  disabled_duration;
    double  halted_duration;
    double  faint_distance;
    double  faint_duration;

    double  distance;
    double  duration;
    int     total_step_num;
    int     start_time;
    int     end_time;
    int     init_time;

    double max_altitude;
    double max_pace;
    double max_step_freq;
    double max_step_length;
    double min_altitude;
    double min_pace;
    double min_step_freq;
    double min_step_length;

    int    sensor_cheat_flag;
    int    cheat_flag;
    int    version;

    vector<TrackPoint> trackPoints;
}RunRecord;

class RecordLoader
{
public:
    RecordLoader();

    int load(const char* str,RunRecord* pRunRecord);
};
