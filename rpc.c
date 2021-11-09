/*
** RPC COMMAND EXAMPLE PROGRAM
** ---------------------------
**
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
**	This example program demonstrates sending a RPC command 
**	to a Server and the processing of row, parameter, and status
**	results returned from the remote procedure.
**
**	The example uses standard ANSI C for input/output and
**	memory management.
**
**	All work is performed synchronously.
**
** Routines Used
** -------------
**	All the required routines required for establishing and closing
**	a connection to a server, sending a language command to a
**	server, and processing row results.
**
**	In addition, the following routines were used to demonstrate
**	sending and processing a RPC command:
** 
**	ct_param()
**	ct_bind()
**	cs_convert()
**	ct_res_info()
**	ct_command()
** 
** Input
** -----
**	No input is required. Information normally required
**	from the user is retrieved from global variables defined
**	in the example header files.
**
** Output
** ------
**	The example program displays the row results
**	status results, parameter results, and server message
**	returned by the remote procedure.
**
** Server Dependencies
** -------------------
**	If connecting to an Open Server, the Open Server must be able 
**	to handle language commands intended for ASE.
**
** Server Tables
** -------------
**	No tables are used. All data is internal to the 
**
** Algorithm
** ----------
**	Initialize Client-Library.
**
**	install message handling callbacks.
**
**	Establish a connections. 
**
**	Create a database.
**
**	Create a stored procedure.
**
**	Execute the stored procedure.
**
**	Retrieve and display the results returned from the stored 
**	procedure.
**
**	Perform cleanup by dropping the database and the connection,
**	deallocating memory allocated for commands, connections, and 
**	contexts, and exiting Client-Library.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctpublic.h>
#include "example.h"
#include "exutils.h"

/*****************************************************************************
** 
** defines and globals used.
** 
*****************************************************************************/
/*
** Global names used in this module
*/
CS_CHAR *Ex_appname = "rpc_example";
CS_CHAR	*Ex_dbname = "sampledb";
CS_CHAR *Ex_server   = EX_SERVER;
CS_CHAR *Ex_username = EX_USERNAME;
CS_CHAR *Ex_password = EX_PASSWORD;

/*
** Prototypes for routines in the example code.
*/
CS_STATIC CS_RETCODE CreateStoredProcedure(CS_CONNECTION *connection);
CS_STATIC CS_RETCODE InstallNulls(CS_CONTEXT *context);
CS_STATIC CS_RETCODE DoRpc(CS_CONNECTION *connection);


/*
** main()
** 
** Purpose:
**	Entry point for example program.
** 
** Parameters:
**	None, argc and argv will not be used.
**
** Return:
** 	EX_EXIT_ERROR  or EX_EXIT_SUCCEED
**
*/

int
main(int argc, char *argv[])
{
	CS_CONTEXT	*context;
	CS_CONNECTION	*connection;
	CS_RETCODE	retcode;
	
	fprintf(stdout, "RPC Example\n");
	fflush(stdout);

	/* 
	** Allocate a context structure and initialize Client-Library 
	*/
	retcode = ex_init(&context);
	if (retcode != CS_SUCCEED)
	{
		ex_panic("ex_init failed");
	}

	/* 
	** Allocate a connection structure, set its properties, and  
	** establish a connection.
	*/
	retcode = ex_connect(context, &connection, Ex_appname,
				Ex_username, Ex_password, Ex_server);

	/*
	** Create a database for the sample program and change to it.
	*/
	if (retcode == CS_SUCCEED)
	{
		retcode = ex_create_db(connection, Ex_dbname);
	}

	/*
	** Create a stored procedure to execute
	*/
	if (retcode == CS_SUCCEED)
	{
		retcode = CreateStoredProcedure(connection);
	}

	/*
	** Install our null values to display
	*/
	if (retcode == CS_SUCCEED)
	{
		retcode = InstallNulls(context);
	}

	/*
	** Execute the the newly created RPC
	*/
	if (retcode == CS_SUCCEED)
	{
		retcode = DoRpc(connection);

		/*
		** Remove the database that was created.  The error
		** code (if not succeed), will be used in the cleanup
		** routines.
		*/
		if (retcode == CS_SUCCEED)
		{
			retcode = ex_remove_db(connection, Ex_dbname);
		}
		else
		{
			(void)ex_remove_db(connection, Ex_dbname);
		}
	}

	/*
	** Deallocate the allocated structures, close the connection,
	** and exit Client-Library.
	*/
	if (connection != NULL)
	{
		retcode = ex_con_cleanup(connection, retcode);
	}
	
	if (context != NULL)
	{
		retcode = ex_ctx_cleanup(context, retcode);
	}

	return (retcode == CS_SUCCEED) ? EX_EXIT_SUCCEED : EX_EXIT_FAIL;
}

/*
** CreateStoredProcedure()
**
** Type of function:
** 	rpc program internal api
**
** Purpose:
** 	Create a stored procedure in the server for subsequent.
**
** Parameters:
** 	connection	- Pointer to CS_CONNECTION structure.
**
** Return:
**	CS_SUCCEED if rpc was created.
**	Otherwise a Client-Library failure code.
*/

CS_STATIC CS_RETCODE 
CreateStoredProcedure(CS_CONNECTION *connection)
{

	CS_RETCODE	retcode;
	CS_CHAR		*cmdbuf;
	
	if ((retcode = ex_use_db(connection, Ex_dbname)) != CS_SUCCEED)
	{
		ex_error("CreateStoredProcedure: ex_use_db() failed");
		return retcode;
	}

	/* 
	** Allocate the buffer for the command string.
	*/
	cmdbuf = (CS_CHAR *) malloc(EX_BUFSIZE);
	if (cmdbuf == NULL)
	{
		ex_error("CreateTable: malloc() failed");
		return CS_MEM_ERROR;
	}

	/* 
	** Build the command for creating the stored procedure.
	** First, drop the stored procedure if it already exits.
	*/
	strcpy(cmdbuf, "if exists (select name from sysobjects \
			where type = \"P\" and name = \"sample_rpc\") \
			Begin	\
				drop proc sample_rpc	\
			End ");
	if ((retcode = ex_execute_cmd(connection, cmdbuf)) != CS_SUCCEED)
	{
		ex_error("CreateTable: ex_execute_cmd(drop table) failed");
		free (cmdbuf);
		return retcode;
	}
		
	/* 
	** Define the parameters.
	*/
	strcpy(cmdbuf, "create proc sample_rpc (@intparam int, \
		@sintparam smallint output, @floatparam float output, \
		@moneyparam money output,  \
		@dateparam datetime output, @charparam char(20) output, \
		@binaryparam	binary(20) output) \
		as ");
	/* 
	** Define queries to return row results, assign parameter values,
	** and return a message result.
	*/
	strcat(cmdbuf, "select @intparam, @sintparam, @floatparam, @moneyparam, \
		@dateparam, @charparam, @binaryparam \
		select @sintparam = @sintparam + @intparam \
		select @floatparam = @floatparam + @intparam \
		select @moneyparam = @moneyparam + convert(money, @intparam) \
		select @dateparam = getdate() \
		select @charparam = \"The char parameters\" \
		select @binaryparam = @binaryparam \
		print \"This is the message printed out by sample_rpc.\"");
	if ((retcode = ex_execute_cmd(connection, cmdbuf)) != CS_SUCCEED)
	{
		ex_error("CreateTable: ex_execute_cmd(drop table) failed");
	}
		
	free (cmdbuf);
	return retcode;
}

/*
** InstallNulls()
**
** Type of function:
** 	rpc program internal api
**
** Purpose:
** 	Install the character string "NULL" as the default null value
**	when bind to character columns.
**
** Parameters:
** 	context	- Pointer to CS_CONTEXT structure.
**
** Return:
**	CS_SUCCEED if null was installed.
**	Otherwise a Client-Library failure code.
*/

CS_STATIC CS_RETCODE 
InstallNulls(CS_CONTEXT *context)
{
	CS_DATAFMT	datafmt;

	memset(&datafmt, 0, sizeof (datafmt));
	datafmt.datatype = CS_CHAR_TYPE;
	datafmt.status = CS_FMT_UNUSED;
	datafmt.locale = NULL;
	
	return cs_setnull(context, &datafmt, "NULL", strlen("NULL"));
}

/*
** BuildRpcCommand()
**
** Type of function:
** 	rpc program internal api
**
** Purpose:
**	This routine contructs the parameters list for the rpc to execute
**
** Parameters:
** 	cmd	- Pointer to CS_COMMAND structure.
**
** Return:
**	CS_SUCCEED if rpc command was contructed
**	Otherwise a Client-Library failure code.
** 
*/

CS_STATIC CS_RETCODE 
BuildRpcCommand(CS_COMMAND *cmd)
{
	CS_CONNECTION	*connection;
	CS_CONTEXT	*context;
	CS_RETCODE	retcode;
	CS_DATAFMT	datafmt;
	CS_DATAFMT	srcfmt;
	CS_DATAFMT	destfmt;
	CS_INT		intvar;
	CS_SMALLINT	smallintvar;
	CS_FLOAT	floatvar;
	CS_MONEY	moneyvar;
	CS_BINARY	binaryvar;
	char 		moneystring[10];
	char		rpc_name[15];
	CS_INT		destlen;

	/*
	** Assign values to the variables used for parameter passing.
	*/	
	intvar = 2;
	smallintvar = 234;
	floatvar = 0.12;
	binaryvar = (CS_BINARY)0xff;
	strcpy(rpc_name, "sample_rpc");
	strcpy(moneystring, "300.90");

	/*
	** Clear and setup the CS_DATAFMT structures used to convert datatypes.
	*/
	memset(&srcfmt, 0, sizeof (CS_DATAFMT));
	srcfmt.datatype = CS_CHAR_TYPE;
	srcfmt.maxlength = strlen(moneystring);
	srcfmt.precision = 5;
	srcfmt.scale = 2;
	srcfmt.locale = NULL;

	memset(&destfmt, 0, sizeof (CS_DATAFMT));
	destfmt.datatype = CS_MONEY_TYPE;
	destfmt.maxlength = sizeof(CS_MONEY);
	destfmt.precision = 5;
	destfmt.scale = 2;
	destfmt.locale = NULL;

	/*
	** Convert the string representing the money value
	** to a CS_MONEY variable. Since this routine does not have the 
	** context handle, we use the property functions to get it.
	*/
	if ((retcode = ct_cmd_props(cmd, CS_GET, CS_PARENT_HANDLE,
				&connection, CS_UNUSED, NULL)) != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: ct_cmd_props() failed");
		return retcode;
	}
	if ((retcode = ct_con_props(connection, CS_GET, CS_PARENT_HANDLE,
				&context, CS_UNUSED, NULL)) != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: ct_con_props() failed");
		return retcode;
	}
	retcode = cs_convert(context, &srcfmt, (CS_VOID *)moneystring,
					&destfmt, &moneyvar, &destlen);
	if (retcode != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: cs_convert() failed");
		return retcode;
	}

	/*
	** Send the RPC command for our stored procedure.
	*/
	if ((retcode = ct_command(cmd, CS_RPC_CMD, rpc_name, CS_NULLTERM,
			CS_NO_RECOMPILE)) != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: ct_command() failed");
		return retcode;
	}

	/*
	** Clear and setup the CS_DATAFMT structure, then pass 
	** each of the parameters for the RPC.
	*/
	memset(&datafmt, 0, sizeof (datafmt));
	strcpy(datafmt.name, "@intparam");
	datafmt.namelen = CS_NULLTERM;
	datafmt.datatype = CS_INT_TYPE;
	datafmt.maxlength = CS_UNUSED;
	datafmt.status = CS_INPUTVALUE;
	datafmt.locale = NULL;
	
	if ((retcode = ct_param(cmd, &datafmt, (CS_VOID *)&intvar, 
			CS_SIZEOF(CS_INT), 0)) != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: ct_param(int) failed");
		return retcode;
	}

	strcpy(datafmt.name, "@sintparam");
	datafmt.namelen = CS_NULLTERM;
	datafmt.datatype = CS_SMALLINT_TYPE;
	datafmt.maxlength = EX_MAXSTRINGLEN;
	datafmt.status = CS_RETURN;
	datafmt.locale = NULL;
	
	if ((retcode = ct_param(cmd, &datafmt, (CS_VOID *)&smallintvar, 
			CS_SIZEOF(CS_SMALLINT), 0)) != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: ct_param(smallint) failed");
		return retcode;
	}

	strcpy(datafmt.name, "@floatparam");
	datafmt.namelen = CS_NULLTERM;
	datafmt.datatype = CS_FLOAT_TYPE;
	datafmt.maxlength = EX_MAXSTRINGLEN;
	datafmt.status = CS_RETURN;
	datafmt.locale = NULL;
	
	if ((retcode = ct_param(cmd, &datafmt, (CS_VOID *)&floatvar, 
			CS_SIZEOF(CS_FLOAT), 0))  != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: ct_param(float) failed");
		return retcode;
	}

	strcpy(datafmt.name, "@moneyparam");
	datafmt.namelen = CS_NULLTERM;
	datafmt.datatype = CS_MONEY_TYPE;
	datafmt.maxlength = EX_MAXSTRINGLEN;
	datafmt.status = CS_RETURN;
	datafmt.locale = NULL;
	
	if ((retcode = ct_param(cmd, &datafmt, (CS_VOID *)&moneyvar, 
			CS_SIZEOF(CS_MONEY), 0))  != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: ct_param(money) failed");
		return retcode;
	}

	strcpy(datafmt.name, "@dateparam");
	datafmt.namelen = CS_NULLTERM;
	datafmt.datatype = CS_DATETIME4_TYPE;
	datafmt.maxlength = EX_MAXSTRINGLEN;
	datafmt.status = CS_RETURN;
	datafmt.locale = NULL;
	
	/*
	** The datetime variable is filled in by the RPC so pass NULL for 
	** the data, 0 for data length, and -1 for the indicator arguments.
	*/
	if ((retcode = ct_param(cmd, &datafmt, NULL, 0, -1))  != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: ct_param(datetime4) failed");
		return retcode;
	}

	strcpy(datafmt.name, "@charparam");
	datafmt.namelen = CS_NULLTERM;
	datafmt.datatype = CS_CHAR_TYPE;
	datafmt.maxlength = EX_MAXSTRINGLEN;
	datafmt.status = CS_RETURN;
	datafmt.locale = NULL;
	
	/*
	** The character string variable is filled in by the RPC so pass NULL  
	** for the data 0 for data length, and -1 for the indicator arguments.
	*/
	if ((retcode = ct_param(cmd, &datafmt, NULL, 0, -1))  != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: ct_param(char) failed");
		return retcode;
	}

	strcpy(datafmt.name, "@binaryparam");
	datafmt.namelen = CS_NULLTERM;
	datafmt.datatype = CS_BINARY_TYPE;
	datafmt.maxlength = EX_MAXSTRINGLEN;
	datafmt.status = CS_RETURN;
	datafmt.locale = NULL;
	
	if ((retcode = ct_param(cmd, &datafmt, (CS_VOID *)&binaryvar, 
			CS_SIZEOF(CS_BINARY), 0))  != CS_SUCCEED)
	{
		ex_error("BuildRpcCommand: ct_param(binary) failed");
		return retcode;
	}

	return retcode;
}

/*
** DoRpc()
**
** Type of function:
** 	rpc program internal api
**
** Purpose:
**	This routine passes the parameters and runs 
**	the sample RPC on the server.
**
** Parameters:
** 	connection	- Pointer to CS_CONNECTION structure.
**
** Return:
**	CS_SUCCEED if rpc was executed.
**	Otherwise a Client-Library failure code.
** 
*/

CS_STATIC CS_RETCODE 
DoRpc(CS_CONNECTION *connection)
{
	CS_RETCODE	retcode;
	CS_COMMAND	*cmd;

	/*
	** Use the new database
	*/
	if ((retcode = ex_use_db(connection, Ex_dbname)) != CS_SUCCEED)
	{
		ex_error("DoRpc: ex_use_db() failed");
		return retcode;
	}

	if ((retcode = ct_cmd_alloc(connection, &cmd)) != CS_SUCCEED)
	{
		ex_error("DoRpc: ct_cmd_alloc() failed");
		return retcode;
	}

	if ((retcode = BuildRpcCommand(cmd)) != CS_SUCCEED)
	{
		ex_error("DoRpc: BuildRpcCommand() failed");
		return retcode;
	}

	/*
	** Send the command to the server 
	*/
	if (ct_send(cmd) != CS_SUCCEED)
	{
		ex_error("DoCompute: ct_send() failed");
		return retcode;
	}

	return ex_handle_results(cmd);
}
