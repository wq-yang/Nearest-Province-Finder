// Copyright 2021 Wenqiang Yang, Jiaqi Zhang
// Reference: HW5_codes by Prof. Brower; oi-wiki.org/ds/kdt
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
constexpr int RECORDS = 1<<22;
constexpr int LEN_ST = 3; // strlen of state
constexpr int LEN_CTY = 9; // strlen of county
constexpr int LEN_TEMP = 64; // used for skip the first line in input file
struct County {
    char state[LEN_ST], name[LEN_CTY];
    double lat, lon;
} counties[RECORDS];
int main(int argc, char *argv[]) {
    char *infile = argv[1], outfile[strlen(infile)+5];
    strcpy(outfile, infile); strcat(outfile, "_out");
    FILE *fin = fopen(infile, "r");
    if (fin == nullptr) {
        printf("Did not find input file!\n");
        exit(1);
    }
    double lat[RECORDS], lon[RECORDS];
    char state[RECORDS][LEN_ST], county[RECORDS][LEN_CTY];
    char temp[LEN_TEMP];
    fgets(temp, LEN_TEMP, fin); // skip the first line
    int i=0;
    while (i<RECORDS && fscanf(fin, "%s %s %lf %lf", counties[i].state, counties[i].name, &counties[i].lat, &counties[i].lon) != EOF) {
        ++i;
    }
    fclose(fin);
    FILE *fout = fopen(outfile,"w");

    fclose(fout);
}
