"""
load bins to sqlite
"""
from pathlib import Path
import csv
import sqlite3
from functools import wraps


def db_connect(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        result = None
        database = args[0].database_file
        try:
            connection = sqlite3.connect(database)
            connection.enable_load_extension(True)
            connection.execute('SELECT load_extension("mod_spatialite")')
            cursor = connection.cursor()
            result = func(*args, cursor, **kwargs)
            connection.commit()

        except sqlite3.Error as error:
            print(f"Error while connect to sqlite {database}: {error}")

        finally:
            if connection:
                cursor.close()
                connection.close()

        return result

    return wrapper


def create_database(database_file):
    connection = None
    try:
        connection = sqlite3.connect(database_file)
        connection.enable_load_extension(True)
        connection.execute('SELECT load_extension("mod_spatialite")')
        connection.execute("SELECT InitSpatialMetaData(1);")
        connection.commit()

    except sqlite3.Error as error:
        print(f"error while connect to sqlite {database_file}: " f"{error}")

    finally:
        if connection:
            connection.close()

    print(f"Database {database_file} created ...")


class DbBins:

    def __init__(
        self,
        database_file: Path,
        csv_file: Path,
        table: str,
        epsg: int,
    ) -> None:
        self.database_file = database_file
        self.csv_file = csv_file
        self.table = table
        self.epsg = epsg

    @db_connect
    def create_bins_table(self, cursor):
        sql_string = (
            f"CREATE TABLE {self.table} ("
            f"id INTEGER PRIMARY KEY, "
            f"bin_id, "
            f"bin_sp INTEGER, "
            f"bin_rp INTEGER, "
            f"easting DOUBLE PRECISION, "
            f"northing DOUBLE PRECISION, "
            f"bin_count INT "
            f");"
        )
        cursor.executescript(sql_string)

        # once table is created you can add the geomety column
        sql_string = (
            f'SELECT AddGeometryColumn("{self.table}", '
            f'"geom", {self.epsg}, "POINT", "XY");'
        )
        cursor.execute(sql_string)

        print(f"Table {self.table} created ...")

    @db_connect
    def csv_to_sqlite(self, cursor):
        sql_string = (
            f"INSERT into {self.table} ("
            f"bin_id, bin_sp, bin_rp, easting, northing, bin_count, geom) "
            f"VALUES ({", ".join(["?"]*6)}, MakePoint(?, ?, ?) "
            f");"
        )            
        with open(self.csv_file, "rt") as csv_file:
            rows = csv.reader(csv_file)

            next(rows)
            for row in rows:
                bin_count = int(row[5]) if int(row[5]) != 0 else None
                easting = float(row[3])
                northing = float(row[4])
                cursor.execute(sql_string, 
                    (
                        int(row[0]),
                        int(row[1]),
                        int(row[2]),
                        easting,
                        northing,
                        bin_count,
                        easting,
                        northing,
                        self.epsg
                    )
                )


def main():
    file_stem = "./data/sps_phase1_final"
    epsg = 32638
    database_file = Path(file_stem + ".sqlite")
    csv_file = Path(file_stem + ".csv")
                         
    create_database(database_file)
    db_bins = DbBins(
        database_file,
        csv_file,
        "bins",
        epsg
    )
    db_bins.create_bins_table()
    db_bins.csv_to_sqlite()


if __name__ == "__main__":
    main()
