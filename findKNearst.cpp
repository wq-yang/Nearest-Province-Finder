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
const double R = 6371;
const double PI = 3.14159265358979;
int K;
int RECORDS;
struct Place {
    string state, county;
    double lat, lon; // latitude and longitude
    double phi, lam; // rad ver. of latitude and longitude
    double minPhi, minLam, maxPhi, maxLam;// min and max coordinates among descendants
    int left, right; // children id
} places[MAX];
// find the nearest K places for each query. pair: { distance, index }
priority_queue<pair<double, int>, vector<pair<double, int> >, less<pair<double, int> > > pq;

inline bool validLat(int lat) { return -90<=lat && lat<=90; }
inline bool validLon(int lon) { return -180<=lon && lon<=180; }
inline double degToRad(double deg) { return deg*PI/180; }
// equirectangular approx. distance = sqrt(sq_dist(c1, c2)) * R
inline double sq_dist(const double phi1, const double lam1, const double phi2, const double lam2) {
    double x = (lam2-lam1) * cos((phi1+phi2)/2), y = phi2-phi1;
    return x*x+y*y;
}
// heuristic function. the min distance between the queired point and the rectangle that subtree locates
inline double h(const Place& c, const double phi, const double lam) {
    double x = lam < c.minLam ? c.minLam - lam
            : c.maxLam < lam ? lam - c.maxLam : 0;
    double y = phi < c.minPhi ? c.minPhi - phi
            : c.maxPhi < phi ? phi - c.maxPhi : 0;
    if (phi < c.minPhi) {
        x *= cos((phi+c.minPhi)/2);
    } else if (c.maxPhi < phi) {
        x *= cos((phi+c.maxPhi)/2);
    } else {
        x *= cos(phi);
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
            places[i].phi = degToRad(places[i].lat);
        } catch(const std::exception& e) {
            cout << i << ": " << line << endl;
        }
        try {
            if (!getline(fin, line)) break;
            places[i].lon = stod(line);
            places[i].lam = degToRad(places[i].lon);
        } catch(const std::exception& e) {
            cout << i << ": " << line << endl;
        }
        if (!validLat(places[i].lat) || !validLon(places[i].lon)) {
            cout << "invalid: i=" << i << ", county=" << places[i].county
            << ", lat = " << places[i].lat
            << ", lon = " << places[i].lon << endl;
        }
        ++i;
    }
    RECORDS = i;
}
void maintain(Place& c) {
    c.minPhi = c.maxPhi = c.phi;
    c.minLam = c.maxLam = c.lam;
    if (c.left != -1) {
        c.minPhi = fmin(c.minPhi, places[c.left].minPhi);
        c.minLam = fmin(c.minLam, places[c.left].minLam);
        c.maxPhi = fmax(c.maxPhi, places[c.left].maxPhi);
        c.maxLam = fmax(c.maxLam, places[c.left].maxLam);
    }
    if (c.right != -1) {
        c.minPhi = fmin(c.minPhi, places[c.right].minPhi);
        c.minLam = fmin(c.minLam, places[c.right].minLam);
        c.maxPhi = fmax(c.maxPhi, places[c.right].maxPhi);
        c.maxLam = fmax(c.maxLam, places[c.right].maxLam);
    }
}
// build balanced kd-tree.
int build(int l, int r) {
    if (l>r) return -1;
    int mid = l + (r-l)/2;
    double mean1=0, mean2=0, var1=0, var2=0;
    for (int i=l; i<=r; ++i) {
        mean1 += places[i].phi;
        mean2 += places[i].lam;
    }
    mean1 /= r-l+1;
    mean2 /= r-l+1;
    for (int i=l; i<=r; ++i) {
        var1 += (places[i].phi-mean1) * (places[i].phi-mean1);
        var2 += (places[i].lam-mean2) * (places[i].lam-mean2);
    }
    // cut at the dimension with greatest variance
    if (var1 >= var2) {
        nth_element(places+l, places+mid, places+r+1, [](const Place &c1, const Place &c2) { return c1.phi < c2.phi; });
    } else {
        nth_element(places+l, places+mid, places+r+1, [](const Place &c1, const Place &c2) { return c1.lam < c2.lam; });
    }
    places[mid].left = build(l, mid-1);
    places[mid].right = build(mid+1, r);
    maintain(places[mid]);
    return mid;
}
void query(int l, int r, double phi, double lam) {
    if (l > r) return;
    int mid = l + (r-l)/2;
    Place cur = places[mid];
    double sqd = sq_dist(cur.phi, cur.lam, phi, lam);
    if (pq.top().first > sqd) {
        if (pq.size() >= max(5, K)) pq.pop();
        pq.push({ sqd, mid });
    }
    // search the left and right subtree heuristically
    double sqdl = cur.left == -1 ? INFINITY : h(places[cur.left], phi, lam),
        sqdr = cur.right == -1 ? INFINITY : h(places[cur.right], phi, lam);
    if (sqdl < sqdr && sqdl < pq.top().first) { // search left first, then right if necessary
        query(l, mid-1, phi, lam);
        if (sqdr < pq.top().first) query(mid+1, r, phi, lam);
    } else if (sqdr <= sqdl && sqdr < pq.top().first) { // search right first, then left if necessary
        query(mid+1, r, phi, lam);
        if (sqdl < pq.top().first) query(l, mid-1, phi, lam);
    }
}
int main(int argc, char *argv[]) {
    cout << "\033[1;33mLoading file...\033[0m\n";
    readFile(argv[1]);
    cout << "\033[1;36mPlease input K...\033[0m";
    cin >> K;
    cout << "\033[1;33mPreprocessing...\033[0m\n";
    build(0, RECORDS-1);
    while (true) {
        double lat, lon, phi, lam;
        cout << "\033[1;36mPlease input latitude and longitude, separated by space... To quit, press Ctrl+C\033[0m\n";
        cin >> lat >> lon;
        phi = degToRad(lat);
        lam = degToRad(lon);
        if (!validLat(lat) || !validLon(lon)) {
            cout << "\033[1;31mInvalid input! Please try Again...\033[0m\n";
            continue;
        }
        cout << "\033[1;33mQuerying { latitude=" << lat << ", longitude=" << lon << " }...\033[0m\n";
        pq.push({INFINITY, -1});
        query(0, RECORDS-1, phi, lam);
        cout << "The " << K << " nearest places are:\n";
        int i=0;
        while (!pq.empty() && i++<K) {
            int id = pq.top().second;
            double dist = pq.top().first;
            if (id != -1) {
                cout << "    " << places[id].county << ", " << places[id].state
                    << "(" << places[id].lat << ", " << places[id].lon << "), ";
                cout << "distance = " << sqrt(dist)*R << "km";
            }
            cout << endl;
            pq.pop();
        }
    }
}
