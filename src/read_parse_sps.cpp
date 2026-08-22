#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "read_parse_sps.h"

using namespace std;

namespace sps {


    void parse_rcv_sps(string filename, vector<RcvStruct>& rcv_sps_vector)
    {
        ifstream file(filename);

        if (!file.is_open()) {
            printf("Error, could not open the file: %s\n", filename.c_str());
            exit(0);
        }
        string line;

        while (getline(file, line)) {
            string p_code = line.substr(24,2);
            if (p_code == "KL") continue;
            string elevation = line.substr(65, 10);
            float elev = all_of(elevation.begin(), elevation.end(), [](unsigned char c) {
                return std::isspace(c);}) ? 0.0 : stof(elevation);

            RcvStruct rcv_sps;
            rcv_sps.type = line.substr(0,1);
            rcv_sps.line = stoi(line.substr(1,10));
            rcv_sps.point = stoi(line.substr(11,10));
            rcv_sps.p_index = stoi(line.substr(21,3));
            rcv_sps.p_code = p_code;
            rcv_sps.easting = stof(line.substr(45,10));
            rcv_sps.northing = stof(line.substr(55,10));
            rcv_sps.elevation = elev;

            rcv_sps_vector.push_back(rcv_sps);
        }
        file.close();
        printf("SPS R records: %'llu\n", rcv_sps_vector.size());
    }

    void parse_src_sps(string filename, vector<SrcStruct>& src_sps_vector)
    {
        ifstream file(filename);

        if (!file.is_open()) {
            printf("Error, could not open the file: %s\n", filename.c_str());
            exit(0);
        }
        string line;

        while (getline(file, line)) {
            string p_code = line.substr(24,2);
            if (p_code == "KL") continue;
            string elevation = line.substr(65, 10);
            float elev = all_of(elevation.begin(), elevation.end(), [](unsigned char c) {
                return std::isspace(c);}) ? 0.0 : stof(elevation);

            SrcStruct src_sps;
            src_sps.type = line.substr(0,1);
            src_sps.line = stoi(line.substr(1,10));
            src_sps.point = stoi(line.substr(11,10));
            src_sps.p_index = stoi(line.substr(21,3));
            src_sps.p_code = p_code;
            src_sps.easting = stof(line.substr(45,10));
            src_sps.northing = stof(line.substr(55,10));
            src_sps.elevation = elev;

            src_sps_vector.push_back(src_sps);
        }
        file.close();
        printf("SPS S records: %'llu\n", src_sps_vector.size());
    }

    void parse_x_sps(string filename, vector<XStruct>& x_sps_vector)
    {
        ifstream file(filename);

        if (!file.is_open()) {
            printf("Error, could not open the file: %s\n", filename.c_str());
            exit(0);
        }
        string line;

        while (getline(file, line)) {
            XStruct x_sps;
            x_sps.type = line.substr(0,1);
            x_sps.src_line = stoi(line.substr(17,10));
            x_sps.src_point = stoi(line.substr(27,10));
            x_sps.src_index = stoi(line.substr(37,1));
            x_sps.chan_start = stoi(line.substr(38,5));
            x_sps.chan_end = stoi(line.substr(43,5));
            x_sps.chan_incr = stoi(line.substr(48,1));
            x_sps.rcv_line = stoi(line.substr(49,10));
            x_sps.rcv_point_start = stoi(line.substr(59,10));
            x_sps.rcv_point_end = stoi(line.substr(69,10));
            x_sps.rcv_index = stoi(line.substr(79,1));
            x_sps.tb = line.substr(80,20);

            x_sps_vector.push_back(x_sps);
        }
        file.close();
        printf("SPS X records: %'llu\n", x_sps_vector.size());
    }
}

