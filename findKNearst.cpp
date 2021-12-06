// Copyright 2021 Wenqiang Yang, Jiaqi Zhang
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <queue>
#include <utility>
#include <vector>
using std::less;
using std::nth_element;
using std::pair;
using std::priority_queue;
using std::vector;
const int RECORDS = 1<<22;
const int K = 7;
const int LEN_ST = 3; // strlen of state
const int LEN_CTY = 9; // strlen of county
struct County {
    char state[LEN_ST], name[LEN_CTY];
    double lat, lon; // latitude and longitude
    double minLat, minLon, maxLat, maxLon;// min and max coordinates among descendants
    int left, right; // children id
} counties[RECORDS];
// find the nearest K counties for each query. pair: { distance, id }
priority_queue<pair<int, int>, vector<pair<int, int> >, less<pair<int, int> > > pq;

// equirectangular approx. distance = sqrt(sq_dist(c1, c2)) * R
inline double sq_dist(const double lat1, const double lon1, const double lat2, const double lon2) {
    double x = (lon2-lon1) * cos((lat1+lat2)/2), y = lat2-lat1;
    return x*x+y*y;
}
// heuristic function. the min distance between the queired point and the rectangle that subtree locates
inline double h(const County& c, const double lat, const double lon) {
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
    FILE *fin = fopen(infile, "r");
    if (fin == nullptr) {
        printf("Did not find input file!\n");
        exit(1);
    }
    char temp[64];
    fgets(temp, 64, fin); // skip the first line
    int i=0;
    while (i<RECORDS && fscanf(fin, "%s %s %lf %lf", counties[i].state, counties[i].name, &counties[i].lat, &counties[i].lon) != EOF) {
        ++i;
    }
    fclose(fin);
}
void maintain(County& c) {
    c.minLat = c.maxLat = c.lat;
    c.minLon = c.maxLon = c.lon;
    if (c.left != -1) {
        c.minLat = fmin(c.minLat, counties[c.left].minLat);
        c.minLon = fmin(c.minLon, counties[c.left].minLon);
        c.maxLat = fmax(c.maxLat, counties[c.left].maxLat);
        c.maxLon = fmax(c.maxLon, counties[c.left].maxLon);
    }
    if (c.right != -1) {
        c.minLat = fmin(c.minLat, counties[c.right].minLat);
        c.minLon = fmin(c.minLon, counties[c.right].minLon);
        c.maxLat = fmax(c.maxLat, counties[c.right].maxLat);
        c.maxLon = fmax(c.maxLon, counties[c.right].maxLon);
    }
}
// build balanced kd-tree.
int build(int l, int r) {
    if (l>r) return -1;
    int mid = l + (r-l)/2;
    double mean1=0, mean2=0, var1=0, var2=0;
    for (int i=l; i<=r; ++i) {
        mean1 += counties[i].lat;
        mean2 += counties[i].lon;
    }
    mean1 /= r-l+1;
    mean2 /= r-l+1;
    for (int i=l; i<=r; ++i) {
        var1 += (counties[i].lat-mean1) * (counties[i].lat-mean1);
        var2 += (counties[i].lon-mean2) * (counties[i].lon-mean2);
    }
    // cut at the dimension with greatest variance
    if (var1 >= var2) {
        nth_element(counties+l, counties+mid, counties+r+1, [](County &c1, County &c2) { return c1.lat < c2.lat; });
    } else {
        nth_element(counties+l, counties+mid, counties+r+1, [](County &c1, County &c2) { return c1.lon < c2.lon; });
    }
    counties[mid].left = build(l, mid-1);
    counties[mid].right = build(mid+1, r);
    maintain(counties[mid]);
    return mid;
}
bool validLat(int lat) { return -90<=lat && lat<=90; }
bool validLon(int lon) { return -180<=lon && lon<=180; }
void query(int l, int r, int lat, int lon) {
    if (l > r) return;
    int mid = l + (r-l)/2;
    County cur = counties[mid];
    double sqd = sq_dist(cur.lat, cur.lon, lat, lon);
    if (pq.top().first > sqd) {
        if (pq.size() >= K) pq.pop();
        pq.push({ sqd, mid });
    }
    // search the left and right subtree heuristically
    double sqdl = cur.left == -1 ? INFINITY : h(counties[cur.left], lat, lon),
        sqdr = cur.right == -1 ? INFINITY : h(counties[cur.right], lat, lon);
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
        printf("Please input latitude and longitude, separated by space...\n");
        scanf("%lf %lf", &lat, &lon);
        if (!validLat(lat) || !validLon(lon)) {
            printf("Invalid input! Try Again...\n");
            continue;
        }
        printf("querying...\n");
        query(0, RECORDS-1, lat, lon);
        printf("The (at most) %d nearest counties are: ", K);
        while (!pq.empty()) {
            int id = pq.top().second;
            printf("%s ", counties[id].name);
            pq.pop();
        }
        printf("\n");
    }
}
