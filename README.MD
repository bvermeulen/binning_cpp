# Seismic binning - fold plot

* binning.exe - read sps files (R,S, X), create traces and store traces in the database 
* CPP version of seisbinning (at least  10x faster)

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
