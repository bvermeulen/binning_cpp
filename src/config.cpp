#include <iostream>
#include <fstream>
#include <cmath>
#include <boost/json/src.hpp>
#include "config.h"

using namespace std;

ConfigStruct read_config(string file_config) {
    string bin_files_stem;
    float azimuth, max_offset, bin_sp_int, bin_rp_int;
    double origin_easting, origin_northing;
    int nb_bin_sp, nb_bin_rp, epsg, base_linepoint, batch_size;
    ifstream file(file_config);
    if (!file.is_open()) {
        printf("Error, could not open the file: %s!\n", file_config.c_str());
        exit(0);
    }
    ostringstream ss;
    ss << file.rdbuf();
    string json_str = ss.str();

    try {
        boost::json::value jv = boost::json::parse(json_str);
        boost::json::object const& obj = jv.as_object();
        
        bin_files_stem = boost::json::value_to<string>(obj.at("bin_files_stem"));
        azimuth = boost::json::value_to<float>(obj.at("azimuth"));
        origin_easting = boost::json::value_to<double>(obj.at("origin_easting"));
        origin_northing = boost::json::value_to<double>(obj.at("origin_northing"));
        bin_sp_int = boost::json::value_to<float>(obj.at("bin_sp_int"));
        bin_rp_int = boost::json::value_to<float>(obj.at("bin_rp_int"));
        nb_bin_sp = boost::json::value_to<int>(obj.at("nb_bin_sp"));
        nb_bin_rp = boost::json::value_to<int>(obj.at("nb_bin_rp"));
        max_offset = boost::json::value_to<float>(obj.at("max_offset"));
        epsg = boost::json::value_to<int>(obj.at("epsg"));
        base_linepoint = boost::json::value_to<int>(obj.at("base_linepoint"));
        batch_size = boost::json::value_to<int>(obj.at("batch_size"));
    } 
    catch (exception const& e) {
        printf("Parsing failed: %s\n", e.what());
        exit(0);
    }
    ConfigStruct cfg;
    cfg.bin_files_stem = bin_files_stem;
    cfg.origin = {origin_easting, origin_northing, azimuth * DEG_TO_RAD};
    cfg.nb_bin_sp = nb_bin_sp;
    cfg.nb_bin_rp = nb_bin_rp;
    cfg.bin_sp_int = bin_sp_int;
    cfg.bin_rp_int = bin_rp_int;
    cfg.max_offset = max_offset;
    cfg.epsg = epsg;
    cfg.base_linepoint = base_linepoint;
    cfg.batch_size = batch_size;
    return cfg;
}
