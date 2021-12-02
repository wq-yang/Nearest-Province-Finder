// Copyright 2021 Wenqiang Yang, Jiaqi Zhang
// Reference: HW5_codes by Prof. Brower; oi-wiki.org/ds/kdt
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
constexpr int RECORDS = 1<<17;
constexpr int LEN_ST = 3;
constexpr int LEN_CTY = 9;
constexpr int LEN_TEMP = 64;
struct County {
    char state[LEN_ST];
    char name[LEN_CTY];
    double lat;
    double lon;
};
int main(int argc, char *argv[]) {
    char *infile = argv[1];
    FILE *fin = fopen(infile, "r");
    if (fin == nullptr) {
        printf("Did not find input file!\n");
        exit(1);
    }
    double lat[RECORDS], lon[RECORDS];
    char state[RECORDS][LEN_ST], county[RECORDS][LEN_CTY];
    char temp[LEN_TEMP];
    fgets(temp, LEN_TEMP, fin);
    int i=0;
    County counties[RECORDS];
    while (i<RECORDS && fscanf(fin, "%s %s %lf %lf", counties[i].state, counties[i].name, &counties[i].lat, &counties[i].lon) != EOF) {
    ++i;
    }
    County c = counties[20];
    printf("%s %s %lf %lf\n", c.state, c.name, c.lat, c.lon);
}
