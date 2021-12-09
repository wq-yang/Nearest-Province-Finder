#include "httplib.h"
#include "../findKNearst.h"
// HTTP
int main(int argc, char** argv) {
    httplib::Server svr;
    readFile(argv[1]);
    K = 7; // default K
    build(0, RECORDS-1);
    svr.Get("/api/pos", [&](const httplib::Request &req, httplib::Response &res) {
        string data = "{\"pos\":[";
        double lat, lon, phi, lam;
        if (req.has_param("lat")) {
            lat = stold(req.get_param_value("lat"));
        }
        if (req.has_param("lng")) {
            lon = stold(req.get_param_value("lng"));
        }
        phi = degToRad(lat);
        lam = degToRad(lon);
        pq.push({INFINITY, -1});
        query(0, RECORDS-1, phi, lam);

        int i=pq.size();
        double lats[i], lons[i];
        while (!pq.empty()) {
            --i;
            Place& p = places[pq.top().second];
            lats[i] = p.lat;
            lons[i] = p.lon;
            pq.pop();
        }
        for (int i=0; i<K; ++i) {
            data += "{\"rank\":"+to_string(i)+",";
            data += "\"lat\":"+to_string(lats[i])+",";
            data += "\"lng\":"+to_string(lons[i])+"},";
        }
        data.pop_back();
        data += "]}";
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_content(data.c_str(), "application/json");
    });

    svr.listen("0.0.0.0", 3000);
}
