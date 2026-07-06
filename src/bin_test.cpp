#include <iostream>
#include <vector>
#include "save_data.h"

using namespace std;

int main() {
    SaveData sd;
    string filename = "./data/test_db.sqlite";
    sd.create_database(filename);
    sd.create_bin_table();
    sd.close_database();
}

// int main() {
//     sqlite3 *db;
//     sqlite3_open("test.db", &db);

//     // 1. Prepare the statement with ? placeholders
//     const char *sql = "INSERT INTO users (name, age) VALUES (?, ?);";
//     sqlite3_stmt *stmt;
//     sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);

//     // 2. Bind values to the placeholders (Index starts at 1)
//     const char *myName = "Alice";
//     int myAge = 25;

//     // Index 1 for the first ?, Index 2 for the second ?
//     sqlite3_bind_text(stmt, 1, myName, -1, SQLITE_STATIC);
//     sqlite3_bind_int(stmt, 2, myAge);

//     // 3. Step/Execute the query
//     int rc = sqlite3_step(stmt);
//     if (rc != SQLITE_DONE) {
//         std::cerr << "Execution failed: " << sqlite3_errmsg(db) << "\n";
//     } else {
//         std::cout << "Insert successful!\n";
//     }

//     // 4. Clean up
//     sqlite3_finalize(stmt);
//     sqlite3_close(db);
//     return 0;
// }