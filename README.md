# logtransfer
In support of HVR-15723, this is a POC with ASE's `dbcc logtransfer` API.

Customizing:
In `example.h`, set `EX_USERNAME` and `EX_PASSWORD` appropriately. In
`logtransfer.c`, set `Ex_dbname` to the name of the database to be scanned.
(Setting `DSQUERY` below gets you to the right data server.)

In the database to be scanned, create tables as follows:
```sql
create table test_lob (
    id int primary key not null ,
    story text
)

create table test_lob_dol (
    id int primary key not null ,
    story text
)
lock datarows

create table test_all_lobs_dol (
                                   id integer primary key ,
                                   t text null off row,
                                   ui unitext in row ,
                                   i image off row,
                                   ti text in row ,
                                   u unitext null off row,
                                   ii image null in row
) lock datarows

create table test_all_lobs_apl
(
    id integer primary key,
    t  text    null off row,
    ui unitext in row,
    i  image off row,
    ti text in row,
    u  unitext null off row,
    ii image   null in row
)

create table test_some_lobs_dol (
                                    id integer primary key ,
                                    t text null off row,
                                    dt datetime null,
                                    c10 char(10),
                                    ui unitext in row ,
                                    vc10 varchar(10)
) lock datarows

create table test_some_lobs_apl (
                                    id integer primary key,
                                    t text null off row,
                                    dt datetime null,
                                    c10 char(10),
                                    ui unitext in row ,
                                    vc10 varchar(10)
)

create table test_no_lobs_dol (
                                  id integer primary key ,
                                  i integer null,
                                  d1 varchar(200) null,
                                  n18    numeric(18),
                                  d10_2  decimal(10, 2),
                                  dt     datetime,
                                  d      date,
                                  bdt    bigdatetime,
                                  mny    money
) lock datarows

create table test_no_lobs_apl (
                                  id integer primary key ,
                                  i integer null,
                                  d1 varchar(200) null,
                                  n18    numeric(18),
                                  d10_2  decimal(10, 2),
                                  dt     datetime,
                                  d      date,
                                  bdt    bigdatetime,
                                  mny    money
)
```

Building:
```commandline
export SYBASE=/home/sybase
export SYBASE_OCS=OCS-16_0
export SYBPLATFORM=nthread_linux64
export DSQUERY=MYSYBASE
export LD_LIBRARY_PATH=${SYBASE}/${SYBASE_OCS}/lib

make logtransfer
make rpc
```
Note:
- On my machine I run `Linux lt-gcarter 5.4.0-89-generic #100-Ubuntu SMP Fri Sep 24 14:50:10 UTC 2021 x86_64 x86_64 x86_64 GNU/Linux`
- My Sybase software installation is at `/home/sybase`.
- `rpc` is the Open Client sample application I used as the basis for building out the `dbcc logtransfer()` proof-of-concept.


Running:
```commandline
./logtransfer
./rpc
```
Note:

