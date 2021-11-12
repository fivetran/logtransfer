# logtransfer
In support of HVR-15723, this is a POC with ASE's `dbcc logtransfer` API.


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

