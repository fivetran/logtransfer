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
all: rpc logtransfer

exutils.o: exutils.c example.h exutils.h
	@ printf "$(COMPILE) -c exutils.c -o exutils.o\n\n";
	@ $(COMPILE) -c exutils.c -o exutils.o

rpc: rpc.c exutils.o
	@ printf "$(COMPILE) rpc.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@\n\n";
	@ $(COMPILE) rpc.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@

logtransfer: logtransfer.c exutils.o
	@ printf "$(COMPILE) logtransfer.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@\n\n";
	@ $(COMPILE) logtransfer.c exutils.o $(LIBPATH) $(CTLIBS) $(COMLIBS) $(SYSLIBS)  -o $@

#
# Clean all binaries
#
clean: 
	rm -f rpc logtransfer *.o

