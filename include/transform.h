
class Transform
{
public:

    void gcj2wgs84(double wgLat, double wgLon, double &mgLat, double &mgLon);
    bool outOfChina(double lat, double lon);
    double transformLat(double x, double y);
    double transformLon(double x, double y);

    const double pi = 3.14159265358979324;
    const double a = 6378245.0;
    const double ee = 0.00669342162296594323;
};

