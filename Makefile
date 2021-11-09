###############################################################################
#
# UNIX MAKEFILE FOR THE CLIENT-LIBRARY EXAMPLE PROGRAMS
# -----------------------------------------------------
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
# 	To use this makefile, you need to set the following 
#	environment variables:
#
#	SYBASE		Set to the SAP ASE SDK installation directory.
#	SYBASE_OCS      Set to the Open Client products directory.
#	SYBPLATFORM 	Set to the SAP ASE SDK abbreviation for your platform.
#
# 	The shell script sybopts.sh is used to create platform-specific command
# 	line options for compilation and linking with SAP and system 
#	libraries. The SYBPLATFORM environment variable must be set in order 
#	for this script to work correctly. Please see README file for valid 
#	settings.
# 
#############################################################################

#############################################################################
#
# Directories and search paths.
#
#############################################################################

SHELL     = /bin/sh
RELDIR    = $(SYBASE)/$(SYBASE_OCS)

# SAP ASE SDK include directory
INCDIR    = $(RELDIR)/include

#
# Modify the following line to use debug/nondebug libraries
#
#LIBDIR   = $(RELDIR)/devlib
LIBDIR    = $(RELDIR)/lib

# Specify the include and library search paths for the compiler
INCLUDE   = -I. -I$(INCDIR)
LIBPATH   = -L$(LIBDIR)

##############################################################################
#
# Macros for compile/link lines
#
##############################################################################

#
# Specify the compile-time defines.
# Use -DDEBUG only if linking with devlib/*
#
#DEFS 	= -DDEBUG -D$(SYBPLATFORM)=1 
DEFS 	= -D$(SYBPLATFORM)=1 

#
# Define the compiler command and compile flags.
#
# `sh sybopts.sh compile` echoes a platform-specific command to invoke the C 
# compiler. You can replace this invocation with the comand to invoke your 
# preferred compiler if you like.
#
COMPILE = `sh sybopts.sh compile` $(INCLUDE) $(DEFS)

#
# Define the list of libraries that must be linked with in order to call 
# Client-Library and Bulk-Library routines.
#
# `sh sybopts.sh comlibs` echoes a platform-specific list of lower-layer 
# SAP libraries to link with. These are required in order to link with 
# CS-Libary, Client-Library, Bulk-Library, or Server-Library.
# 'sh sybopts.sh' with argument 'ct', 'srv' or 'blk' respectively echo the 
# Client-Library, Server-Library and Bulk-Library.
#
# `sh sybopts.sh syslibs` echoes a platform-specific list of system libraries 
# that are required.
#
COMLIBS = `sh sybopts.sh comlibs`
CTLIBS	= `sh sybopts.sh ct` 
BLKLIBS	= `sh sybopts.sh blk`
SYSLIBS	= `sh sybopts.sh syslibs`

##############################################################################
#
# The targets and how to build them.
#
##############################################################################

# 
# 'make all' does not build the multthrd or secct samples, since they
# are not supported on all platforms. 'make all' also does not build the 
# unicode examples. These are built by 'make uni'.
#
all: arraybind compute csr_disp csr_disp_implicit csr_disp_scrollcurs csr_disp_scrollcurs2 csr_disp_scrollcurs3 rpc getsend i18n usedir id_update exasync exconfig firstapp dynamic lobdynamic lobrpc locator wide_curupd wide_compute wide_dynamic wide_rpc blktxt twophase uctext batch_lang batch_dynamic

uni: uni_firstapp uni_csr_disp uni_compute uni_rpc uni_blktxt

exutils.o: exutils.c example.h exutils.h
	@ printf "$(COMPILE) -c exutils.c -o exutils.o\n\n";
	@ $(COMPILE) -c exutils.c -o exutils.o

exutils_uctext.o: exutils.c example.h exutils.h
	@ printf "$(COMPILE) -c exutils.c -DPARTIAL_TEXT -o exutils_uctext.o\n\n";
	@ $(COMPILE) -c exutils.c -DPARTIAL_TEXT -o exutils_uctext.o

exutils2.o: exutils2.c example.h exutils.h
	@ printf "$(COMPILE) -c exutils2.c -o exutils2.o\n\n";
	@ $(COMPILE) -c exutils2.c -o exutils2.o

ex_alib.o: ex_alib.c example.h exasync.h
	@ printf "$(COMPILE) -c ex_alib.c -o ex_alib.o\n\n";
	@ $(COMPILE) -c ex_alib.c -o ex_alib.o

wide_util.o: wide_util.c 
	@ printf "$(COMPILE) -c wide_util.c -o wide_util.o\n\n";
	@ $(COMPILE) -c wide_util.c -o wide_util.o

thrdfunc.o: thrdfunc.c thrdfunc.h
	@ sh sybopts.sh verify_threaded;
	@ printf "$(COMPILE) -c thrdfunc.c -o thrdfunc.o\n\n";
	@ $(COMPILE) -c thrdfunc.c -o thrdfunc.o

thrdutil.o: thrdutil.c thrdutil.h
	@ sh sybopts.sh verify_threaded;
	@ printf "$(COMPILE) -c thrdutil.c -o thrdutil.o\n\n";
	@ $(COMPILE) -c thrdutil.c -o thrdutil.o

arraybind: arraybind.c exutils.o
	@ printf "$(COMPILE) arraybind.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) arraybind.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

compute: compute.c exutils.o
	@ printf "$(COMPILE) compute.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) compute.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

dynamic: dynamic.c wide_util.o exutils.o
	@ printf "$(COMPILE) dynamic.c exutils.o wide_util.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) dynamic.c exutils.o wide_util.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

rpc: rpc.c exutils.o
	@ printf "$(COMPILE) rpc.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@\n\n";
	@ $(COMPILE) rpc.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@

lobrpc: lobrpc.c exutils.o
	@ printf "$(COMPILE) lobrpc.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@\n\n";
	@ $(COMPILE) lobrpc.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@

lobdynamic: lobdynamic.c exutils.o
	@ printf "$(COMPILE) lobdynamic.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) lobdynamic.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

locator: locator.c
	@ printf "$(COMPILE) locator.c $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) locator.c $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

csr_disp: csr_disp.c exutils.o
	@ printf "$(COMPILE) csr_disp.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) csr_disp.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

csr_disp_implicit: csr_disp_implicit.c exutils.o
	@ printf "$(COMPILE) csr_disp_implicit.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) csr_disp_implicit.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

csr_disp_scrollcurs: csr_disp_scrollcurs.c exutils.o exutils2.o
	@ printf "$(COMPILE) csr_disp_scrollcurs.c exutils.o exutils2.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) csr_disp_scrollcurs.c exutils.o exutils2.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

csr_disp_scrollcurs2: csr_disp_scrollcurs2.c exutils.o exutils2.o
	@ printf "$(COMPILE) csr_disp_scrollcurs2.c exutils.o exutils2.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) csr_disp_scrollcurs2.c exutils.o exutils2.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

csr_disp_scrollcurs3: csr_disp_scrollcurs3.c exutils.o exutils2.o
	@ printf "$(COMPILE) csr_disp_scrollcurs3.c exutils.o exutils2.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) csr_disp_scrollcurs3.c exutils.o exutils2.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

getsend: getsend.c exutils.o
	@ printf "$(COMPILE) getsend.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) getsend.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

i18n: i18n.c exutils.o
	@ printf "$(COMPILE) i18n.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) i18n.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

exasync: ex_amain.c exutils.o ex_alib.o
	@ printf "$(COMPILE) ex_amain.c exutils.o ex_alib.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) ex_amain.c exutils.o ex_alib.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

blktxt: blktxt.c exutils.o
	@ printf "$(COMPILE) blktxt.c exutils.o $(LIBPATH) $(BLKLIBS) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) blktxt.c exutils.o $(LIBPATH) $(BLKLIBS) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

usedir: usedir.c exutils.o
	@ printf "$(COMPILE) usedir.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) usedir.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

id_update: id_update.c exutils.o
	@ printf "$(COMPILE) id_update.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) id_update.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

#
# Wide table examples
#
wide_curupd: wide_curupd.c exutils.o wide_util.o
	@ printf "$(COMPILE) wide_curupd.c exutils.o wide_util.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) wide_curupd.c exutils.o wide_util.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

wide_compute: wide_compute.c exutils.o
	@ printf "$(COMPILE) wide_compute.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) wide_compute.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

wide_dynamic: wide_dynamic.c wide_util.o exutils.o
	@ printf "$(COMPILE) wide_dynamic.c exutils.o wide_util.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) wide_dynamic.c exutils.o wide_util.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

wide_rpc: wide_rpc.c exutils.o
	@ printf "$(COMPILE) wide_rpc.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@\n\n";
	@ $(COMPILE) wide_rpc.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@

#
# Unichar16 examples
#
uni_firstapp: uni_firstapp.c
	@ printf "$(COMPILE) uni_firstapp.c $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) uni_firstapp.c $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

uni_csr_disp: uni_csr_disp.c exutils.o
	@ printf "$(COMPILE) uni_csr_disp.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) uni_csr_disp.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

uni_compute: uni_compute.c exutils.o
	@ printf "$(COMPILE) uni_compute.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) uni_compute.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

uni_blktxt: uni_blktxt.c exutils.o
	@ printf "$(COMPILE) uni_blktxt.c exutils.o $(LIBPATH) $(BLKLIBS) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) uni_blktxt.c exutils.o $(LIBPATH) $(BLKLIBS) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

uni_rpc: uni_rpc.c exutils.o
	@ printf "$(COMPILE) uni_rpc.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@\n\n";
	@ $(COMPILE) uni_rpc.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@

#
# multthrd requires the pthread or solaris thread interface. 
# sybopts.sh verifies that SYBPLATFORM is set to a "nthread_" variant.
#
multthrd: multthrd.c thrdutil.o thrdfunc.o example.h
	@ sh sybopts.sh verify_threaded;
	@ printf "$(COMPILE) multthrd.c thrdutil.o thrdfunc.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) multthrd.c thrdutil.o thrdfunc.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

exconfig: exconfig.c exutils.o
	@ printf "$(COMPILE) exconfig.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) exconfig.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

uctext: uctext.c exutils_uctext.o
	@ printf "$(COMPILE) uctext.c exutils_uctext.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) uctext.c exutils_uctext.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

firstapp: firstapp.c
	@ printf "$(COMPILE) firstapp.c $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) firstapp.c $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

batch_dynamic: batch_dynamic.c exutils.o
	@ printf "$(COMPILE) batch_dynamic.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) batch_dynamic.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

batch_lang: batch_lang.c exutils.o
	@ printf "$(COMPILE) batch_lang.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) batch_lang.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

#
# Target for the Kerberos security example:
#
secct_krb: secct.c exutils.o
	@ sh sybopts.sh verify_krb;
	@ printf "$(COMPILE) secct.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) secct.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

# 
# Two Phase Commit example:
#
ctpr.o: ctpr.c ctxact.h
	@ printf "$(COMPILE) -c ctpr.c -o ctpr.o\n\n";
	@ $(COMPILE) -c ctpr.c -o ctpr.o

ctxact.o: ctxact.c ctxact.h
	@ printf "$(COMPILE) -c ctxact.c -o ctxact.o\n\n";
	@ $(COMPILE) -c ctxact.c -o ctxact.o

twophase: twophase.c ctpr.o ctxact.o example.h
	@ printf "$(COMPILE) twophase.c ctpr.o ctxact.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@\n\n";
	@ $(COMPILE) twophase.c ctpr.o ctxact.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS) -o $@

#
# Clean all binaries
#
clean: 
	rm -f arraybind compute csr_disp csr_disp_implicit csr_disp_scrollcurs \
	csr_disp_scrollcurs2 csr_disp_scrollcurs3 rpc getsend i18n usedir \
	id_update exasync blktxt exconfig firstapp wide_compute wide_dynamic \
	wide_curupd wide_rpc multthrd uni_firstapp uni_csr_disp uni_compute \
	uni_blktxt uni_rpc secct_krb uctext twophase lobrpc lobdynamic locator \
	dynamic batch_dynamic batch_lang *.o

