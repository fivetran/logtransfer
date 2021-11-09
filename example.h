/*
** Copyright Notice and Disclaimer
** -------------------------------
**      (c) Copyright 2013.
**      SAP AG or an SAP affiliate company. All rights reserved.
**      Unpublished rights reserved under U.S. copyright laws.
**
**      SAP grants Licensee a non-exclusive license to use, reproduce,
**      modify, and distribute the sample source code below (the "Sample Code"),
**      subject to the following conditions:
**
**      (i) redistributions must retain the above copyright notice;
**
**      (ii) SAP shall have no obligation to correct errors or deliver
**      updates to the Sample Code or provide any other support for the
**      Sample Code;
**
**      (iii) Licensee may use the Sample Code to develop applications
**      (the "Licensee Applications") and may distribute the Sample Code in
**      whole or in part as part of such Licensee Applications, however in no
**      event shall Licensee distribute the Sample Code on a standalone basis;
**
**      (iv) and subject to the following disclaimer:
**      THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
**      INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
**      AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
**      SAP AG or an SAP affiliate company OR ITS LICENSORS BE LIABLE FOR ANY DIRECT, 
**	INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
**	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
**	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
**	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
**	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
**      USE OF THE SAMPLE CODE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
**      DAMAGE.
**
** Description
** -----------
** 	This is the header file that goes with the SAP 
** 	Client-Library example programs.  
**
*/

/*
** Define symbolic names, constants, and macros
*/
#define	EX_MAXSTRINGLEN		255
#define	EX_BUFSIZE		1024
#define	EX_CTLIB_VERSION	CS_CURRENT_VERSION
#define	EX_BLK_VERSION		BLK_VERSION_160
#define	EX_ERROR_OUT		stderr
#define	EX_BADVAL		(CS_INT)-1
#define	EX_MAX_ARR		64

/*
** exit status values
*/
#define EX_EXIT_SUCCEED		0
#define EX_EXIT_FAIL		1

/*
** Define global variables used in all sample programs
*/
#define EX_SERVER 	NULL		/* use DSQUERY env var */
#define EX_USERNAME	"sa"
#define EX_PASSWORD 	"myPassword"

/*
** Uncomment the following line to test the HA Failover feature.
*/
/* #define HAFAILOVER 1 */

#define EX_SCREEN_INIT() 

