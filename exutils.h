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
** 	Header file which contains the defines and prototypes for the utility
** 	functions in exutils.c and exutils2.c.
** 
*/

/*****************************************************************************
** 
** defines and typedefs used 
** 
*****************************************************************************/

#ifndef MAX
#define MAX(X,Y)	(((X) > (Y)) ? (X) : (Y))
#endif

#ifndef MIN
#define MIN(X,Y)	(((X) < (Y)) ? (X) : (Y))
#endif

/*
** Maximum character buffer for displaying a column
*/
#define MAX_CHAR_BUF	1024

/*
** Useful macro's
*/
#define EX_RETURN_IF(_r, _msg) if (_r != CS_SUCCEED)\
	{\
		fprintf(EX_ERROR_OUT, "Error in %s on line %d: %s\n",\
			__FILE__, __LINE__, _msg);\
		return _r;\
	}



#define EX_EXIT_IF(_r, _msg) if (_r != CS_SUCCEED)\
	{\
		fprintf(EX_ERROR_OUT, "Error in %s on line %d: %s\n",\
			__FILE__, __LINE__, _msg);\
		exit(EX_EXIT_FAIL);\
	}

/*
** Define structure where row data is bound.
*/
typedef struct _ex_column_data
{
	CS_INT		indicator;
	CS_CHAR		*value;
	CS_INT		valuelen;
} EX_COLUMN_DATA;

typedef struct _ct_scroll_indexlist
{
        int          index;
        int          scrolltype;
} CT_SCROLL_INDEX;

/*
** Data structure for array binding of scrollable cursor
** results. This is an illustrative example only, there
** are different and more flexible ways to do columns.
** Column length is fixed in this example. It is advised
** to set ARRAY_BND_LEN to CS_CURSOR_ROWS for efficiency.
*/
#define ARRAY_BND_LEN 	5	/* so set CS_CURSOR_ROWS to 5 */
#define	MAX_COLUMN_LEN	45	/* maximum size of a column */

typedef struct ex_column_array {
	CS_DATAFMT	datafmt;
	CS_CHAR		value[ARRAY_BND_LEN][MAX_COLUMN_LEN];
	CS_INT		valuelen[ARRAY_BND_LEN];
	CS_SMALLINT	indicator[ARRAY_BND_LEN];
} COLUMN_ARRAY; 

/*****************************************************************************
** 
** protoypes for all public functions 
** 
*****************************************************************************/
/* exutils.c */
extern CS_INT CS_PUBLIC ex_display_dlen(
	CS_DATAFMT *column
	);
extern CS_RETCODE CS_PUBLIC ex_display_header(
	CS_INT numcols,
	CS_DATAFMT columns[]
	);
extern CS_RETCODE CS_PUBLIC ex_display_column(
	CS_CONTEXT *context,
	CS_DATAFMT *colfmt,
	CS_VOID *data,
	CS_INT datalength,
	CS_INT indicator
	);
extern CS_VOID CS_PUBLIC ex_panic(
	char *msg
	);
extern CS_VOID CS_PUBLIC ex_error(
	char *msg
	);
extern CS_RETCODE CS_PUBLIC ex_clientmsg_cb(
	CS_CONTEXT *context,
	CS_CONNECTION *connection,
	CS_CLIENTMSG *errmsg
	);
extern CS_RETCODE CS_PUBLIC ex_servermsg_cb(
	CS_CONTEXT *context,
	CS_CONNECTION *connection,
	CS_SERVERMSG *srvmsg
	);
extern CS_RETCODE CS_PUBLIC ex_init(
	CS_CONTEXT **context
	);
extern CS_RETCODE CS_PUBLIC ex_connect(
	CS_CONTEXT *context,
	CS_CONNECTION **connection,
	CS_CHAR *appname,
	CS_CHAR *username,
	CS_CHAR *password,
	CS_CHAR *server
	);
extern CS_RETCODE CS_PUBLIC ex_con_cleanup(
	CS_CONNECTION *connection,
	CS_RETCODE status
	);
extern CS_RETCODE CS_PUBLIC ex_ctx_cleanup(
	CS_CONTEXT *context,
	CS_RETCODE status
	);
extern CS_RETCODE CS_PUBLIC ex_execute_cmd(
	CS_CONNECTION *connection,
	CS_CHAR *cmdbuf
	);
extern CS_RETCODE CS_PUBLIC ex_fetch_data(
	CS_COMMAND *cmd
	);
extern CS_RETCODE CS_PUBLIC ex_create_db(
	CS_CONNECTION *connection,
	char *dbname
	);
extern CS_RETCODE ex_remove_db(
	CS_CONNECTION *connection,
	char *dbname
	);
extern CS_RETCODE ex_use_db(
	CS_CONNECTION *connection,
	char *dbname
	);
extern CS_RETCODE CS_PUBLIC ex_handle_results(
	CS_COMMAND *cmd
	);
