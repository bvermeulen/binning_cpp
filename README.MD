# Seismic binning - fold plot

* binning.exe - reads the sps files (R,S, X) and creates bins and traces 
* configuration is given in json config file, default location and file name is `./config.json`, otherwise config filename can be given as the first argument
* This is a CPP version of seisbinning (at least  10x faster)

Example of `config.json` file:
```
{
    "bin_files_stem": "d:/data/sps_phase1_final",
    "azimuth": 90,
    "origin_easting": 701692.00,
    "origin_northing": 3450142.50,
    "bin_sp_int": -25.0,
    "bin_rp_int": 12.5,
    "nb_bin_sp": 780,
    "nb_bin_rp": 1280,
    "max_offset": 3500.00,
    "epsg": 32638,
    "base_linepoint": 10000,
    "batch_size": 1000000
}
```
`base_linepoint` is to create an unique identifier for a bin bin point, for example: `1084513250` (src_bin 845, recv_bin 3250)
`batch_size` is the number of traces inserted in the database per transaction

Bins and traces are stored in a SQLite database. Binning can be updated using the following queries for source index 1 and 2 and a maximum offset of 3500 meter:
```
/* remove first all existing values from bins */
update bins set bin_count = null;
```
```
/* calculate bin_count from traces */ 
update bins set bin_count = bc from 
	(select bin_sp, bin_rp, count(*) as bc 
		from traces tr 
			where 
				tr.offset >= 0 and tr.offset < 3500 and
				tr.src_index < 3 and tr.rcv_index < 3
			group by tr.bin_sp, tr.bin_rp
	) as bins_grouped
	where bins.bin_sp = bins_grouped.bin_sp and bins.bin_rp = bins_grouped.bin_rp;
```
QGIS fold plot is shown below for offset 0 - 1500 meter.

![fold plot 1500](example_bin_1500.png)
