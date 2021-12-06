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
** Define whether the compiler supports structure assignments.
*/
#define STRUCTASSIGN(d, s)	(d) = (s)

/*
** Various "operation" types used to identify how output is to appear.
** Must be less than 20 characters.
*/
#define OPERATION_NONE "none"
#define OPERATION_BEGIN_XACT "0"
#define OPERATION_INSERT "4"
#define OPERATION_AFTER_IMAGE "after image"  // Row AFTER image
#define OPERATION_TEXTINSERT "32"
#define OPERATION_TEXT_AFTER "text after"  // TEXT column AFTER image
#define OPERATION_COMMIT_XACT "30"
#define OPERATION_DELETE "5"
#define OPERATION_BEFORE_AND_AFTER_IMAGE "before & after image"
#define OPERATION_BEFORE_IMAGE "before image"
#define OPERATION_BT_INSERT "71"
#define OPERATION_BT_DELETE "72"

/*
** Various operation "status" types used to identify how output is to appear.
** Must be less than 20 characters.
*/
#define STATUS_NONE "none"
#define STATUS_UPDATE "4"

/*
** Global names used in this module
*/
CS_CHAR *Ex_appname = "logtransfer_example";
CS_CHAR	*Ex_dbname = "lobs";
CS_CHAR *Ex_server   = EX_SERVER;
CS_CHAR *Ex_username = EX_USERNAME;
CS_CHAR *Ex_password = EX_PASSWORD;

#define	GET_CS_CONTEXT	Cs_context
CS_CONTEXT		*Cs_context;

/*
** Prototypes for routines in the example code.
*/
CS_STATIC CS_RETCODE InstallNulls(CS_CONTEXT *context);
CS_STATIC CS_RETCODE DoLogtransfer(CS_CONNECTION *connection,
                                   CS_CHAR *operation,
                                   CS_CHAR *qualifier,
                                   CS_CHAR *parm);
CS_RETCODE CS_PUBLIC handle_logtransfer_scan_results(CS_COMMAND *cmd);
CS_STATIC CS_RETCODE DoDML(CS_CONNECTION *connection, CS_CHAR *dml);
CS_RETCODE CS_PUBLIC logtransfer_fetch_data(CS_COMMAND *cmd,
                                            CS_CHAR *operation,
                                            CS_CHAR *status);
CS_RETCODE CS_PUBLIC logtransfer_display_header(CS_INT numcols,
                                                CS_DATAFMT columns[],
                                                CS_CHAR *operation,
                                                CS_CHAR *status);
CS_RETCODE logtransfer_dt_fmt(CS_VOID *val, CS_CHAR *out_buf,
                              CS_INT bufSize, CS_INT date_type);

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
	CS_CONNECTION	*connection;
	CS_RETCODE	retcode;

	fprintf(stdout, "LOGTRANSFER Example\n");
	fflush(stdout);

	/* 
	** Allocate a Cs_context structure and initialize Client-Library
	*/
	retcode = ex_init(&GET_CS_CONTEXT);
	if (retcode != CS_SUCCEED)
	{
		ex_panic("ex_init failed");
	}

	/* 
	** Allocate a connection structure, set its properties, and  
	** establish a connection.
	*/
	retcode = ex_connect(GET_CS_CONTEXT, &connection, Ex_appname,
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
		retcode = InstallNulls(GET_CS_CONTEXT);
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
        /*
         * Let's see what we get by default.
         *
         * I'm not even sure that these are correct options, or, for
         * that matter, whether this is all of them.
         *
         * However, we seem to get a lot of information if we don't specifically
         * request anything (i.e., use the default), than if we try to
         * specify everything using the below technique.
         */
        /*
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "beginxact");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "endxact");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "insert");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "delete");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "insind");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "execend");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "checkpoint");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "syncdpdb");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "synclddb");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "syncldxact");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "cmd");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "cmdnoop");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "savexact");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "textinsert");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "inooptext");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "rowimage");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "trunctab");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "cmdtext");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "objinfo");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "colinfo");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "dol_insert");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "dol_delete");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "dol_insind");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "dol_update");
        ** retcode = DoLogtransfer(connection, "setqual", "logop", "clr");
        */
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
    ** Do a few APL updates.
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
    ** Do a few DOL updates.
    */
    if (retcode == CS_SUCCEED)
    {
        retcode = DoDML(connection, "insert into test_lob_dol (id, story) "
                                    "values(103, 'My hat is old. My teeth are gold.')");
    }
    if (retcode == CS_SUCCEED)
    {
        retcode = DoDML(connection, "update test_lob_dol set story='I have a bird I like to hold.' "
                                    "where id = 103");
    }
    if (retcode == CS_SUCCEED)
    {
        retcode = DoDML(connection, "delete from test_lob_dol "
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
    ** Advance truncation point.
    */
    if (retcode == CS_SUCCEED)
    {
        retcode = DoDML(connection, "dbcc gettrunc");
    }
    if (retcode == CS_SUCCEED)
    {
        retcode = DoDML(connection, "dbcc settrunc(ltm, ignore)");
    }
    if (retcode == CS_SUCCEED)
    {
        retcode = DoDML(connection, "dump tran lobs with no_log");
    }
    if (retcode == CS_SUCCEED)
    {
        retcode = DoDML(connection, "dbcc settrunc(ltm, valid)");
    }

	/*
	** Deallocate the allocated structures, close the connection,
	** and exit Client-Library.
	*/
	if (connection != NULL)
	{
		retcode = ex_con_cleanup(connection, retcode);
	}
	
	if (GET_CS_CONTEXT != NULL)
	{
		retcode = ex_ctx_cleanup(GET_CS_CONTEXT, retcode);
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

    if(strcasecmp(operation, "scan") != 0) {
        retcode = ex_handle_results(cmd);
    } else {
        retcode = handle_logtransfer_scan_results(cmd);
    }
    if (retcode != CS_SUCCEED) {
        CS_CHAR     tmpbuf[EX_MAXSTRINGLEN];

        sprintf(tmpbuf, "DoLogtransfer: handling results failed with operation=<%s>, qualifier=<%s>, parm=<%s>.",
                operation, qualifier, parm);
        ex_error(tmpbuf);
    }

    return retcode;
}


/*
** handle_logtransfer_scan_results
**
** logtransfer results processing.
*/
CS_RETCODE CS_PUBLIC
handle_logtransfer_scan_results(CS_COMMAND *cmd)
{
    CS_RETCODE retcode;
    CS_INT res_type;
    CS_SMALLINT msg_id;
    CS_CHAR operation[20];
    CS_CHAR status[20];

    /*
    ** Process the results.
    */
    strcpy(operation, OPERATION_NONE);
    strcpy(status, STATUS_NONE);
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
                retcode = logtransfer_fetch_data(cmd, &operation[0], &status[0]);
                if (retcode != CS_SUCCEED)
                {
                    ex_error("handle_logtransfer_scan_results: logtransfer_fetch_data() failed");
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
                    ex_error("handle_logtransfer_scan_results: ct_res_info(msgtype) failed");
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
                ex_error("handle_logtransfer_scan_results: ct_results returned CS_CMD_FAIL.");
                return CS_FAIL;
                break;

            default:
                /*
                ** We got something unexpected.
                */
                ex_error("handle_logtransfer_scan_results: ct_results returned unexpected result type");
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
            ex_error("handle_logtransfer_scan_results: ct_results() failed");
            break;

        default:
            /*
            ** We got an unexpected return value.
            */
            ex_error("handle_logtransfer_scan_results: ct_results returned unexpected result type");
            break;
    }

    return retcode;
}

/*
** logtransfer_fetch_data()
**
** Type of function:
** 	logtransfer results data processing
**
** Purpose:
** 	This function processes fetchable result sets. The results include:
**
**		CS_ROW_RESULT
**		CS_CURSOR_RESULT
**		CS_PARAM_RESULT
**		CS_STATUS_RESULT
**		CS_COMPUTE_RESULT
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
logtransfer_fetch_data(CS_COMMAND *cmd, CS_CHAR *operation, CS_CHAR *status)
{
    CS_RETCODE		retcode;
    CS_INT			num_cols;
    CS_INT			i;
    CS_INT			j;
    CS_INT			row_count = 0;
    CS_INT			rows_read;
    CS_INT			disp_len;
    CS_DATAFMT		*datafmt;
    CS_DATAFMT		*orig_datafmt;
    EX_COLUMN_DATA		*coldata;

    /*
    ** Find out how many columns there are in this result set.
    */
    retcode = ct_res_info(cmd, CS_NUMDATA, &num_cols, CS_UNUSED, NULL);
    if (retcode != CS_SUCCEED)
    {
        ex_error("logtransfer_fetch_data: ct_res_info() failed");
        return retcode;
    }

    /*
    ** Make sure we have at least one column
    */
    if (num_cols <= 0)
    {
        ex_error("logtransfer_fetch_data: ct_res_info() returned zero columns");
        return CS_FAIL;
    }

    /*
    ** 'coldata', is an array of
    ** EX_COLUMN_DATA structures. Each array element represents
    ** one column.  Each array element will re-used for each row.
    **
    ** First, allocate memory for the data element to process.
    */
    coldata = (EX_COLUMN_DATA *)malloc(num_cols * sizeof (EX_COLUMN_DATA));
    if (coldata == NULL)
    {
        ex_error("logtransfer_fetch_data: 1 malloc() failed");
        return CS_MEM_ERROR;
    }

    datafmt = (CS_DATAFMT *)malloc(num_cols * sizeof (CS_DATAFMT));
    if (datafmt == NULL)
    {
        ex_error("logtransfer_fetch_data: 2 malloc() failed");
        free(coldata);
        return CS_MEM_ERROR;
    }

    /*
    ** We may modify the datafmt structures in order to supply values for
    ** meaningful for the PoC. Capture the original values here for convenient
    ** access when needed.
    */
    orig_datafmt = (CS_DATAFMT *)malloc(num_cols * sizeof (CS_DATAFMT));
    if (orig_datafmt == NULL)
    {
        ex_error("logtransfer_fetch_data: 3 malloc() failed");
        free(coldata);
        free(datafmt);
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
            ex_error("logtransfer_fetch_data: ct_describe() failed");
            break;
        }

        STRUCTASSIGN(orig_datafmt[i], datafmt[i]);

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
        if((datafmt[i].datatype == CS_DATETIME_TYPE) ||
           (datafmt[i].datatype == CS_DATETIME4_TYPE) ||
           (datafmt[i].datatype == CS_TIME_TYPE) ||
           (datafmt[i].datatype == CS_BIGDATETIME_TYPE) ||
           (datafmt[i].datatype == CS_BIGTIME_TYPE)) {
            /*
            ** We'll do these ourselves.
            */
        }
        else {
            datafmt[i].datatype = CS_CHAR_TYPE;
            datafmt[i].format = CS_FMT_NULLTERM;
        }

        /*
        ** Allocate memory for the column string
        */
        coldata[i].value = (CS_CHAR *)malloc(datafmt[i].maxlength);
        if (coldata[i].value == NULL)
        {
            ex_error("logtransfer_fetch_data: malloc() failed");
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
            ex_error("logtransfer_fetch_data: ct_bind() failed");
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
        free(orig_datafmt);
        return retcode;
    }

    /*
    ** Fetch the first row since the column headers will depend upon the
    ** operation type.
    */
    retcode = ct_fetch(cmd, CS_UNUSED, CS_UNUSED, CS_UNUSED,
                       &rows_read);

    if((strcmp(coldata[0].value, OPERATION_BT_INSERT) == 0) ||
            (strcmp(coldata[0].value, OPERATION_BT_DELETE) == 0)) {
        fprintf(stdout, "Ignoring results for <%s>.\n",
                (strcmp(coldata[0].value, OPERATION_BT_INSERT) == 0) ? "BT_INSERT" : "BT_DELETE");
        fflush(stdout);
        while (((retcode = ct_fetch(cmd, CS_UNUSED, CS_UNUSED, CS_UNUSED,
                                    &rows_read)) == CS_SUCCEED) || (retcode == CS_ROW_FAIL))
        {
            /*
            ** Ignore some operations.
            */
        }
    }
    else {
        if((strcmp(operation, OPERATION_NONE) == 0) ||
           (strcmp(operation, OPERATION_BEGIN_XACT) == 0) ||
           (strcmp(operation, OPERATION_AFTER_IMAGE) == 0) ||
           (strcmp(operation, OPERATION_TEXT_AFTER) == 0) ||
           (strcmp(operation, OPERATION_COMMIT_XACT) == 0) ||
           (strcmp(operation, OPERATION_BEFORE_AND_AFTER_IMAGE) == 0) ||
           (strcmp(operation, OPERATION_BEFORE_IMAGE) == 0)) {
            strcpy(operation, coldata[0].value);
        } else if(strcmp(operation, OPERATION_INSERT) == 0) {
            strcpy(operation, OPERATION_AFTER_IMAGE);
        } else if(strcmp(operation, OPERATION_TEXTINSERT) == 0) {
            strcpy(operation, OPERATION_TEXT_AFTER);
        } else if(strcmp(operation, OPERATION_DELETE) == 0) {
            if(strcmp(status, STATUS_UPDATE) == 0) {
                strcpy(operation, OPERATION_BEFORE_AND_AFTER_IMAGE);
            } else {
                strcpy(operation, OPERATION_BEFORE_IMAGE);
            }
        }

        if((strcmp(operation, OPERATION_INSERT) == 0) ||
           (strcmp(operation, OPERATION_DELETE) == 0)) {
            strcpy(status, coldata[3].value);
        } else {
            strcpy(status, STATUS_NONE);
        }

        if((retcode == CS_SUCCEED) || (retcode == CS_ROW_FAIL)) {
            /*
            ** Display column header
            */
            logtransfer_display_header(num_cols, datafmt, operation, status);

            /*
            ** Fetch the rows.  Loop while ct_fetch() returns CS_SUCCEED or
            ** CS_ROW_FAIL
            */
            do {
                /*
                ** Increment our row count by the number of rows just fetched.
                */
                row_count = row_count + rows_read;

                /*
                ** Check if we hit a recoverable error.
                */
                if(retcode == CS_ROW_FAIL) {
                    fprintf(stdout, "Error on row %d.\n", row_count);
                    fflush(stdout);
                }

                /*
                ** We have a row.  Loop through the columns displaying the
                ** column values.
                */
                for(i = 0; i < num_cols; i++) {
                    /*
                    ** Display the column value
                    */
                    if((datafmt[i].datatype == CS_DATETIME_TYPE) ||
                       (datafmt[i].datatype == CS_DATETIME4_TYPE) ||
                       (datafmt[i].datatype == CS_TIME_TYPE) ||
                       (datafmt[i].datatype == CS_BIGDATETIME_TYPE) ||
                       (datafmt[i].datatype == CS_BIGTIME_TYPE)) {
                        CS_CHAR out_buf[30];

                        retcode = logtransfer_dt_fmt(coldata[i].value,
                                                     &out_buf[0], 32,
                                                     datafmt[i].datatype);
                        if(retcode == CS_SUCCEED) {
                            fprintf(stdout, "%s", out_buf);
                        }
                    } else {
                        fprintf(stdout, "%s", coldata[i].value);
                    }
                    fflush(stdout);

                    /*
                    ** If not last column, Print out spaces between this
                    ** column and next one.
                    */
                    if(i != num_cols - 1) {
                        disp_len = ex_display_dlen(&datafmt[i]);
                        disp_len -= coldata[i].valuelen - 1;
                        for(j = 0; j < disp_len; j++) {
                            fputc(' ', stdout);
                        }
                    }
                }
                fprintf(stdout, "\n");
                fflush(stdout);
            } while(((retcode = ct_fetch(cmd, CS_UNUSED, CS_UNUSED, CS_UNUSED,
                                         &rows_read)) == CS_SUCCEED) ||
                    (retcode == CS_ROW_FAIL));
        } else {
            ex_display_header(num_cols, datafmt);
        }
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
    free(orig_datafmt);

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
            ex_error("logtransfer_fetch_data: ct_fetch() failed");
            break;

        default:
            /*
            ** We got an unexpected return value.
            */
            ex_error("logtransfer_fetch_data: ct_fetch() returned an expected retcode");
            break;

    }
    return retcode;
}

/*
** logtransfer_display_header()
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
logtransfer_display_header(CS_INT numcols, CS_DATAFMT columns[], CS_CHAR* operation, CS_CHAR *status)
{
    CS_INT		i;
    CS_INT		l;
    CS_INT		j;
    CS_INT		disp_len;

    /*
    ** Row preamble.
    */
    fputc('\n', stdout);
    if(strcmp(operation, OPERATION_BEGIN_XACT) == 0) {
        fprintf(stdout, "BEGIN XACT");
    }
    else if(strcmp(operation, OPERATION_INSERT) == 0) {
        if(strcmp(status, STATUS_UPDATE) == 0) {
            fprintf(stdout, "UPDATE");
        }
        else {
            fprintf(stdout, "INSERT");
        }
    }
    else if(strcmp(operation, OPERATION_AFTER_IMAGE) == 0) {
        fprintf(stdout, "AFTER IMAGE");
    }
    else if(strcmp(operation, OPERATION_TEXTINSERT) == 0) {
        fprintf(stdout, "TEXTINSERT");
    }
    else if(strcmp(operation, OPERATION_TEXT_AFTER) == 0) {
        fprintf(stdout, "Text column AFTER image");
    }
    else if(strcmp(operation, OPERATION_COMMIT_XACT) == 0) {
        fprintf(stdout, "COMMIT XACT");
    }
    else if(strcmp(operation, OPERATION_DELETE) == 0) {
        if(strcmp(status, STATUS_UPDATE) == 0) {
            fprintf(stdout, "UPDATE");
        }
        else {
            fprintf(stdout, "DELETE");
        }
    }
    else if(strcmp(operation, OPERATION_BEFORE_AND_AFTER_IMAGE) == 0) {
        fprintf(stdout, "BEFORE & AFTER images");
    }
    else if(strcmp(operation, OPERATION_BEFORE_IMAGE) == 0) {
        fprintf(stdout, "BEFORE IMAGE");
    }
    fflush(stdout);

    fputc('\n', stdout);
    for (i = 0; i < numcols; i++)
    {
        switch (i)
        {
            case 0:
            {
                if((strcmp(operation, OPERATION_AFTER_IMAGE) != 0) &&
                    (strcmp(operation, OPERATION_TEXT_AFTER) != 0) &&
                    (strcmp(operation, OPERATION_BEFORE_AND_AFTER_IMAGE) != 0) &&
                    (strcmp(operation, OPERATION_BEFORE_IMAGE) != 0)) {
                    strncpy(columns[i].name, "operation",
                            sizeof(columns[i].name) - 1);
                }
                break;
            }
            case 1:
            {
                if((strcmp(operation, OPERATION_AFTER_IMAGE) != 0) &&
                   (strcmp(operation, OPERATION_TEXT_AFTER) != 0) &&
                   (strcmp(operation, OPERATION_BEFORE_AND_AFTER_IMAGE) != 0) &&
                   (strcmp(operation, OPERATION_BEFORE_IMAGE) != 0)) {
                    strncpy(columns[i].name, "sessionid page", sizeof(columns[i].name)-1);
                }
                break;
            }
            case 2:
            {
                if((strcmp(operation, OPERATION_AFTER_IMAGE) != 0) &&
                   (strcmp(operation, OPERATION_TEXT_AFTER) != 0) &&
                   (strcmp(operation, OPERATION_BEFORE_AND_AFTER_IMAGE) != 0) &&
                   (strcmp(operation, OPERATION_BEFORE_IMAGE) != 0)) {
                    strncpy(columns[i].name, "sessionid record", sizeof(columns[i].name)-1);
                }
                break;
            }
            case 3:
            {
                if(strcmp(operation, OPERATION_BEGIN_XACT) == 0) {
                    strncpy(columns[i].name, "begin time",
                            sizeof(columns[i].name) - 1);
                }
                else if((strcmp(operation, OPERATION_INSERT) == 0) ||
                        (strcmp(operation, OPERATION_DELETE) == 0)) {
                    strncpy(columns[i].name, "status",
                            sizeof(columns[i].name) - 1);
                }
                else if(strcmp(operation, OPERATION_TEXTINSERT) == 0) {
                    strncpy(columns[i].name, "log page",
                            sizeof(columns[i].name) - 1);
                }

                break;
            }
            case 4:
            {
                if((strcmp(operation, OPERATION_INSERT) == 0) ||
                   (strcmp(operation, OPERATION_DELETE) == 0) ||
                   (strcmp(operation, OPERATION_COMMIT_XACT) == 0)) {
                    strncpy(columns[i].name, "log page",
                            sizeof(columns[i].name) - 1);
                }
                else if(strcmp(operation, OPERATION_TEXTINSERT) == 0) {
                    strncpy(columns[i].name, "log record",
                            sizeof(columns[i].name) - 1);
                }

                break;
            }
            case 5:
            {
                if((strcmp(operation, OPERATION_INSERT) == 0) ||
                   (strcmp(operation, OPERATION_DELETE) == 0) ||
                   (strcmp(operation, OPERATION_COMMIT_XACT) == 0)) {
                    strncpy(columns[i].name, "log record",
                            sizeof(columns[i].name) - 1);
                }
                break;
            }
            case 6:
            {
                if(strcmp(operation, OPERATION_BEGIN_XACT) == 0) {
                    strncpy(columns[i].name, "transaction name",
                            sizeof(columns[i].name) - 1);
                }
                break;
            }
            case 8:
            {
                if((strcmp(operation, OPERATION_INSERT) == 0) ||
                   (strcmp(operation, OPERATION_DELETE) == 0)) {
                    strncpy(columns[i].name, "table name",
                            sizeof(columns[i].name) - 1);
                }
                else if(strcmp(operation, OPERATION_TEXTINSERT) == 0) {
                    strncpy(columns[i].name, "column name",
                            sizeof(columns[i].name) - 1);
                }
                break;
            }
            case 9:
            {
                if((strcmp(operation, OPERATION_INSERT) == 0) ||
                   (strcmp(operation, OPERATION_DELETE) == 0)) {
                    strncpy(columns[i].name, "table owner",
                            sizeof(columns[i].name) - 1);
                }
                break;
            }
        }
        columns[i].name[sizeof(columns[i].name) - 1] = 0x00;

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
** logtransfer_dt_fmt
**
** Purpose:
**      Convert a temporal value to character string including all components.
**
** Parameters:
**      CS_VOID *val (input)
**              The value to convert.
**      CS_CHAR *out_buf (input/output)
**              The ascii format is put here.
**      CS_INT bufSize (input)
**              The size of the output buffer
**      CS_INT	date_type (input)
**		        Should be one of CS_DATETIME_TYPE, CS_DATETIME4_TYPE,
**		        CS_DATE_TYPE, CS_TIME_TYPE, CS_BIGDATETIME_TYPE or
**		        CS_BIGTIME_TYPE.
*/
CS_RETCODE
logtransfer_dt_fmt(CS_VOID *val, CS_CHAR *out_buf, CS_INT bufSize, CS_INT date_type)
{
    CS_RETCODE retcode;
    CS_DATEREC	cracked; /* cracked date */

    if ((retcode = cs_dt_crack(GET_CS_CONTEXT, date_type, val, &cracked))
        !=CS_SUCCEED)
    {
        ex_error("logtransfer_fetch_data: ct_fetch() returned an expected retcode");
        return retcode;
    }

    if ((date_type == CS_DATETIME_TYPE) || (date_type == CS_DATETIME4_TYPE))
    {
        snprintf(out_buf, bufSize, "%.4d%.2d%.2d %.2d:%.2d:%.2d:%.3d",
                 cracked.dateyear, cracked.datemonth+1,
                 cracked.datedmonth, cracked.datehour,
                 cracked.dateminute, cracked.datesecond,
                 cracked.datemsecond);
    }
    else if (date_type == CS_DATE_TYPE)
    {
        snprintf(out_buf, bufSize, "%.4d%.2d%.2d",
                 cracked.dateyear, cracked.datemonth+1,
                 cracked.datedmonth);
    }
    else if (date_type == CS_TIME_TYPE)
    {
        snprintf(out_buf, bufSize, "%.2d:%.2d:%.2d:%.3d",
                 cracked.datehour, cracked.dateminute,
                 cracked.datesecond, cracked.datemsecond);
    }
    else if (date_type == CS_BIGDATETIME_TYPE)
    {
        snprintf(out_buf, bufSize, "%.4d%.2d%.2d %.2d:%.2d:%.2d.%.6d",
                 cracked.dateyear, cracked.datemonth+1,
                 cracked.datedmonth, cracked.datehour,
                 cracked.dateminute, cracked.datesecond,
                 cracked.datesecfrac);
    }
    else if (date_type == CS_BIGTIME_TYPE)
    {
        snprintf(out_buf, bufSize, "%.2d:%.2d:%.2d.%.6d",
                 cracked.datehour, cracked.dateminute,
                 cracked.datesecond, cracked.datesecfrac);
    }

    return CS_SUCCEED;
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
DoDML(CS_CONNECTION *connection, CS_CHAR *dml)
{
    CS_RETCODE	retcode;
    CS_COMMAND	*cmd;

    if ((retcode = ct_cmd_alloc(connection, &cmd)) != CS_SUCCEED)
    {
        ex_error("DoDML: ct_cmd_alloc() failed");
        return retcode;
    }

    ex_msg("Attempting command:");
    ex_msg(dml);

    /*
    ** Build the command for our `SELECT` execution.
    */
    if ((retcode = ct_command(cmd, CS_LANG_CMD, dml, CS_NULLTERM,
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
                dml);
        ex_error(tmpbuf);
    }

    return retcode;
}
