#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>
#include <cmath>

using namespace std;

float deg_to_rad = acos(-1.0) / 180.0;
float rad_to_deg = 180 / acos(-1.0);
tuple<double, double, float> origin(
    701692.0, 3450142.50, 90.0 * deg_to_rad
);
int nb_bin_sp = 780;
int nb_bin_rp = 1280;
double bin_sp_int = -25.0;
double bin_rp_int = 12.5;
float max_offset = 1000.0;
int base_pnt = 10'000;

struct RcvSpsStruct {
    string type;
    int line;
    int point;
    int p_index;
    string p_code;
    double easting;
    double northing;
    float elevation;
};

struct SrcSpsStruct {
    string type;
    int line;
    int point;
    int p_index;
    string p_code;
    double easting;
    double northing;
    float elevation;
};

struct XSpsStruct {
    string type;
    int src_line;
    int src_point;
    int src_index;
    int chan_start;
    int chan_end;
    int chan_incr;
    int rcv_line;
    int rcv_point_start;
    int rcv_point_end;
    int rcv_index;
    string tb;
};

struct BinStruct {
    int id;
    int bin_sp;
    int bin_rp;
    double easting;
    double northing;
    int bin_count;
};

tuple<double, double> xy_rotation_clockwise(double x, double y, float azimuth) {
    float cos_azim = cos(azimuth);
    float sin_azim = sin(azimuth);
    double x_trans = x * cos_azim + y * sin_azim;
    double y_trans = -x * sin_azim + y * cos_azim;
    return make_tuple(x_trans, y_trans);
}

tuple<double, double> calc_bin_coordinate(
    tuple<double, double, float> origin, double src_distance, double rcv_distance
) {
    auto coord = xy_rotation_clockwise(src_distance, rcv_distance, get<2>(origin));
    double x = get<0>(coord) + get<0>(origin);
    double y = get<1>(coord) + get<1>(origin);
    return make_tuple(x, y);
}

tuple<int, int> calc_bin_index(
     double x, double y, tuple<double, double, float> origin, 
     double bin_src_int, double bin_rcv_int
) {
    x -= get<0>(origin);
    y -= get<1>(origin);
    auto coord = xy_rotation_clockwise(x, y, -get<2>(origin));
    int index_sp = round(get<0>(coord) / bin_src_int);
    int index_rp = round(get<1>(coord) / bin_rcv_int);
    return make_tuple(index_sp, index_rp);
}

int calc_point_index(int i, int j) {
    return (base_pnt + i) * base_pnt * 10 + (base_pnt + j);
}

void create_bins(vector<BinStruct>& bins) {
    for (int i=0; i < nb_bin_sp; i++) {
        double src_distance = i * bin_sp_int;
        for (int j=0; j < nb_bin_rp; j++) {
            double rcv_distance = j * bin_rp_int;
            auto coord = calc_bin_coordinate(origin, src_distance, rcv_distance);
            BinStruct bin;
            bin.id = calc_point_index(i, j);
            bin.bin_sp = i;
            bin.bin_rp = j;
            bin.easting = get<0>(coord);
            bin.northing = get<1>(coord);
            bin.bin_count = 0;
            bins.push_back(bin);
        }
    }
}

void parse_rcv_sps(string filename, vector<RcvSpsStruct>& rcv_sps_vector) 
{
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open the file!" << endl;
    }
    string line;
    
    while (getline(file, line)) {
        string p_code = line.substr(24,2);
        if (p_code == "KL") continue;
        string elevation = line.substr(65, 10);
        float elev = all_of(elevation.begin(), elevation.end(), [](unsigned char c) { 
            return std::isspace(c);}) ? 0.0 : stof(elevation);

        RcvSpsStruct rcv_sps;
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
}

void parse_src_sps(string filename, vector<SrcSpsStruct>& src_sps_vector) 
{
    ifstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not open the file!" << endl;
    }
    string line;
    
    while (getline(file, line)) {
        string p_code = line.substr(24,2);
        if (p_code == "KL") continue;
        string elevation = line.substr(65, 10);
        float elev = all_of(elevation.begin(), elevation.end(), [](unsigned char c) { 
            return std::isspace(c);}) ? 0.0 : stof(elevation);

        SrcSpsStruct src_sps;
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
}

void parse_x_sps(string filename, vector<XSpsStruct>& x_sps_vector) 
{
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error: Could not open the file!" << endl;
    }
    string line;
    
    while (getline(file, line)) {
        XSpsStruct x_sps;
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
}

void save_bins_csv(string filename, const vector<BinStruct>& bins) {
    string line;
    ofstream outfile(filename);
    line = "id, bin_sp, bin_rp, easting, northing, bin_count\n";
    outfile << line;
    for (BinStruct bin : bins) {
        string id = to_string(bin.id) + ", ";
        string sp = to_string(bin.bin_sp) + ", ";
        string rp = to_string(bin.bin_rp) + ", ";
        string e = to_string(bin.easting) + ", ";
        string n = to_string(bin.northing) + ", ";
        string bc = to_string(bin.bin_count) + "\n";
        line = id + sp + rp + e + n + bc;
        outfile << line;
    }
    outfile.close();
}

void binning(
    const vector<RcvSpsStruct>& rcv_sps,
    const vector<SrcSpsStruct>& src_sps,
    const vector<XSpsStruct>& x_sps,
    vector<BinStruct>& bins
) {
    int src_line, src_point, src_index;
    int rcv_line, rcv_point_start, rcv_point_end, rcv_index;
    int id, src_bin, rcv_bin, trace_count;
    double src_easting, src_northing, mid_point_x, mid_point_y, dx, dy;
    float azimuth, offset;
    trace_count = 0;
    for (const auto& x_row : x_sps) {
        src_line = x_row.src_line;
        src_point = x_row.src_point;
        src_index = x_row.src_index;
        auto src_item = find_if(
            src_sps.begin(), src_sps.end(), 
            [src_line, src_point, src_index](const SrcSpsStruct& s)
            {return (s.line == src_line) && (s.point == src_point) && (s.p_index == src_index);}
        );
        src_easting = src_item->easting;
        src_northing = src_item->northing;
        rcv_line = x_row.rcv_line;
        rcv_point_start = x_row.rcv_point_start;
        rcv_point_end = x_row.rcv_point_end;
        rcv_index = x_row.rcv_index;
        vector<RcvSpsStruct> rcv_items;
        copy_if(rcv_sps.begin(), rcv_sps.end(), back_inserter(rcv_items), 
            [rcv_line, rcv_point_start, rcv_point_end, rcv_index]
            (const RcvSpsStruct& r) {return 
                (r.line == rcv_line) && 
                (r.point >= rcv_point_start) && (r.point <= rcv_point_end) &&
                (r.p_index == rcv_index);
            }
        );
        for (const auto& rcv_item : rcv_items) {
            mid_point_x = (src_easting + rcv_item.easting) * 0.5;
            mid_point_y = (src_northing + rcv_item.northing) * 0.5;
            dx = rcv_item.easting - src_easting;
            dy = rcv_item.northing - src_northing;
            azimuth = atan2(dy, dx) * rad_to_deg;
            offset = sqrt(dx*dx + dy*dy);
            if (offset > max_offset) continue;
            auto val = calc_bin_index(
                mid_point_x, mid_point_y, origin, bin_sp_int, bin_rp_int
            );
            src_bin = get<0>(val);
            rcv_bin = get<1>(val);
            if (src_bin > nb_bin_sp || rcv_bin > nb_bin_rp) continue;
            id = calc_point_index(src_bin, rcv_bin);
            auto bin_item = lower_bound(bins.begin(), bins.end(), id, 
                [](BinStruct& b, int value){return b.id < value;}
            );
            if (bin_item != bins.end()) {
                bin_item->bin_count++;
                if (trace_count % 1'000'000 == 0) {
                    printf("trace count: %d\n", trace_count);
                };
                trace_count++;
            }
        }
    }
}

int main() {
    string filename = "d:/onedrive/development/binning_cpp/data/sps_phase1_final";
    vector<RcvSpsStruct> rcv_sps, matched_rcv;
    vector<SrcSpsStruct> src_sps;
    vector<XSpsStruct> x_sps;
    vector<BinStruct> bins, matched_bins;

    parse_rcv_sps(filename + ".R", rcv_sps);
    parse_src_sps(filename + ".S", src_sps);
    parse_x_sps(filename + ".X", x_sps);
    create_bins(bins);
    printf("number of bins: %d\n", bins.size());
    binning(rcv_sps, src_sps, x_sps, bins);
    int i = 400;
    int j = 700;
    auto bin = find_if(bins.begin(), bins.end(), [i, j](const BinStruct& b) {return (b.bin_sp == i && b.bin_rp == j);});
    printf("bin_sp: %d, bin_rp: %d, easting: %.0f, northing: %.0f, bin_count: %d",
        bin->bin_sp, bin->bin_rp, bin->easting, bin->northing, bin->bin_count
    );
    save_bins_csv(filename + ".csv", bins);
    return 0;
}
