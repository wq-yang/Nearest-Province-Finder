// Copyright 2021 Wenqiang Yang, Jiaqi Zhang
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <utility>
#include <vector>
using namespace std;
const int MAX = 1<<22;
const int K = 7;
const double R = 6371000;
int RECORDS;
struct Place {
    string state, county;
    double lat, lon; // latitude and longitude
    double minLat, minLon, maxLat, maxLon;// min and max coordinates among descendants
    int left, right; // children id
} places[MAX];
// find the nearest K places for each query. pair: { distance, id }
priority_queue<pair<int, int>, vector<pair<int, int> >, less<pair<int, int> > > pq;
inline bool validLat(int lat) { return -90<=lat && lat<=90; }
inline bool validLon(int lon) { return -180<=lon && lon<=180; }
// equirectangular approx. distance = sqrt(sq_dist(c1, c2)) * R
inline double sq_dist(const double lat1, const double lon1, const double lat2, const double lon2) {
    double x = (lon2-lon1) * cos((lat1+lat2)/2), y = lat2-lat1;
    return x*x+y*y;
}
// heuristic function. the min distance between the queired point and the rectangle that subtree locates
inline double h(const Place& c, const double lat, const double lon) {
    double x = lon < c.minLon ? c.minLon - lon
            : c.maxLon < lon ? lon - c.maxLon : 0;
    double y = lat < c.minLat ? c.minLat - lat
            : c.maxLat < lat ? lat - c.maxLat : 0;
    if (lat < c.minLat) {
        x *= cos((lat+c.minLat)/2);
    } else if (c.maxLat < lat) {
        x *= cos((lat+c.maxLat)/2);
    } else {
        x *= cos(lat);
    }
    return x*x+y*y;
}

void readFile(char *infile) {
    ifstream fin(infile);
    if (!fin) {
        cout << "\033[1;31mDid not find the input file! \033[0m\n";
        exit(1);
    }
    string line;
    getline(fin, line); // skip the first line
    int i=0;
    while (i<MAX) {
        if (!getline(fin, line, '\t')) break;
        places[i].state = line;
        if (!getline(fin, line, '\t')) break;
        places[i].county = line;
        try {
            if (!getline(fin, line, '\t')) break;
            places[i].lat = stod(line);
        } catch(const std::exception& e) {
            cout << i << ": " << line << endl;
        }
        try {
            if (!getline(fin, line)) break;
            places[i].lon = stod(line);
        } catch(const std::exception& e) {
            cout << i << ": " << line << endl;
        }
        # if 0
        if (!validLat(places[i].lat) || !validLon(places[i].lon)) {
            cout << "invalid: i=" << i << ", county=" << places[i].county
            << ", lat = " << places[i].lat
            << ", lon = " << places[i].lon << endl;
        }
        # endif
        ++i;
    }
    RECORDS = i;
}
void maintain(Place& c) {
    c.minLat = c.maxLat = c.lat;
    c.minLon = c.maxLon = c.lon;
    if (c.left != -1) {
        c.minLat = fmin(c.minLat, places[c.left].minLat);
        c.minLon = fmin(c.minLon, places[c.left].minLon);
        c.maxLat = fmax(c.maxLat, places[c.left].maxLat);
        c.maxLon = fmax(c.maxLon, places[c.left].maxLon);
    }
    if (c.right != -1) {
        c.minLat = fmin(c.minLat, places[c.right].minLat);
        c.minLon = fmin(c.minLon, places[c.right].minLon);
        c.maxLat = fmax(c.maxLat, places[c.right].maxLat);
        c.maxLon = fmax(c.maxLon, places[c.right].maxLon);
    }
}
// build balanced kd-tree.
int build(int l, int r) {
    if (l>r) return -1;
    int mid = l + (r-l)/2;
    double mean1=0, mean2=0, var1=0, var2=0;
    for (int i=l; i<=r; ++i) {
        mean1 += places[i].lat;
        mean2 += places[i].lon;
    }
    mean1 /= r-l+1;
    mean2 /= r-l+1;
    for (int i=l; i<=r; ++i) {
        var1 += (places[i].lat-mean1) * (places[i].lat-mean1);
        var2 += (places[i].lon-mean2) * (places[i].lon-mean2);
    }
    // cut at the dimension with greatest variance
    if (var1 >= var2) {
        nth_element(places+l, places+mid, places+r+1, [](Place &c1, Place &c2) { return c1.lat < c2.lat; });
    } else {
        nth_element(places+l, places+mid, places+r+1, [](Place &c1, Place &c2) { return c1.lon < c2.lon; });
    }
    places[mid].left = build(l, mid-1);
    places[mid].right = build(mid+1, r);
    maintain(places[mid]);
    return mid;
}
void query(int l, int r, int lat, int lon) {
    if (l > r) return;
    int mid = l + (r-l)/2;
    Place cur = places[mid];
    double sqd = sq_dist(cur.lat, cur.lon, lat, lon);
    if (pq.top().first > sqd) {
        if (pq.size() >= K) pq.pop();
        pq.push({ sqd, mid });
    }
    // search the left and right subtree heuristically
    double sqdl = cur.left == -1 ? INFINITY : h(places[cur.left], lat, lon),
        sqdr = cur.right == -1 ? INFINITY : h(places[cur.right], lat, lon);
    if (sqdl < sqdr && sqdl < pq.top().first) { // search left first, then right if necessary
        query(l, mid-1, lat, lon);
        if (sqdr < pq.top().first) query(mid+1, r, lat, lon);
    } else if (sqdr <= sqdl && sqdr < pq.top().first) { // search right first, then left if necessary
        query(mid+1, r, lat, lon);
        if (sqdl < pq.top().first) query(l, mid-1, lat, lon);
    }
}
int main(int argc, char *argv[]) {
    readFile(argv[1]);
    build(0, RECORDS-1);
    while (true) {
        double lat, lon;
        cout << "\033[;34mPlease input latitude and longitude, separated by space... To quit, press Ctrl+C\033[0m\n";
        cin >> lat >> lon;
        if (!validLat(lat) || !validLon(lon)) {
            cout << "\033[1;31m Invalid input! Please try Again...\033[0m\n";
            continue;
        }
        cout << "\033[;33mQuerying { latitude=" << lat << ", longitude=" << lon << " }...\033[0m\n";
        pq.push({INFINITY, -1});
        query(0, RECORDS-1, lat, lon);
        cout << "The " << K << " nearest places are:\n";
        while (!pq.empty()) {
            int id = pq.top().second;
            double dist = pq.top().first;
            if (id != -1) {
                cout << "    " << places[id].county << ", " << places[id].state
                    << "(" << places[id].lat << ", " << places[id].lon << "), ";
                cout << "distance = " << sqrt(dist)*R << "km";
            }
            if (pq.size() > 1) cout << ",\n";
            else cout << ".\n";
            pq.pop();
        }
    }
}
