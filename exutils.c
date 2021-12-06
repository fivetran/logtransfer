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
** 	This example file contains utility routines which are used by all other 
** 	sample programs. It demonstrates how an application can hide some of the
** 	implementation details of CT-Lib from higher level programs.
**
** Routines Used
** -------------
** 	cs_ctx_alloc	
** 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctpublic.h>
#include "example.h"
#include "exutils.h"

/* 
** The macro PARTIAL_TEXT to enable partial text update is defined at the
** Makefile on uctext.
*/
#undef EX_API_DEBUG_CTX
#undef EX_API_DEBUG_CON

/*****************************************************************************
** 
** display functions 
** 
*****************************************************************************/

/*
** ex_display_dlen()
**
** Type of function:
** example program api
**
** Purpose:
**
** Returns:
** Nothing.
**
** Side Effects:
** None
*/

CS_INT CS_PUBLIC
ex_display_dlen(CS_DATAFMT *column)
{
	CS_INT		len;

	switch ((int) column->datatype)
	{
		case CS_CHAR_TYPE:
		case CS_LONGCHAR_TYPE:
		case CS_VARCHAR_TYPE:
		case CS_TEXT_TYPE:
		case CS_IMAGE_TYPE:
			len = MIN(column->maxlength, MAX_CHAR_BUF);
			break;

		case CS_UNICHAR_TYPE:
			len = MIN((column->maxlength / 2), MAX_CHAR_BUF);
			break;

		case CS_BINARY_TYPE:
		case CS_VARBINARY_TYPE:
			len = MIN((2 * column->maxlength) + 2, MAX_CHAR_BUF);
			break;

		case CS_BIT_TYPE:
		case CS_TINYINT_TYPE:
			len = 3;
			break;

		case CS_SMALLINT_TYPE:
			len = 6;
			break;

		case CS_INT_TYPE:
			len = 11;
			break;

		case CS_REAL_TYPE:
		case CS_FLOAT_TYPE:
			len = 20;
			break;

		case CS_MONEY_TYPE:
		case CS_MONEY4_TYPE:
			len = 24;
			break;

		case CS_DATETIME_TYPE:
		case CS_DATETIME4_TYPE:
        case CS_TIME_TYPE:
        case CS_BIGDATETIME_TYPE:
        case CS_BIGTIME_TYPE:
			len = 30;
			break;

		case CS_NUMERIC_TYPE:
		case CS_DECIMAL_TYPE:
			len = (CS_MAX_PREC + 2);
			break;

		default:
			len = 12;
			break;
	}

	return MAX((CS_INT)(strlen(column->name) + 1), len);
}

/*
** ex_display_header()
**
** Type of function:
** example program api
**
** Purpose:
**
** Returns:
** Nothing.
**
** Side Effects:
** None
*/

CS_RETCODE CS_PUBLIC
ex_display_header(CS_INT numcols, CS_DATAFMT columns[])
{
	CS_INT		i;
	CS_INT		l;
	CS_INT		j;
	CS_INT		disp_len;

	fputc('\n', stdout);
	for (i = 0; i < numcols; i++)
	{
		disp_len = ex_display_dlen(&columns[i]);
		fprintf(stdout, "%s", columns[i].name);
		fflush(stdout);
		l = disp_len - strlen(columns[i].name);
		for (j = 0; j < l; j++)
		{
			fputc(' ', stdout);
			fflush(stdout);
		}
	}
	fputc('\n', stdout);
	fflush(stdout);
	for (i = 0; i < numcols; i++)
	{
		disp_len = ex_display_dlen(&columns[i]);
		l = disp_len - 1;
		for (j = 0; j < l; j++)
		{
			fputc('-', stdout);
		}
		fputc(' ', stdout);
	}
	fputc('\n', stdout);

	return CS_SUCCEED;
}

/*
** ex_display_column()
**
** Type of function:
** example program api
**
** Purpose:
**
** Returns:
** Nothing.
**
** Side Effects:
** None
*/

CS_RETCODE CS_PUBLIC
ex_display_column(CS_CONTEXT *context, CS_DATAFMT *colfmt, CS_VOID *data, 
		  CS_INT datalength, CS_INT indicator)
{
	char		*null = "NULL";
	char		*nc   = "NO CONVERT";
	char		*cf   = "CONVERT FAILED";
	CS_DATAFMT	srcfmt;
	CS_DATAFMT	destfmt;
	CS_INT		olen;
	CS_CHAR		wbuf[MAX_CHAR_BUF];
	CS_BOOL		res;
	CS_INT		i;
	CS_INT		disp_len;
	CS_SMALLINT	indi;

	indi = (CS_SMALLINT)indicator;

	if (indi == CS_NULLDATA)
	{
		olen = strlen(null);
		strcpy(wbuf, null);
	}
	else
	{
		cs_will_convert(context, colfmt->datatype, CS_CHAR_TYPE, &res);
		if (res != CS_TRUE)
		{
			olen = strlen(nc);
			strcpy(wbuf, nc);
		}
		else
		{
			srcfmt.datatype  = colfmt->datatype;
			srcfmt.format    = colfmt->format;
			srcfmt.locale    = colfmt->locale;
			srcfmt.maxlength = datalength;

			memset(&destfmt, 0, sizeof(destfmt));
			memset(wbuf, 0, MAX_CHAR_BUF);

			destfmt.maxlength = MAX_CHAR_BUF;
			destfmt.datatype  = CS_CHAR_TYPE;
			destfmt.format    = CS_FMT_NULLTERM;
			destfmt.locale    = NULL;

			if (cs_convert(context, &srcfmt, data, &destfmt,
					wbuf, &olen) != CS_SUCCEED)
			{
				olen = strlen(cf);
				strcpy(wbuf, cf);
			}
			else
			{
				/*
				** output length include null
				** termination
				*/
				olen -= 1;
			}
		}
	}

	for (i = 0; i < olen; i++)
	{
		fprintf(stdout, "%c", wbuf[i]);
	}

	fflush(stdout);

	disp_len = ex_display_dlen(colfmt);
	for (i = 0; i < (disp_len - olen); i++)
	{
		fputc(' ', stdout);
	}
	fflush(stdout);
	
	return CS_SUCCEED;
}

/*
** ex_msg()
**
** Type of function:
** 	example program utility api
**
** Purpose:
** 	Reports a string message to EX_STANDARD_OUT.
**
** Returns:
** 	nothing
**
** Side Effects:
** 	none.
*/

CS_VOID CS_PUBLIC
ex_msg(char *msg)
{
    fprintf(EX_STANDARD_OUT, "MESSAGE: %s\n", msg);
    fflush(EX_STANDARD_OUT);
}

/*****************************************************************************
** 
** error functions 
** 
*****************************************************************************/

/*
** ex_panic()
**
** Type of function:
** 	example program utility api
**
** Purpose:
** 	Reports a string message to EX_ERROR_OUT, and exits program.
**
** Returns:
** 	nothing
**
** Side Effects:
** 	Terminates program
*/

CS_VOID CS_PUBLIC
ex_panic(char *msg)
{
	fprintf(EX_ERROR_OUT, "ex_panic: FATAL ERROR: %s\n", msg);
	fflush(EX_ERROR_OUT);
	exit(EX_EXIT_FAIL);
}

/*
** ex_error()
**
** Type of function:
** 	example program utility api
**
** Purpose:
** 	Reports a string message to EX_ERROR_OUT.
**
** Returns:
** 	nothing
**
** Side Effects:
** 	none.
*/

CS_VOID CS_PUBLIC
ex_error(char *msg)
{
	fprintf(EX_ERROR_OUT, "ERROR: %s\n", msg);
	fflush(EX_ERROR_OUT);
}

/*****************************************************************************
** 
** callback functions 
** 
*****************************************************************************/

/*
** ex_clientmsg_cb()
**
** Type of function:
** 	example program client message handler
**
** Purpose:
** 	Installed as a callback into Open Client.
**
** Returns:
** 	CS_SUCCEED
**
** Side Effects:
** 	None
*/

CS_RETCODE CS_PUBLIC
ex_clientmsg_cb(CS_CONTEXT *context, CS_CONNECTION *connection, CS_CLIENTMSG *errmsg)
{	

	/*
	** Suppress the 'cursor before first' and 'cursor after last' messages.
	** These are informational messages but come with "external error" string
	** potentially confusing users. These messages are related to the scrollable
	** cursor examples.
	*/
	if ((CS_NUMBER(errmsg->msgnumber) == 211) || (CS_NUMBER(errmsg->msgnumber) == 212))
	{
		return CS_SUCCEED;
	}
	
	fprintf(EX_ERROR_OUT, "\nOpen Client Message:\n");
	fprintf(EX_ERROR_OUT, "Message number: LAYER = (%d) ORIGIN = (%d) ",
		CS_LAYER(errmsg->msgnumber), CS_ORIGIN(errmsg->msgnumber));
	fprintf(EX_ERROR_OUT, "SEVERITY = (%d) NUMBER = (%d)\n",
		CS_SEVERITY(errmsg->msgnumber), CS_NUMBER(errmsg->msgnumber));
	fprintf(EX_ERROR_OUT, "Message String: %s\n", errmsg->msgstring);
	if (errmsg->osstringlen > 0)
	{
		fprintf(EX_ERROR_OUT, "Operating System Error: %s\n",
			errmsg->osstring);
	}
	fflush(EX_ERROR_OUT);

	return CS_SUCCEED;
}

/*
** ex_servermsg_cb()
**
** Type of function:
** 	example program server message handler
**
** Purpose:
** 	Installed as a callback into Open Client.
**
** Returns:
** 	CS_SUCCEED
**
** Side Effects:
** 	None
*/
CS_RETCODE CS_PUBLIC
ex_servermsg_cb(CS_CONTEXT *context, CS_CONNECTION *connection, CS_SERVERMSG *srvmsg)
{
	/*
	** Ignore the 'Changed database to' and 'Changed language to'
	** messages.
	*/
	if ((srvmsg->msgnumber == 5701) || (srvmsg->msgnumber == 5703))
	{
		return CS_SUCCEED;
	}
	 
	fprintf(EX_ERROR_OUT, "\nServer message:\n");
	fprintf(EX_ERROR_OUT, "Message number: %d, Severity %d, ",
		srvmsg->msgnumber, srvmsg->severity);
	fprintf(EX_ERROR_OUT, "State %d, Line %d\n",
		srvmsg->state, srvmsg->line);
	
	if (srvmsg->svrnlen > 0)
	{
		fprintf(EX_ERROR_OUT, "Server '%s'\n", srvmsg->svrname);
	}
	
	if (srvmsg->proclen > 0)
	{
		fprintf(EX_ERROR_OUT, " Procedure '%s'\n", srvmsg->proc);
	}

	fprintf(EX_ERROR_OUT, "Message String: %s\n", srvmsg->text);
	fflush(EX_ERROR_OUT);

	return CS_SUCCEED;
}

/*****************************************************************************
** 
** utility functions 
** 
*****************************************************************************/

/*
** ex_init()
**
** Type of function:
** 	example program utility api
**
** Purpose:
** 	This function allocates the context structure, initializes
**	Client-Library, and installs the default callbacks. If this function
**	should fail, it will deallocated all memory allocations it has done.
**
**	The callbacks are installed at the context level. Other applications
**	may need to install callbacks at the connection level.
**
** Parameters:
** 	context		- Pointer to A Pointer to CS_CONTEXT structure.
**
** Returns:
** 	Result of initialization functions from CT-Lib.
**
*/

CS_RETCODE CS_PUBLIC
ex_init(CS_CONTEXT **context)
{
	CS_RETCODE	retcode;
	CS_INT		netio_type = CS_SYNC_IO;

	/*
	** Get a context handle to use.
	*/
	retcode = cs_ctx_alloc(EX_CTLIB_VERSION, context);
	if (retcode != CS_SUCCEED)
	{
		ex_error("ex_init: cs_ctx_alloc() failed");
		return retcode;
	}

	/*
	** Initialize Open Client.
	*/
	retcode = ct_init(*context, EX_CTLIB_VERSION);
	if (retcode != CS_SUCCEED)
	{
		ex_error("ex_init: ct_init() failed");
		cs_ctx_drop(*context);
		*context = NULL;
		return retcode;
	}

#ifdef EX_API_DEBUG_CTX
	/*
	** ct_debug stuff. Enable this function right before any call to
	** OC-Lib that is returning failure.
	*/
	retcode = ct_debug(*context, NULL, CS_SET_FLAG, CS_DBG_API_STATES,
				NULL, CS_UNUSED);
	if (retcode != CS_SUCCEED)
	{
		ex_error("ex_init: ct_debug() failed");
	}
#endif

	/*
	** Install client and server message handlers.
	*/
	if (retcode == CS_SUCCEED)
	{
		retcode = ct_callback(*context, NULL, CS_SET, CS_CLIENTMSG_CB,
					(CS_VOID *)ex_clientmsg_cb);
		if (retcode != CS_SUCCEED)
		{
			ex_error("ex_init: ct_callback(clientmsg) failed");
		}
	}

	if (retcode == CS_SUCCEED)
	{
		retcode = ct_callback(*context, NULL, CS_SET, CS_SERVERMSG_CB,
				(CS_VOID *)ex_servermsg_cb);
		if (retcode != CS_SUCCEED)
		{
			ex_error("ex_init: ct_callback(servermsg) failed");
		}
	}

	/* 
	** This is an synchronous example so set the input/output type
	** to synchronous (This is the default setting, but show an
	** example anyway).
	*/
	if (retcode == CS_SUCCEED)
	{
		retcode = ct_config(*context, CS_SET, CS_NETIO, &netio_type, 
						CS_UNUSED, NULL);
		if (retcode != CS_SUCCEED)
		{
			ex_error("ex_init: ct_config(netio) failed");
		}
	}

	if (retcode != CS_SUCCEED)
	{
		ct_exit(*context, CS_FORCE_EXIT);
		cs_ctx_drop(*context);
		*context = NULL;
	}

	return retcode;
}

/*
** ex_connect()
**
** Type of function:
** 	example program utility api
**
** Purpose:
** 	This routine establishes a connection to the server identified
** 	in example.h and sets the CS_USER, CS_PASSWORD, and 
** 	CS_APPNAME properties for the connection. 
**
** 	If a connection property is NULL, it is not set.
**
** 	If this function should fail, it will deallocated all memory 
** 	allocations it has done.
**
** Parameters:
** 	context		- Pointer to CS_CONTEXT structure.
** 	connection 	- Pointer to CS_CONNECTION pointer.
**	appname		- Name of application calling this routine.
**	username	- user name to use when connecting.
**	password	- password to use when connecting.
**	server		- server to connect to.
**
** Return:
** 	Result of function calls from CT-Lib.
*/

CS_RETCODE CS_PUBLIC
ex_connect(CS_CONTEXT *context, CS_CONNECTION **connection, CS_CHAR *appname, 
	   CS_CHAR *username, CS_CHAR *password, CS_CHAR *server)
{
	CS_INT		len;
	CS_RETCODE	retcode;
#ifdef HAFAILOVER
	CS_BOOL		hafailover = CS_TRUE;
#endif  /* HAFAILOVER */
#ifdef PARTIAL_TEXT
	CS_BOOL		partial = CS_TRUE;
#endif

	/* 
	** Allocate a connection structure. 
	*/
	retcode = ct_con_alloc(context, connection);
	if (retcode != CS_SUCCEED)
	{
		ex_error("ct_con_alloc failed");
		return retcode;
	}

#ifdef EX_API_DEBUG_CON

	/*
	** ct_debug stuff. Enable this function on the connection if you
	** want to write TDS information for debug purposes.
	*/
	retcode = ct_debug(NULL, *connection, CS_SET_PROTOCOL_FILE, 
					CS_UNUSED, "tds_log_file", CS_NULLTERM);

	if (retcode != CS_SUCCEED)
	{
		ex_error("ex_init: ct_debug PROTOCOL_FILE failed");
	}

	retcode = ct_debug(NULL, *connection, CS_SET_FLAG, 
					CS_DBG_PROTOCOL, NULL, CS_UNUSED);

	if (retcode != CS_SUCCEED)
	{
		ex_error("ex_init: ct_debug CS_DBG_PROTOCOL failed");
	}
#endif

	/*	
	** If a username is defined, set the CS_USERNAME property.
	*/
	if (retcode == CS_SUCCEED && username != NULL)
	{
		if ((retcode = ct_con_props(*connection, CS_SET, CS_USERNAME, 
				username, CS_NULLTERM, NULL)) != CS_SUCCEED)
		{
			ex_error("ct_con_props(username) failed");
		}
	}

	/*	
	** If a password is defined, set the CS_PASSWORD property.
	*/
	if (retcode == CS_SUCCEED && password != NULL)
	{
		if ((retcode = ct_con_props(*connection, CS_SET, CS_PASSWORD, 
				password, CS_NULLTERM, NULL)) != CS_SUCCEED)
		{
			ex_error("ct_con_props(password) failed");
		}
	}

	/*	
	** Set the CS_APPNAME property.
	*/
	if (retcode == CS_SUCCEED && appname != NULL)
	{
		if ((retcode = ct_con_props(*connection, CS_SET, CS_APPNAME, 
				appname, CS_NULLTERM, NULL)) != CS_SUCCEED)
		{
			ex_error("ct_con_props(appname) failed");
		}
	}

#ifdef HAFAILOVER

	/*	
	** Set the CS_HAFAILOVER property.
	*/
	if (retcode == CS_SUCCEED)
	{
		if ((retcode = ct_con_props(*connection, CS_SET, CS_HAFAILOVER, 
				&hafailover, CS_UNUSED, NULL)) != CS_SUCCEED)
		{
			ex_error("ct_con_props(CS_HAFAILOVER) failed");
		}
	}
#endif  /* HAFAILOVER */


#ifdef PARTIAL_TEXT
	/*
	** Set the CS_PARTIAL_TEXT property.
	*/
	if (retcode == CS_SUCCEED)
	{
		if ((retcode = ct_con_props(*connection, CS_SET,
			CS_PARTIAL_TEXT, &partial, CS_UNUSED, NULL))
			!= CS_SUCCEED)
		{
			printf("ct_con_props(partial update) failed\n");
		}
	}
#endif /* PARTIAL_TEXT */

	/*	
	** Open a Server connection.
	*/
	if (retcode == CS_SUCCEED)
	{
		len = (server == NULL) ? 0 : CS_NULLTERM;
		retcode = ct_connect(*connection, server, len);
		if (retcode != CS_SUCCEED)
		{
			ex_error("ct_connect failed");
		}
	}

	if (retcode != CS_SUCCEED)
	{
		ct_con_drop(*connection);
		*connection = NULL;
	}

	return retcode;
}

/*
** ex_con_cleanup()
**
** Type of function:
** 	example program utility api
**
** Purpose:
** 	The routine closes a connection and deallocates the
**	CS_CONNECTION structure.
**
** Parameters:
** 	connection	- Pointer to connection structure.
** 	status		- status of last interaction with this Client-Library
**			  If not ok, this routine will perform a force close.
**
** Returns:
** 	Result of function calls from CT-Lib.
*/

CS_RETCODE CS_PUBLIC
ex_con_cleanup(CS_CONNECTION *connection, CS_RETCODE status)
{
	CS_RETCODE	retcode;
	CS_INT		close_option;

	close_option = (status != CS_SUCCEED) ? CS_FORCE_CLOSE : CS_UNUSED;
	retcode = ct_close(connection, close_option);
	if (retcode != CS_SUCCEED)
	{
		ex_error("ex_con_cleanup: ct_close() failed");
		return retcode;
	}
	retcode = ct_con_drop(connection);
	if (retcode != CS_SUCCEED)
	{
		ex_error("ex_con_cleanup: ct_con_drop() failed");
		return retcode;
	}

	return retcode;
}

/*
** ex_ctx_cleanup()
**
** Type of function:
** 	example program utility api
**
** Purpose:
** 	The routine exits Client-Library deallocates the
**	CS_CONTEXT structure.
**
** Parameters:
** 	context		- Pointer to context structure.
** 	status		- status of last interaction with Client-Library.
**			  If not ok, this routine will perform a force exit.
**
** Returns:
** 	Result of function calls from CT-Lib.
*/

CS_RETCODE CS_PUBLIC
ex_ctx_cleanup(CS_CONTEXT *context, CS_RETCODE status)
{
	CS_RETCODE	retcode;
	CS_INT		exit_option;

	exit_option = (status != CS_SUCCEED) ? CS_FORCE_EXIT : CS_UNUSED;
	retcode = ct_exit(context, exit_option);
	if (retcode != CS_SUCCEED)
	{
		ex_error("ex_ctx_cleanup: ct_exit() failed");
		return retcode;
	}
	retcode = cs_ctx_drop(context);
	if (retcode != CS_SUCCEED)
	{
		ex_error("ex_ctx_cleanup: cs_ctx_drop() failed");
		return retcode;
	}
	return retcode;
}

/*
** ex_execute_cmd()
**
** Type of function:   
**      example program utility api
**
** Purpose:
**      This routine sends a language command to the server. It expects no
**	rows or parameters to be returned from the server.
**
** Parameters:   
**      connection       - Pointer to CS_COMMAND structure.
**      cmdbuf          - The buffer containing the command.
**
** Return:
**      Result of functions called in CT-Lib
*/

CS_RETCODE CS_PUBLIC
ex_execute_cmd(CS_CONNECTION *connection, CS_CHAR *cmdbuf)
{
	CS_RETCODE      retcode;
	CS_INT          restype;
	CS_COMMAND      *cmd;
	CS_RETCODE      query_code;

	/*
	** Get a command handle, store the command string in it, and
	** send it to the server.
	*/
	if ((retcode = ct_cmd_alloc(connection, &cmd)) != CS_SUCCEED)
	{
		ex_error("ex_execute_cmd: ct_cmd_alloc() failed");
		return retcode;
	}

	if ((retcode = ct_command(cmd, CS_LANG_CMD, cmdbuf, CS_NULLTERM,
			CS_UNUSED)) != CS_SUCCEED)
	{
		ex_error("ex_execute_cmd: ct_command() failed");
		(void)ct_cmd_drop(cmd);
		return retcode;
	}

	if ((retcode = ct_send(cmd)) != CS_SUCCEED)
	{
		ex_error("ex_execute_cmd: ct_send() failed");
		(void)ct_cmd_drop(cmd);
		return retcode;
	}

	/*
	** Examine the results coming back. If any errors are seen, the query
	** result code (which we will return from this function) will be
	** set to FAIL.
	*/
	query_code = CS_SUCCEED;
	while ((retcode = ct_results(cmd, &restype)) == CS_SUCCEED)
	{
		switch((int)restype)
		{
		    case CS_CMD_SUCCEED:
		    case CS_CMD_DONE:
			break;

		    case CS_CMD_FAIL:
			query_code = CS_FAIL;
			break;

		    case CS_STATUS_RESULT:
			retcode = ct_cancel(NULL, cmd, CS_CANCEL_CURRENT);
			if (retcode != CS_SUCCEED)
			{
				ex_error("ex_execute_cmd: ct_cancel() failed");
				query_code = CS_FAIL;
			}
			break;

		    default:
			/*
			** Unexpected result type.
			*/
			query_code = CS_FAIL;
			break;
		}
		if (query_code == CS_FAIL)
		{
			/*
			** Terminate results processing and break out of
			** the results loop
			*/
			retcode = ct_cancel(NULL, cmd, CS_CANCEL_ALL);
			if (retcode != CS_SUCCEED)
			{
				ex_error("ex_execute_cmd: ct_cancel() failed");
			}
			break;
		}
	}

	/*
	** Clean up the command handle used
	*/
	if (retcode == CS_END_RESULTS)
	{
		retcode = ct_cmd_drop(cmd);
		if (retcode != CS_SUCCEED)
		{
			query_code = CS_FAIL;
		}
	}
	else
	{
		(void)ct_cmd_drop(cmd);
		query_code = CS_FAIL;
	}

	if (query_code == CS_FAIL)
	{
		ex_error("ex_execute_cmd: The following command caused an error:");
		ex_error(cmdbuf);
	}
	return query_code;
}

/*
** ex_fetch_data()
**
** Type of function:
** 	example program utility api
** 
** Purpose:
** 	This function processes fetchable results sets. The results include:
**
**		CS_ROW_RESULT
**		CS_CURSOR_RESULT 
**		CS_PARAM_RESULT 
**		CS_STATUS_RESULT 
**		CS_COMPUTE_RESULT 
**
**	Since the Client-Library result model has been unified, the same
**	apis are used for each of the above result types.
**
**	One caveat is the processing of CS_COMPUTE_RESULTs. The name field
**	sent from the server is typically zero length. To display a meaningful
**	header, the aggregate compute operator name should be found for the
**	column, and that name used instead. The compute example program has
**	code which demonstrates this.
**
** Parameters:
**	cmd - Pointer to command structure
**
** Return:
**	CS_MEM_ERROR	If an memory allocation failed.
**	CS_SUCCEED	If the data was displayed.
**	CS_FAIL		If no columns were present.
** 	<retcode>	Result of the Client-Library function if a failure was
**			returned.
**
*/

CS_RETCODE CS_PUBLIC
ex_fetch_data(CS_COMMAND *cmd)
{
	CS_RETCODE		retcode;
	CS_INT			num_cols;
	CS_INT			i;
	CS_INT			j;
	CS_INT			row_count = 0;
	CS_INT			rows_read;
	CS_INT			disp_len;
	CS_DATAFMT		*datafmt;
	EX_COLUMN_DATA		*coldata;

	/*
	** Find out how many columns there are in this result set.
	*/
	retcode = ct_res_info(cmd, CS_NUMDATA, &num_cols, CS_UNUSED, NULL);
	if (retcode != CS_SUCCEED)
	{
		ex_error("ex_fetch_data: ct_res_info() failed");
		return retcode;
	}

	/*
	** Make sure we have at least one column
	*/
	if (num_cols <= 0)
	{
		ex_error("ex_fetch_data: ct_res_info() returned zero columns");
		return CS_FAIL;
	}

	/*
	** Our program variable, called 'coldata', is an array of 
	** EX_COLUMN_DATA structures. Each array element represents
	** one column.  Each array element will re-used for each row.
	**
	** First, allocate memory for the data element to process.
	*/
	coldata = (EX_COLUMN_DATA *)malloc(num_cols * sizeof (EX_COLUMN_DATA));
	if (coldata == NULL)
	{
		ex_error("ex_fetch_data: malloc() failed");
		return CS_MEM_ERROR;
	}

	datafmt = (CS_DATAFMT *)malloc(num_cols * sizeof (CS_DATAFMT));
	if (datafmt == NULL)
	{
		ex_error("ex_fetch_data: malloc() failed");
		free(coldata);
		return CS_MEM_ERROR;
	}

	/*
	** Loop through the columns getting a description of each one
	** and binding each one to a program variable.
	**
	** We're going to bind each column to a character string; 
	** this will show how conversions from server native datatypes
	** to strings can occur via bind.
	**
	** We're going to use the same datafmt structure for both the describe
	** and the subsequent bind.
	**
	** If an error occurs within the for loop, a break is used to get out
	** of the loop and the data that was allocated is free'd before
	** returning.
	*/
	for (i = 0; i < num_cols; i++)
	{
		/*
		** Get the column description.  ct_describe() fills the
		** datafmt parameter with a description of the column.
		*/
		retcode = ct_describe(cmd, (i + 1), &datafmt[i]);
		if (retcode != CS_SUCCEED)
		{
			ex_error("ex_fetch_data: ct_describe() failed");
			break;
		}

		/*
		** update the datafmt structure to indicate that we want the
		** results in a null terminated character string.
		**
		** First, update datafmt.maxlength to contain the maximum
		** possible length of the column. To do this, call
		** ex_display_len() to determine the number of bytes needed
		** for the character string representation, given the
		** datatype described above.  Add one for the null
		** termination character.
		*/
		datafmt[i].maxlength = ex_display_dlen(&datafmt[i]) + 1;
		
		/*
		** Set datatype and format to tell bind we want things
		** converted to null terminated strings
		*/
		datafmt[i].datatype = CS_CHAR_TYPE;
		datafmt[i].format   = CS_FMT_NULLTERM;

		/*
		** Allocate memory for the column string
		*/
		coldata[i].value = (CS_CHAR *)malloc(datafmt[i].maxlength);
		if (coldata[i].value == NULL)
		{
			ex_error("ex_fetch_data: malloc() failed");
			retcode = CS_MEM_ERROR;
			break;
		}

		/*
		** Now bind.
		*/
		retcode = ct_bind(cmd, (i + 1), &datafmt[i],
				coldata[i].value, &coldata[i].valuelen,
				(CS_SMALLINT *)&coldata[i].indicator);
		if (retcode != CS_SUCCEED)
		{
			ex_error("ex_fetch_data: ct_bind() failed");
			break;
		}
	}
	if (retcode != CS_SUCCEED)
	{
		for (j = 0; j < i; j++)
		{
			free(coldata[j].value);
		}
		free(coldata);
		free(datafmt);
		return retcode;
	}

	/*
	** Display column header
	*/
	ex_display_header(num_cols, datafmt);

	/*
	** Fetch the rows.  Loop while ct_fetch() returns CS_SUCCEED or 
	** CS_ROW_FAIL
	*/
	while (((retcode = ct_fetch(cmd, CS_UNUSED, CS_UNUSED, CS_UNUSED,
			&rows_read)) == CS_SUCCEED) || (retcode == CS_ROW_FAIL))
	{
		/*
		** Increment our row count by the number of rows just fetched.
		*/
		row_count = row_count + rows_read;

		/*
		** Check if we hit a recoverable error.
		*/
		if (retcode == CS_ROW_FAIL)
		{
			fprintf(stdout, "Error on row %d.\n", row_count);
			fflush(stdout);
		}

		/*
		** We have a row.  Loop through the columns displaying the
		** column values.
		*/
		for (i = 0; i < num_cols; i++)
		{	  
			/*
			** Display the column value
			*/
			fprintf(stdout, "%s", coldata[i].value);
			fflush(stdout);

			/*
			** If not last column, Print out spaces between this
			** column and next one. 
			*/
			if (i != num_cols - 1)
			{
				disp_len = ex_display_dlen(&datafmt[i]);
				disp_len -= coldata[i].valuelen - 1;
				for (j = 0; j < disp_len; j++)
				{
					fputc(' ', stdout);
				}
			}
		} 
		fprintf(stdout, "\n");
		fflush(stdout);
	}

	/*
	** Free allocated space.
	*/
	for (i = 0; i < num_cols; i++)
	{
		free(coldata[i].value);
	}
	free(coldata);
	free(datafmt);

	/*
	** We're done processing rows.  Let's check the final return
	** value of ct_fetch().
	*/
	switch ((int)retcode)
	{
		case CS_END_DATA:
			/*
			** Everything went fine.
			*/
			fprintf(stdout, "All done processing rows.\n");
			fflush(stdout);
			retcode = CS_SUCCEED;
			break;

		case CS_FAIL:
			/*
			** Something terrible happened.
			*/
			ex_error("ex_fetch_data: ct_fetch() failed");
			break;

		default:
			/*
			** We got an unexpected return value.
			*/
			ex_error("ex_fetch_data: ct_fetch() returned an expected retcode");
			break;

	}
	return retcode;
}

/*****************************************************************************
** 
** sql based functions 
** 
*****************************************************************************/

/*
** ex_create_db()
**
** Type of function:
**      example program utility api
**
** Purpose:
**      This routine creates a database and opens it. It first checks
**      that the database does not already exists. If it does exist
**      the database is dropped before creating a new one.
**
** Parameters:
**      connection      - Pointer to CS_CONNECTION structure.
**      dbname          - The name to be used for the created database.
**
** Return:
**      Result of functions called in CT-Lib.
**
*/

CS_RETCODE CS_PUBLIC
ex_create_db(CS_CONNECTION *connection, char *dbname)
{
	CS_RETCODE      retcode;
	CS_CHAR         *cmdbuf;

	/*
	** If the database already exists, drop it.
	*/
	if ((retcode = ex_remove_db(connection, dbname)) != CS_SUCCEED)
	{
		ex_error("ex_create_db: ex_remove_db() failed");
	}

	/*
	** Allocate the buffer for the command string.
	*/
	cmdbuf = (CS_CHAR *) malloc(EX_BUFSIZE);
	if (cmdbuf == NULL)
	{
		ex_error("ex_create_db: malloc() failed");
		return CS_FAIL;
	}

	/*
	** Set up and send the command to create the database.
	*/
	sprintf(cmdbuf, "create database %s on example_db_tgt_data", dbname);
	if ((retcode = ex_execute_cmd(connection, cmdbuf)) != CS_SUCCEED)
	{
		ex_error("ex_create_db: ex_execute_cmd(create db) failed");
	}
	free(cmdbuf);
	return retcode;
}

/*
** ex_remove_db()
**
** Type of function:
**      example program utility api
**
** Purpose:
**      This routine removes a database. It first checks that
**      the database exists, and if so, removes it.
**
** Parameters:
**      connection      - Pointer to CS_CONNECTION structure.
**      dbname          - The name of the database to remove.
**
** Return:
**      Result of functions called in CT-Lib or CS_FAIL if a malloc failure
**      occurred.
*/

CS_RETCODE
ex_remove_db(CS_CONNECTION *connection, char *dbname)
{
	CS_RETCODE      retcode;
	CS_CHAR         *cmdbuf;

	/*
	** Connect to the master database in order to
	** remove the specified database.
	*/
	if ((retcode = ex_use_db(connection, "master")) != CS_SUCCEED)
	{
		ex_error("ex_remove_db: ex_use_db(master) failed");
		return retcode;
	}

	/*
	** Allocate the buffer for the command string.
	*/
	cmdbuf = (CS_CHAR *) malloc(EX_BUFSIZE);
	if (cmdbuf == NULL)
	{
		ex_error("ex_remove_db: malloc() failed");
		return CS_FAIL;
	}

	/*
	** Set up and send the command to check for and drop the
	** database if it exists.
	*/
	sprintf(cmdbuf,
		"if exists (select name from sysdatabases where name = \"%s\") \
		drop database %s", dbname, dbname);
	if ((retcode = ex_execute_cmd(connection, cmdbuf)) != CS_SUCCEED)
	{
		ex_error("ex_remove_db: ex_execute_cmd(drop db) failed");
	}
	free(cmdbuf);
	return retcode;
}

/*
** ex_use_db()
**
** Type of function:
**      example program utility api
**
** Purpose:
**      This routine changes the current database to the named db passed in.
**
** Parameters:
**      connection      - Pointer to CS_CONNECTION structure.
**      dbname          - The name of the database to use.
**
** Return:
**      Result of functions called in CT-Lib or CS_FAIL if a malloc failure
**      occured.
*/

CS_RETCODE
ex_use_db(CS_CONNECTION *connection, char *dbname)
{
	CS_RETCODE      retcode;
	CS_CHAR         *cmdbuf;

	/*
	** Allocate the buffer for the command string.
	*/
	cmdbuf = (CS_CHAR *) malloc(EX_BUFSIZE);
	if (cmdbuf == NULL)
	{
		ex_error("ex_use_db: malloc() failed");
		return CS_FAIL;
	}

	/*
	** Set up and send the command to use the database
	*/
	sprintf(cmdbuf, "use %s\n", dbname);
	if ((retcode = ex_execute_cmd(connection, cmdbuf)) != CS_SUCCEED)
	{
		ex_error("ex_use_db: ex_execute_cmd(use db) failed");
	}
	free(cmdbuf);
	return retcode;
}


/*
** ex_handle_results
**
** Generic example result processing.
*/
CS_RETCODE CS_PUBLIC
ex_handle_results(CS_COMMAND *cmd)
{
	CS_RETCODE retcode;
	CS_INT res_type;
	CS_SMALLINT msg_id;

	/*
	** Process the results.
	*/
	while ((retcode = ct_results(cmd, &res_type)) == CS_SUCCEED)
	{
		switch ((int)res_type)
		{
		  case CS_ROW_RESULT:
		  case CS_PARAM_RESULT:
		  case CS_STATUS_RESULT:
			/* 
			** Print the result header based on the result type.
			*/
			switch ((int)res_type)
			{
			  case  CS_ROW_RESULT:
				fprintf(stdout, "\nROW RESULTS\n");
				break;

			  case  CS_PARAM_RESULT:
				fprintf(stdout, "\nPARAMETER RESULTS\n");
				break;

			  case  CS_STATUS_RESULT:
				fprintf(stdout, "\nSTATUS RESULTS\n");
				break;
			}
	
			/*
			** All three of these result types are fetchable.
			*/
			retcode = ex_fetch_data(cmd);
			if (retcode != CS_SUCCEED)
			{
				ex_error("ex_handle_results: ex_fetch_data() failed");
				return retcode;
			}
			break;

		  case CS_MSG_RESULT:
			/*
			**  Retrieve and print the message ID.
			*/
			retcode = ct_res_info(cmd, CS_MSGTYPE,
					(CS_VOID *)&msg_id, CS_UNUSED, NULL);
			if (retcode != CS_SUCCEED)
			{
				ex_error("ex_handle_results: ct_res_info(msgtype) failed");
				return retcode;
			}
			fprintf(stdout, "ct_result returned CS_MSG_RESULT where msg id = %d.\n",
				 msg_id);
			break;

		  case CS_CMD_SUCCEED:
			/*
			** This means no rows were returned.
			*/
			break;

		  case CS_CMD_DONE:
			/*
			** Done with result set.
			*/
			break;

		  case CS_CMD_FAIL:
			/*
			** The server encountered an error while
			** processing our command.
			*/
			ex_error("ex_handle_results: ct_results returned CS_CMD_FAIL.");
			return CS_FAIL;
			break;

		  default:
			/*
			** We got something unexpected.
			*/
			ex_error("ex_handle_results: ct_results returned unexpected result type");
			return CS_FAIL;
		}
	}

	/*
	** We're done processing results. Let's check the
	** return value of ct_results() to see if everything
	** went ok.
	*/
	switch (retcode)
	{
	  case CS_END_RESULTS:
		/*
		** Everything went fine.
		*/
		retcode = CS_SUCCEED;
		break;

	  case CS_FAIL:
		/*
		** Something failed happened.
		*/
		ex_error("ex_handle_results: ct_results() failed");
		break;

	  default:
		/*
		** We got an unexpected return value.
		*/
		ex_error("ex_handle_results: ct_results returned unexpected result type");
		break;
	}

	return retcode;
}
