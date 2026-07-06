#include <iostream>
#include <sqlite3.h>

int main() {
    sqlite3* db = nullptr;
    char* errMsg = nullptr;
    int rc;

    // 1. Open a standard SQLite database connection
    rc = sqlite3_open("spatial_db.sqlite", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    // 2. Explicitly enable extension loading (disabled by default for security)
    rc = sqlite3_enable_load_extension(db, 1);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to enable extension loading: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return rc;
    }

    // 3. Load the SpatiaLite shared library
    // Use "mod_spatialite" on Linux/macOS or "mod_spatialite.dll" on Windows
    rc = sqlite3_load_extension(db, "mod_spatialite", nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to load SpatiaLite extension: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return rc;
    }
    std::cout << "SpatiaLite loaded successfully!" << std::endl;

    // 4. Initialize Spatial Metadata (Required once for every new database file)
    // Passing '1' (TRUE) wraps the initialization in a single fast transaction
    const char* init_sql = "SELECT InitSpatialMetadata(1);";
    rc = sqlite3_exec(db, init_sql, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Spatial metadata init failed: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    } else {
        std::cout << "Spatial metadata tables created." << std::endl;
    }

    // 5. Test a spatial function (e.g., creating a geometry point and outputting as text)
    const char* test_sql = "SELECT AsText(MakePoint(4.8951, 52.3702, 4326));";
    auto callback = [](void*, int argc, char** argv, char**) -> int {
        if (argc > 0 && argv[0]) {
            std::cout << "Geometry output: " << argv[0] << std::endl;
        }
        return 0;
    };
    
    sqlite3_exec(db, test_sql, callback, nullptr, &errMsg);

    // Clean up
    sqlite3_close(db);
    return 0;