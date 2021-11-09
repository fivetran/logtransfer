#!/bin/sh
###########################################################################
#
# Copyright Notice and Disclaimer
# -------------------------------
#       (c) Copyright 2013.
#       SAP AG or an SAP affiliate company. All rights reserved.
#       Unpublished rights reserved under U.S. copyright laws.
#
#       SAP grants Licensee a non-exclusive license to use, reproduce,
#       modify, and distribute the sample source code below (the "Sample Code"),
#       subject to the following conditions:
#
#       (i) redistributions must retain the above copyright notice;
#
#       (ii) SAP shall have no obligation to correct errors or deliver
#       updates to the Sample Code or provide any other support for the
#       Sample Code;
#
#       (iii) Licensee may use the Sample Code to develop applications
#       (the "Licensee Applications") and may distribute the Sample Code in
#       whole or in part as part of such Licensee Applications, however in no
#       event shall Licensee distribute the Sample Code on a standalone basis;
#
#       (iv) and subject to the following disclaimer:
#       THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
#       INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
#       AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
#       SAP AG or an SAP affiliate company OR ITS LICENSORS BE LIABLE FOR ANY DIRECT, 
#	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
#       BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
#       OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#       ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
#       TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
#       USE OF THE SAMPLE CODE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
#       DAMAGE.
#
# Description
# -----------
#       sybopts.sh -- Helper script for UNIX makefiles that build Open Client
#       and Open Server applications. This script uses the value of
#       the SYBPLATFORM environment variable to determine what platform
#       specific libraries must be linked with the application.
#
# Usage
# -----
#       sybopts.sh <args>
#
#       where <args> can be one or more of the following:
#
#       compile -- Based on SYBPLATFORM, echoes the compiler command
#       and platform-specific compile flags. For example, if
#       SYBPLATFORM indicates a platform with support for
#       multithreading, the compile flag "-D_REENTRANT" might be
#       required.
#
#       comlibs -- Based on SYBPLATFORM, echoes the list of required
#       lower-layer SAP libraries that must be linked with the
#       application.
#
#       syslibs -- Based on SYBPLATFORM, echoes the list of required
#       system (i.e., non-SAP) libraries that must be linked with the
#       application.
#
#       ct, blk, srv (respectively) -- Based on SYBPLATFORM, echoes
#       the name of the archive for Client-Library, Bulk-Library,
#       or Server-Library routines.
#
#       verify -- Check SYBPLATFORM setting, if ok, echoes the library
#       names that will be used. If not ok, returns non-zero. This
#       argument must appear by itself with no other arguments.
#
#       verify_threaded -- Returns non-zero if SYBPLATFORM indicates
#       a version of Client-Library without multi-threaded support.
#
#       verify_krb -- Returns non-zero if SYBPLATFORM value is a platform
#       for which we currently do *not* support Kerberos.  Note that
#       Kerberos is currently only supported on the following UNIX
#       SYBPLATFORMs: nthread_sun_svr4(64), nthread_hpux(64),
#       nthread_rs6000(64), nthread_hpia(64), nthread_linux(64),
#       nthread_linux(amd)64 and nthread_sunx86(64).
#
# Example
# -------
#       COMPILE = `sh sybopts.sh compile` $(INCLUDE) $(DEFS)
#
###########################################################################

SET_SYBPLATFORM_MSG="Unknown machine type. Please set SYBPLATFORM variable"
BAD_ARGUMENT_MSG="Unknown argument"
THREAD_SUPPORT_MSG="This target requires multi-threaded support."
KRB_SUPPORT_MSG="This target requires that Kerberos is installed and running."
SYBPLATFORM_MSG="Check the value of your SYBPLATFORM environment variable."

#
# CC: Compiler/Linker name
# CFLAGS: Compiler/linker flags
# BLKLIB, SRVLIB, CTLIB and COMLIBS: SAP libraries. These come 
#	in multi-threaded and single-threaded versions.
# SYSLIBS: The list of system libraries that are required in order
#	to build a SAP Open Client/Server application, in the order
#	in which they must be linked.
#
case "${SYBPLATFORM}" in
	hpia)
			CC="cc";
			CFLAGS="-g +DD32 -mt";
			BLKLIB="-lsybblk"; SRVLIB="-lsybsrv"; CTLIB="-lsybct";
			COMLIBS="-lsybtcl -lsybcs -lsybcomn -lsybintl -lsybunic";
			SYSLIBS="-lcl -lm -ldld -Wl,-E,+s" ;;
	nthread_hpia)
			CC="cc";
			CFLAGS="-g +DD32 -mt -D_POSIX_C_SOURCE=199506L";
			BLKLIB="-lsybblk_r"; SRVLIB="-lsybsrv_r"; CTLIB="-lsybct_r";
			COMLIBS="-lsybtcl_r -lsybcs_r -lsybcomn_r -lsybintl_r -lsybunic";
			SYSLIBS="-lpthread -lrt -lcl -lm -ldld -Wl,-E,+s" ;;
	hpia64)
			CC="cc";
			CFLAGS="-g +DD64 -DSYB_LP64 -mt";
			BLKLIB="-lsybblk64"; SRVLIB="-lsybsrv64"; CTLIB="-lsybct64";
			COMLIBS="-lsybtcl64 -lsybcs64 -lsybcomn64 -lsybintl64 -lsybunic64";
			SYSLIBS="-lcl -lm -ldld -Wl,-E,+s" ;;
	nthread_hpia64) CC="cc";
			CFLAGS="-g +DD64 -DSYB_LP64 -mt";
			BLKLIB="-lsybblk_r64"; SRVLIB="-lsybsrv_r64"; CTLIB="-lsybct_r64";
			COMLIBS="-lsybtcl_r64 -lsybcs_r64 -lsybcomn_r64 -lsybintl_r64 -lsybunic64";
			SYSLIBS="-lpthread -lrt -lcl -lm -ldld -Wl,-E,+s" ;;
	hpux)
			CC="cc";
			CFLAGS="-g -Ae +Z +DA1.1 +DS2.0";
			BLKLIB="-lsybblk"; SRVLIB="-lsybsrv"; CTLIB="-lsybct";
			COMLIBS="-lsybtcl -lsybcs -lsybcomn -lsybintl -lsybunic";
			SYSLIBS="-Wl -lcl -lm -lBSD -ldld -Wl,-E,+s" ;;
	nthread_hpux)
			CC="cc";
			CFLAGS="-g -D_POSIX_C_SOURCE=199506L -Ae +Z +DA1.1 +DS2.0";
			BLKLIB="-lsybblk_r"; SRVLIB="-lsybsrv_r"; CTLIB="-lsybct_r";
			COMLIBS="-lsybtcl_r -lsybcs_r -lsybcomn_r -lsybintl_r -lsybunic";
			SYSLIBS="-lpthread -lrt -lcl -lm -lBSD -ldld -Wl,-E,+s " ;;
	hpux64)
			CC="cc";
			CFLAGS="-g -DSYB_LP64 -Ae -DANSI_VARARGS +Z +DA2.0W +DS2.0 -Wp,-H256000";
			BLKLIB="-lsybblk64"; SRVLIB="-lsybsrv64"; CTLIB="-lsybct64";
			COMLIBS="-lsybtcl64 -lsybcs64 -lsybcomn64 -lsybintl64 -lsybunic64";
			SYSLIBS="-Wl -lcl -lm -ldld -Wl,-E,+s" ;;
	nthread_hpux64) CC="cc";
			CFLAGS="-g -DSYB_LP64 -Ae -DANSI_VARARGS +Z +DA2.0W +DS2.0 -Wp,-H256000";
			BLKLIB="-lsybblk_r64"; SRVLIB="-lsybsrv_r64"; CTLIB="-lsybct_r64";
			COMLIBS="-lsybtcl_r64 -lsybcs_r64 -lsybcomn_r64 -lsybintl_r64 -lsybunic64";
			SYSLIBS="-lpthread -lrt -lcl -lm -ldld -Wl,-E,+s " ;;
	linux)
			CC="cc";
			CFLAGS="-m32 -g";
			BLKLIB="-lsybblk"; SRVLIB="-lsybsrv"; CTLIB="-lsybct";
			COMLIBS="-lsybtcl -lsybcs -lsybcomn -lsybintl -lsybunic";
			SYSLIBS="-Wl,-Bdynamic -ldl -lnsl -lm" ;;
	nthread_linux)
			CC="cc";
			CFLAGS="-m32 -g -D_REENTRANT";
			BLKLIB="-lsybblk_r"; SRVLIB="-lsybsrv_r"; CTLIB="-lsybct_r";
			COMLIBS="-lsybtcl_r -lsybcs_r -lsybcomn_r -lsybintl_r -lsybunic";
			SYSLIBS="-Wl,-Bdynamic -ldl -lpthread -lnsl -lm" ;;
	linuxamd64 | linux64)
			CC="cc";
			CFLAGS="-m64 -g -DSYB_LP64 -Werror -Wall -Wformat=2";
			BLKLIB="-lsybblk64"; SRVLIB="-lsybsrv64"; CTLIB="-lsybct64";
			COMLIBS="-lsybtcl64 -lsybcs64 -lsybcomn64 -lsybintl64 -lsybunic64";
			SYSLIBS="-Wl,-Bdynamic -ldl -lnsl -lm" ;;
	nthread_linuxamd64 | nthread_linux64)
			CC="cc";
			CFLAGS="-m64 -g -DSYB_LP64 -D_REENTRANT -Werror -Wall -Wformat=2";
			BLKLIB="-lsybblk_r64"; SRVLIB="-lsybsrv_r64"; CTLIB=-lsybct_r64;
			COMLIBS="-lsybtcl_r64 -lsybcs_r64 -lsybcomn_r64 -lsybintl_r64 -lsybunic64";
			SYSLIBS="-Wl,-Bdynamic -ldl -lpthread -lnsl -lm" ;;
	ibmplinux)
			CC="xlc -q32";
			CFLAGS="-g -Wl,-Bstatic -Wl,--export-dynamic";
			BLKLIB="-lsybblk"; SRVLIB="-lsybsrv"; CTLIB="-lsybct";
			COMLIBS="-lsybtcl -lsybcs -lsybcomn -lsybintl -lsybunic";
			SYSLIBS="-Wl,-Bdynamic -ldl -lnsl -lm" ;;
	nthread_ibmplinux)
			CC="xlc_r -q32";
			CFLAGS="-g -D_REENTRANT -Wl,-Bstatic -Wl,--export-dynamic";
			BLKLIB="-lsybblk_r"; SRVLIB="-lsybsrv_r"; CTLIB="-lsybct_r";
			COMLIBS="-lsybtcl_r -lsybcs_r -lsybcomn_r -lsybintl_r -lsybunic";
			SYSLIBS="-Wl,-Bdynamic -ldl -lpthread -lnsl -lm" ;;
	ibmplinux64)
			CC="xlc -q64";
			CFLAGS="-g -DSYB_LP64 -Wl,-Bstatic -Wl,--export-dynamic";
			BLKLIB="-lsybblk64"; SRVLIB="-lsybsrv64"; CTLIB="-lsybct64";
			COMLIBS="-lsybtcl64 -lsybcs64 -lsybcomn64 -lsybintl64 -lsybunic64";
			SYSLIBS="-Wl,-Bdynamic -ldl -lnsl -lm" ;;
	nthread_ibmplinux64)
			CC="xlc_r -q64";
			CFLAGS="-g -DSYB_LP64 -D_REENTRANT -Wl,-Bstatic -Wl,--export-dynamic";
			BLKLIB="-lsybblk_r64"; SRVLIB="-lsybsrv_r64"; CTLIB=-lsybct_r64;
			COMLIBS="-lsybtcl_r64 -lsybcs_r64 -lsybcomn_r64 -lsybintl_r64 -lsybunic64";
			SYSLIBS="-Wl,-Bdynamic -ldl -lpthread -lnsl -lm" ;;
	macosx)
			CC="cc";
			CFLAGS="-g -fno-common";
			BLKLIB="-lsybblk"; SRVLIB="-lsybsrv"; CTLIB="-lsybct";
			COMLIBS="-lsybtcl -lsybcs -lsybcomn -lsybintl -lsybunic";
			SYSLIBS="-lSystem" ;;
	nthread_macosx)
			CC="cc" ;
			CFLAGS="-g -fno-common";
			BLKLIB="-lsybblk_r"; SRVLIB="-lsybsrv_r"; CTLIB="-lsybct_r";
			COMLIBS="-lsybtcl_r -lsybcs_r -lsybcomn_r -lsybintl_r -lsybunic";
			SYSLIBS="-lSystem";;
	rs6000)
			CC="xlc ";
			CFLAGS="-g";
			BLKLIB="-lsybblk"; SRVLIB="-lsybsrv"; CTLIB="-lsybct";
			COMLIBS="-lsybtcl -lsybcs -lsybcomn -lsybintl -lsybunic";
			SYSLIBS="-lm" ;;
	nthread_rs6000) CC="xlc_r";
			CFLAGS="-g -D_THREAD_SAFE";
			BLKLIB="-lsybblk_r"; SRVLIB="-lsybsrv_r"; CTLIB="-lsybct_r";
			COMLIBS="-lsybtcl_r -lsybcs_r -lsybcomn_r -lsybintl_r -lsybunic";
			SYSLIBS="-lm" ;;
	rs600064)
			CC="xlc";
			CFLAGS="-g -q64 -DSYB_LP64";
			BLKLIB="-lsybblk64"; SRVLIB="-lsybsrv64"; CTLIB="-lsybct64";
			COMLIBS="-lsybtcl64 -lsybcs64 -lsybcomn64 -lsybintl64 -lsybunic64";
			SYSLIBS="-lm" ;;
	nthread_rs600064)
			CC="xlc_r";
			CFLAGS="-g -q64 -bexpall -DSYB_LP64 -D_THREAD_SAFE";
			BLKLIB="-lsybblk_r64"; SRVLIB="-lsybsrv_r64"; CTLIB="-lsybct_r64";
			COMLIBS="-lsybtcl_r64 -lsybcs_r64 -lsybcomn_r64 -lsybintl_r64 -lsybunic64";
			SYSLIBS="-lm" ;;

	# Dynamic versus static linking example for different modes on Solaris (sun_svr4 and sunx86)
	# We link against the static OCS libraries for non-threaded 32bit and threaded 64bit modes.
	# We link against the dynamic OCS libraries for threaded 32bit and non-threaded 64bit modes.
	sun_svr4)
			CC="cc";
			CFLAGS="-g -Xa -Bstatic";
			BLKLIB="-lsybblk"; SRVLIB="-lsybsrv"; CTLIB="-lsybct";
			COMLIBS="-lsybtcl -lsybcs -lsybcomn -lsybintl -lsybunic";
			SYSLIBS="-Bdynamic -lrt -lsocket -lnsl -ldl -lm" ;;
	nthread_sun_svr4)
			CC="cc";
			CFLAGS="-g -Xa -mt -D_REENTRANT";
			BLKLIB="-lsybblk_r"; SRVLIB="-lsybsrv_r"; CTLIB="-lsybct_r";
			COMLIBS="-lsybtcl_r -lsybcs_r -lsybcomn_r -lsybintl_r -lsybunic";
			SYSLIBS="-R/usr/lib/lwp -Bdynamic -lrt -lsocket -lnsl -ldl -lpthread -lthread -lm" ;;
	sun_svr464)
			CC="cc";
			CFLAGS="-g -Xa -DSYB_LP64 -m64";
			BLKLIB="-lsybblk64"; SRVLIB="-lsybsrv64"; CTLIB="-lsybct64";
			COMLIBS="-lsybtcl64 -lsybcs64 -lsybcomn64 -lsybintl64 -lsybunic64";
			SYSLIBS="-Bdynamic -lrt -lsocket -lnsl -ldl -lm" ;;
	nthread_sun_svr464)
			CC="cc";
			CFLAGS="-g -Xa -DSYB_LP64 -mt -D_REENTRANT -m64 -Bstatic";
			BLKLIB="-lsybblk_r64"; SRVLIB="-lsybsrv_r64"; CTLIB="-lsybct_r64";
			COMLIBS="-lsybtcl_r64 -lsybcs_r64 -lsybcomn_r64 -lsybintl_r64 -lsybunic64";
			SYSLIBS="-R/usr/lib/lwp/sparcv9 -Bdynamic -lrt -lsocket -lnsl -ldl -lpthread -lthread -lm" ;;
	sunx86)
			CC="cc";
			CFLAGS="-g -Xa -Bstatic";
			BLKLIB="-lsybblk"; SRVLIB="-lsybsrv"; CTLIB="-lsybct";
			COMLIBS="-lsybtcl -lsybcs -lsybcomn -lsybintl -lsybunic";
			SYSLIBS="-Bdynamic -lrt -lsocket -lnsl -ldl -lm" ;;

	nthread_sunx86)
			CC="cc";
			CFLAGS="-g -Xa -mt -D_REENTRANT";
			BLKLIB="-lsybblk_r"; SRVLIB="-lsybsrv_r"; CTLIB="-lsybct_r";
			COMLIBS="-lsybtcl_r -lsybcs_r -lsybcomn_r -lsybintl_r -lsybunic";
			SYSLIBS="-Bdynamic -lpthread -lthread -lrt -lsocket -lnsl -ldl -lm" ;;
	sunx8664)
			CC="cc";
			CFLAGS="-g -Xa -DSYB_LP64 -m64";
			BLKLIB="-lsybblk64"; SRVLIB="-lsybsrv64"; CTLIB="-lsybct64";
			COMLIBS="-lsybtcl64 -lsybcs64 -lsybcomn64 -lsybintl64 -lsybunic64";
			SYSLIBS="-Bdynamic -lrt -lsocket -lnsl -ldl -lm" ;;
	nthread_sunx8664)
			CC="cc";
			CFLAGS="-g -Xa -mt -D_REENTRANT -DSYB_LP64 -m64 -Bstatic";
			BLKLIB="-lsybblk_r64"; SRVLIB="-lsybsrv_r64"; CTLIB="-lsybct_r64";
			COMLIBS="-lsybtcl_r64 -lsybcs_r64 -lsybcomn_r64 -lsybintl_r64 -lsybunic64";
			SYSLIBS="-Bdynamic -lpthread -lthread -lrt -lsocket -lnsl -ldl -lm" ;;
	*)
			echo " $SET_SYBPLATFORM_MSG"; exit 1 ;;
esac

#
# A single argument of "verify" means that we should print
# the value of SYBPLATFORM, print the choices that will be made
# for comlibs, syslibs, etc., then exit with a status of 0. If
# SYBPLATFORM is incorrect, we will not get here.
#
if [ $1 = "verify" ]
  then
	echo "SYBPLATFORM is $SYBPLATFORM"
	echo "Compiling with $CC $CFLAGS"
	echo "SAP Common libraries: $COMLIBS"
	echo "SAP Client library: $CTLIB"
	echo "SAP Server library: $SRVLIB"
	echo "System libraries: $SYSLIBS"
	exit 0
fi

if [ $1 = verify_threaded ]
  then
	case $SYBPLATFORM in
		nthread_*)
			exit 0;;
		*)
			echo " $THREAD_SUPPORT_MSG";
			exit 1 ;;
	esac
	exit 0
fi

#
# Ensure that Kerberos is installed and running. Kerberos is currently
# supported on nthread_sun_svr4(64), nthread_hpux(64), nthread_rs6000(64),
# nthread_hpia(64), nthread_linux(64), nthread_linuxamd64 and nthread_sunx86(64).
#
if [ $1 = verify_krb ]
  then
	case $SYBPLATFORM in
		nthread_sun_svr4*|nthread_hpux*|nthread_rs6000*|nthread_linux*|nthread_sunx86*|nthread_hpia*)
			exit 0;;
		*)
			echo " $KRB_SUPPORT_MSG";
			exit 1 ;;
	esac
	exit 0
fi

#
# Build a string to echo what was asked for:
#
ECHOSTRING=""
for a
do
	case "$a" in
		compile)	ECHOSTRING="$ECHOSTRING $CC $CFLAGS" ;;
		comlibs)	ECHOSTRING="$ECHOSTRING $COMLIBS" ;;
		syslibs)	ECHOSTRING="$ECHOSTRING $SYSLIBS " ;;
		ct)		ECHOSTRING="$ECHOSTRING $CTLIB" ;;
		srv)		ECHOSTRING="$ECHOSTRING $SRVLIB" ;;
		blk)		ECHOSTRING="$ECHOSTRING $BLKLIB" ;;
		*)		echo "$0: $a: $BAD_ARGUMENT_MSG "; exit 1;;
	esac
done

echo "$ECHOSTRING"
exit 0

