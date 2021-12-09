// Copyright 2021 Wenqiang Yang, Jiaqi Zhang
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include "findKNearst.h" // TODO: clean headers
using namespace std;

int main(int argc, char *argv[]) {
    cout << "\033[1;33mLoading file...\033[0m\n";
    readFile(argv[1]);
    cout << "\033[1;36mPlease input K: \033[0m";
    cin >> K;
    cout << "\033[1;33mPreprocessing...\033[0m\n";
    build(0, RECORDS-1);
    while (true) {
        double lat, lon, phi, lam;
        cout << "\033[1;36mPlease input latitude and longitude, separated by space.  To quit, press Ctrl+C\033[0m\n";
        cin >> lat >> lon;
        if (!validLat(lat) || !validLon(lon)) {
            cout << "\033[1;31mInvalid input! Please try Again.033[0m\n";
            continue;
        }
        phi = degToRad(lat);
        lam = degToRad(lon);
        cout << "\033[1;33mQuerying { latitude=" << lat << ", longitude=" << lon << " }...\033[0m\n";
        pq.push({INFINITY, -1});
        query(0, RECORDS-1, phi, lam);

        // get nearest places
        vector<pair<double, int> > nearest;
        while (!pq.empty()) {
            nearest.push_back({pq.top().first, pq.top().second});
            pq.pop();
        }
        // majority voting to find the state and county
        unordered_map<string, int> countyToCnt;
        for (int i=nearest.size()-1; i>=int(nearest.size()-5); --i) {
            int idx = nearest[i].second;
            string loc = places[idx].county + ", " + places[idx].state;
            countyToCnt[loc]++;
        }
        unordered_map<string, int>::iterator kv;
        for (kv = begin(countyToCnt); kv != end(countyToCnt); kv = next(kv)) {
            if (kv->second > 2) {
                cout << "It might be located at \033[1;31m" << kv->first << "\033[0m\n";
                break;
            }
        }
        if (kv == end(countyToCnt)) cout << "No majority, failed to guess the location.\n";
        // print K nearest reference points
        cout << "The " << K << " nearest places are:\n";
        for (int i=nearest.size()-1; i>=int(nearest.size()-K); --i) {
            int idx = nearest[i].second;
            double dist = nearest[i].first;
            cout << "\t\033[1;31m" << places[idx].county << ", " << places[idx].state
                << "\033[0m(" << places[idx].lat << ", " << places[idx].lon << "), "
                << "distance = " << sqrt(dist)*R << "km" << endl;
        }
    }
}
