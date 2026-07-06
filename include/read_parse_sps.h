#ifndef READ_PARSE_SPS_H
#define READ_PARSE_SPS_H

using namespace std;

struct RcvStruct {
    string type;
    int line;
    int point;
    int p_index;
    string p_code;
    double easting;
    double northing;
    float elevation;
};
    
struct SrcStruct {
    string type;
    int line;
    int point;
    int p_index;
    string p_code;
    double easting;
    double northing;
    float elevation;
};
    
struct XStruct {
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
    
namespace sps {
    void parse_rcv_sps(string filename, vector<RcvStruct>& rcv_sps_vector);
    void parse_src_sps(string filename, vector<SrcStruct>& src_sps_vector);
    void parse_x_sps(string filename, vector<XStruct>& x_sps_vector);
}

#endif // READ_PARSE_SPS
