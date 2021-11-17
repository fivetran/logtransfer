/*
** Description
** -----------
**	This example program demonstrates using the `dbcc logtransfer()`
**	protocol to scan ASE transaction log records.
**
**	The example uses standard ANSI C for output and
**	memory management.
**
**	All work is performed synchronously.
**
** Input
** -----
**
** Output
** ------
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
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
CS_CHAR *Ex_appname = "logtransfer_example";
CS_CHAR	*Ex_dbname = "lobs";
CS_CHAR *Ex_server   = EX_SERVER;
CS_CHAR *Ex_username = EX_USERNAME;
CS_CHAR *Ex_password = EX_PASSWORD;

/*
** Prototypes for routines in the example code.
*/
CS_STATIC CS_RETCODE InstallNulls(CS_CONTEXT *context);
CS_STATIC CS_RETCODE DoLogtransfer(CS_CONNECTION *connection, CS_CHAR *operation, CS_CHAR *qualifier, CS_CHAR *parm);
CS_STATIC CS_RETCODE DoDML(CS_CONNECTION *connection, CS_CHAR *select);

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

	fprintf(stdout, "LOGTRANSFER Example\n");
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
    ** Switch to the database whose tran log is to be scanned
    */
    if (retcode == CS_SUCCEED) {
        retcode = ex_use_db(connection, Ex_dbname);
    }

	/*
	** Install our null values to display
	*/
	if (retcode == CS_SUCCEED)
	{
		retcode = InstallNulls(context);
	}

	/*
	** Reserve log transfer context
	*/
	if (retcode == CS_SUCCEED)
	{
		retcode = DoLogtransfer(connection, "reserve", "context", "770");
	}

    /*
    ** Identify log records of interest
    */
    if (retcode == CS_SUCCEED)
    {
        retcode = DoLogtransfer(connection, "setqual", "logop",
            "beginxact, endxact, insert, delete, insind \
            execbegin, execend, checkpoint, syncdpdb, synclddb, syncldxact, cmd, \
            cmdnoop, savexact, textinsert, inooptext, rowimage, trunctab, cmdtext, \
            objinfo, colinfo, dol_insert, dol_delete, dol_insind, dol_update, clr");
    }

    /*
    ** Set maximum number of records to scan.
    */
    if (retcode == CS_SUCCEED)
    {
        retcode = DoLogtransfer(connection, "setqual", "numrecs", "1000");
    }

    /*
    ** Set scan mode.
    */
    if (retcode == CS_SUCCEED)
    {
        retcode = DoLogtransfer(connection, "setqual", "mode", "poll");
    }

    /*
    ** Set maximum wait time (in seconds) before returning from scan.
    */
    if (retcode == CS_SUCCEED)
    {
        retcode = DoLogtransfer(connection, "setqual", "timeout", "15");
    }

    /*
    ** Perform initial scan.
    */
    if (retcode == CS_SUCCEED)
    {
        retcode = DoLogtransfer(connection, "scan", "normal", "");
    }

    /*
    ** Do a few updates.
    */
    if (retcode == CS_SUCCEED)
    {
        retcode = DoDML(connection, "insert into test_lob (id, story) "
                                    "values(103, 'My hat is old. My teeth are gold.')");
    }
    if (retcode == CS_SUCCEED)
    {
        retcode = DoDML(connection, "update test_lob set story='I have a bird I like to hold.' "
                                       "where id = 103");
    }
    if (retcode == CS_SUCCEED)
    {
        retcode = DoDML(connection, "delete from test_lob "
                                        "where id = 103");
    }

    /*
    ** Perform 1 continuation scan.
    */
    if (retcode == CS_SUCCEED)
    {
        retcode = DoLogtransfer(connection, "scan", "continue", "");
    }

    /*
    ** Release log transfer context
    */
    if (retcode == CS_SUCCEED)
    {
        retcode = DoLogtransfer(connection, "release", "context", "");
    }

    /*
    ** `dbcc logtransfer()` output apparently needs to be augmented with other
    ** data which I do not no how to obtain. So just for "grins", let's see what
    ** we can get.
    **
    ** (Note: It could be that having established the logtransfer context, we may
    ** be protected from buffer corruption in our use of `dbcc page()`. We should
    ** test and/or ask.)
    */
    //if (retcode == CS_SUCCEED)
    //{
    //    retcode = DoDML(connection, "select * from lobs.dbo.syslogs");
    //}

    //if (retcode == CS_SUCCEED)
    //{
    //    retcode = DoDML(connection, "select * from master.dbo.syslogsdetail");
    //}

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
** BuildLogTransferCommand()
**
** Type of function:
** 	logtransfer dbcc option internal api
**
** Purpose:
**	This routine constructs the parameter list for the `dbcc logtransfer` to execute
**
** Parameters:
** 	cmd	- Pointer to CS_COMMAND structure.
**
** Return:
**	CS_SUCCEED if logtransfer command was constructed
**	Otherwise a Client-Library failure code.
** 
*/

CS_STATIC CS_RETCODE 
BuildLogTransferCommand(CS_COMMAND *cmd, CS_CHAR *operation, CS_CHAR *qualifier, CS_CHAR *parm)
{
	CS_RETCODE	retcode;
    CS_CHAR     tmpbuf[EX_MAXSTRINGLEN];

    if (!strcasecmp(operation, "reserve")) {
        sprintf(tmpbuf, "dbcc logtransfer('reserve', '%s', %s)",
                qualifier, parm);
    } else if (!strcasecmp(operation, "setqual")) {
        if (!strcasecmp(qualifier, "numrecs") || !strcasecmp(qualifier, "timeout")) {
            sprintf(tmpbuf, "dbcc logtransfer('setqual', '%s', %s)",
                    qualifier, parm);
        } else {
            sprintf(tmpbuf, "dbcc logtransfer('setqual', '%s', '%s')",
                    qualifier, parm);
        }
    } else if (!strcasecmp(operation, "scan")) {
        sprintf(tmpbuf, "dbcc logtransfer('scan', '%s')",
                qualifier);
    } else if (!strcasecmp(operation, "release")) {
        sprintf(tmpbuf, "dbcc logtransfer('release', '%s')",
                qualifier);
    } else {
        sprintf(tmpbuf, "BuildLogTransferCommand: Unknown operation <%s>.",
                operation);
        ex_error(tmpbuf);
        return -1;
    }

    ex_msg("Attempting command:");
    ex_msg(tmpbuf);

    /*
    ** Build the command for our `dbcc logtransfer` execution.
    */
	if ((retcode = ct_command(cmd, CS_LANG_CMD, tmpbuf, CS_NULLTERM,
                              CS_UNUSED)) != CS_SUCCEED)
	{
		ex_error("BuildLogTransferCommand: ct_command() failed");
		return retcode;
	}

	return retcode;
}

/*
** DoLogtransfer()
**
** Type of function:
** 	logtransfer program internal api
**
** Purpose:
**	This routine passes the parameters to and runs
**	`dbcc logtransfer` on the server.
**
** Parameters:
** 	connection	- Pointer to CS_CONNECTION structure.
**
** Return:
**	CS_SUCCEED if logtransfer was executed.
**	Otherwise, a Client-Library failure code.
** 
*/

CS_STATIC CS_RETCODE 
DoLogtransfer(CS_CONNECTION *connection, CS_CHAR *operation, CS_CHAR *qualifier, CS_CHAR *parm)
{
	CS_RETCODE	retcode;
	CS_COMMAND	*cmd;

	if ((retcode = ct_cmd_alloc(connection, &cmd)) != CS_SUCCEED)
	{
		ex_error("DoLogtransfer: ct_cmd_alloc() failed");
		return retcode;
	}

	if ((retcode = BuildLogTransferCommand(cmd, operation, qualifier, parm)) != CS_SUCCEED)
	{
		ex_error("DoLogtransfer: BuildLogTransferCommand() failed");
		return retcode;
	}

	/*
	** Send the command to the server 
	*/
	if (ct_send(cmd) != CS_SUCCEED)
	{
		ex_error("DoLogtransfer: ct_send() failed");
		return retcode;
	}

	retcode = ex_handle_results(cmd);
    if (retcode != CS_SUCCEED) {
        CS_CHAR     tmpbuf[EX_MAXSTRINGLEN];

        sprintf(tmpbuf, "DoLogtransfer: Failed with operation=<%s>, qualifier=<%s>, parm=<%s>.",
                operation, qualifier, parm);
        ex_error(tmpbuf);
    }

    return retcode;
}

/*
** DoDML()
**
** Type of function:
** 	logtransfer program internal api
**
** Purpose:
**	This routine expects to receive a SELECT/DML statement which it executes,
**  displaying the results.
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
DoDML(CS_CONNECTION *connection, CS_CHAR *select)
{
    CS_RETCODE	retcode;
    CS_COMMAND	*cmd;

    if ((retcode = ct_cmd_alloc(connection, &cmd)) != CS_SUCCEED)
    {
        ex_error("DoDML: ct_cmd_alloc() failed");
        return retcode;
    }

    ex_msg("Attempting command:");
    ex_msg(select);

    /*
    ** Build the command for our `SELECT` execution.
    */
    if ((retcode = ct_command(cmd, CS_LANG_CMD, select, CS_NULLTERM,
                              CS_UNUSED)) != CS_SUCCEED)
    {
        ex_error("DoDML: ct_command() failed");
        return retcode;
    }

    /*
    ** Send the command to the server
    */
    if (ct_send(cmd) != CS_SUCCEED)
    {
        ex_error("DoDML: ct_send() failed");
        return retcode;
    }

    retcode = ex_handle_results(cmd);
    if (retcode != CS_SUCCEED) {
        CS_CHAR     tmpbuf[EX_MAXSTRINGLEN];

        sprintf(tmpbuf, "DoDML: Failed with command=<%s>.",
                select);
        ex_error(tmpbuf);
    }

    return retcode;
}
