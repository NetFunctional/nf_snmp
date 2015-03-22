#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <net-snmp/mib_api.h>

/* Taken from net-snmp demo/example application, modified for use in Appcelerator Titanium */


/* change the word "define" to "undef" to try the (insecure) SNMPv1 version */
#undef DEMO_USE_SNMP_VERSION_3

#ifdef DEMO_USE_SNMP_VERSION_3
const char *our_v3_passphrase = "your pass phrase";
#endif

char errorText[500];
char *successText;
int maxVariableSize;

char *get_string_from_asn_type(netsnmp_variable_list * var)
{
    char *sp;
    
    switch (var->type) {
        case ASN_INTEGER:
        case ASN_COUNTER:
        case ASN_GAUGE:
        case ASN_TIMETICKS:
        {
            sp = (char *)malloc(50 * sizeof(char));
            sprintf(sp, "%ld", *(var->val.integer));
        }
            break;
            
        case ASN_PRIV_IMPLIED_OBJECT_ID:
        case ASN_OBJECT_ID:
            sp = (char *)malloc(1 + var->val_len);
            memcpy(sp, var->val.objid, var->val_len);
            sp[var->val_len] = '\0';
            break;
            
        case ASN_PRIV_IMPLIED_OCTET_STR:
        case ASN_OPAQUE:
        case ASN_OCTET_STR:
            sp = (char *)malloc(1 + var->val_len);
            memcpy(sp, var->val.string, var->val_len);
            sp[var->val_len] = '\0';
            break;
            
        default:
            sprintf(errorText, "Invalid asn type: %d\n", var->type);
    }
    
    return sp;
}

void snmp_mod_add_mibdir(const char *dirname)
{
    //snmp_log(LOG_DEBUG, "snmp - adding mib dirname: %s\n", dirname);
    
    netsnmp_set_mib_directory(dirname);
    
    //snmp_log(LOG_DEBUG, "mib directory (updated) contains: %s\n", netsnmp_get_mib_directory());
}

netsnmp_session *snmp_mod_open(const char *host, const char *community, const char *version, int timeout)
{
    maxVariableSize = 1024;
    
    //snmp_set_mib_warnings(2);
    
    //snmp_log(LOG_DEBUG, "timeout at open: %d", timeout);
    
    netsnmp_session session, *ss;
    
    /*
     * Initialize the SNMP library
     */
    init_snmp("snmpdemoapp");
    
    /*
     * Initialize a "session" that defines who we're going to talk to
     */
    snmp_sess_init( &session );                   /* set up defaults */
    session.peername = strdup(host);
    
    if (timeout > 0)
    {
        session.timeout = timeout;
    }
    
    /* If they asked for version 2c, then use it, otherwise default to v1 */
    
    if(strcmp(version,"SNMP_VERSION_2c") != 0)
    {
        /* Asked for v2c by name */
        session.version = SNMP_VERSION_2c;
    } else
    {
        session.version = SNMP_VERSION_1;
    }

    
    /* set the SNMP version number */
    session.version = SNMP_VERSION_2c;
    
    /* set the SNMPv1 community name used for authentication */
    session.community = (unsigned char *)community;
    session.community_len = strlen(community);
    
    
    /*
     * Open the session
     */
    SOCK_STARTUP;
    ss = snmp_open(&session);                     /* establish the session */
    
    if (!ss) {
        snmp_sess_perror("ack", &session);
        SOCK_CLEANUP;
        exit(1);
    }
    
    return ss;
}

void snmp_mod_close(netsnmp_session *ss)
{
    snmp_close(ss);
    SOCK_CLEANUP;
}





const char *snmp_mod_get(netsnmp_session *ss, const char *mib)
{
    //snmp_log(LOG_DEBUG, "start of snmp_mod_get\n");
    
    netsnmp_pdu *pdu;
    netsnmp_pdu *response;
    
    oid anOID[MAX_OID_LEN];
    size_t anOID_len;
    
    netsnmp_variable_list *vars;
    char *sp = NULL;
    int status;
    
    /*
     * Create the PDU for the data for our request.
     *   1) We're going to GET the system.sysDescr.0 node.
     */
    pdu = snmp_pdu_create(SNMP_MSG_GET);
    anOID_len = MAX_OID_LEN;
    //read_objid(".1.3.6.1.2.1.1.1.0", anOID, &anOID_len);
    //read_objid("system.sysUpTime.0", anOID, &anOID_len);
    //get_node(mib, anOID, &anOID_len);
    snmp_parse_oid(mib, anOID, &anOID_len);
    /*if (!snmp_parse_oid(".1.3.6.1.2.1.1.1.0", anOID, &anOID_len)) {
     snmp_perror(".1.3.6.1.2.1.1.1.0");
     SOCK_CLEANUP;
     exit(1);
     }*/
#if OTHER_METHODS
    /*
     *  These are alternatives to the 'snmp_parse_oid' call above,
     *    e.g. specifying the OID by name rather than numerically.
     */
    read_objid(".1.3.6.1.2.1.1.1.0", anOID, &anOID_len);
    get_node("sysDescr.0", anOID, &anOID_len);
    read_objid("system.sysDescr.0", anOID, &anOID_len);
#endif
    
    snmp_add_null_var(pdu, anOID, anOID_len);
    
    /*
     * Send the Request out.
     */
    status = snmp_synch_response(ss, pdu, &response);
    //snmp_log(LOG_DEBUG, "status: %d\n", status);
    /*
     * Process the response.
     */
    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
        /*
         * SUCCESS: Print the result variables
         */
        
        /* manipuate the information ourselves */
        for(vars = response->variables; vars; vars = vars->next_variable) {
            //sp = get_string_from_asn_type(vars);
            char *mib_string = (char *)malloc(maxVariableSize);
            int snStatus = snprint_variable(mib_string, maxVariableSize, vars->name, vars->name_length, vars);
            
            if (snStatus)
            {
                sp = mib_string;
            }
            
            //snmp_log(LOG_DEBUG, "value # is a string: %s\n", sp);
        }
    } else {
        /*
         * FAILURE: print what went wrong!
         */
        
        if (status == STAT_SUCCESS)
        {
            sprintf(errorText, "%s", snmp_errstring(response->errstat));
        }
        else if (status == STAT_TIMEOUT)
        {
            sprintf(errorText, "Timeout: No response from %s.\n",
                    ss->peername);
        }
        else
        {
            sprintf(errorText, "Unknown error has occured");
            snmp_sess_perror("Unknown error has occured [DETAILED]: ", ss);
        }
        
    }
    
    /*
     * Clean up:
     *  1) free the response.
     *  2) close the session.
     */
    if (response)
        snmp_free_pdu(response);
    
    return sp;
}

const char *snmp_mod_getnext(netsnmp_session *ss, const char *mib)
{
//    snmp_log(LOG_DEBUG, "start of snmp_mod_getnext\n");
    
    netsnmp_pdu *pdu;
    netsnmp_pdu *response;
    
    oid anOID[MAX_OID_LEN];
    size_t anOID_len;
    
    netsnmp_variable_list *vars;
    char *sp = NULL;
    int status;
    
    /*
     * Create the PDU for the data for our request.
     *   
     */
    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
    anOID_len = MAX_OID_LEN;
    snmp_parse_oid(mib, anOID, &anOID_len);
    snmp_add_null_var(pdu, anOID, anOID_len);
    
    /*
     * Send the Request out.
     */
    status = snmp_synch_response(ss, pdu, &response);
  //  snmp_log(LOG_DEBUG, "snmp_mod_getnext status: %d\n", status);
    /*
     * Process the response.
     */
    if (status == STAT_SUCCESS && response->errstat == SNMP_ERR_NOERROR) {
        /*
         * SUCCESS: Print the result variables
         */
        
        /* manipuate the information ourselves */
        for(vars = response->variables; vars; vars = vars->next_variable) {
            //sp = get_string_from_asn_type(vars);
            char *mib_string = (char *)malloc(maxVariableSize);
            int snStatus = snprint_variable(mib_string, maxVariableSize, vars->name, vars->name_length, vars);
            
            if (snStatus)
            {
                sp = mib_string;
            }
            
            //snmp_log(LOG_DEBUG, "value # is a string: %s\n", sp);
        }
    } else {
        /*
         * FAILURE: print what went wrong!
         */
        
        if (status == STAT_SUCCESS)
        {
            sprintf(errorText, "%s", snmp_errstring(response->errstat));
        }
        else if (status == STAT_TIMEOUT)
        {
            sprintf(errorText, "Timeout: No response from %s.\n",
                    ss->peername);
        }
        else
        {
            sprintf(errorText, "Unknown error has occured");
            snmp_sess_perror("Unknown error has occured [DETAILED]: ", ss);
        }
        
    }
    
    /*
     * Clean up:
     *  1) free the response.
     *  2) close the session.
     */
    if (response)
        snmp_free_pdu(response);
    
    return sp;
}


char *snmp_mod_set(netsnmp_session *ss, const char *mib, const char value_type, const char *value)
{
    netsnmp_session session;
    netsnmp_pdu    *pdu, *response = NULL;
    netsnmp_variable_list *vars;
    int             count;
    int             current_name = 0;
    char           *names[SNMP_MAX_CMDLINE_OIDS];
    char            types[SNMP_MAX_CMDLINE_OIDS];
    char           *values[SNMP_MAX_CMDLINE_OIDS];
    oid             name[MAX_OID_LEN];
    size_t          name_length;
    int             status;
    int             failures = 0;
    int             exitval = 0;
    int             quiet = 0;
    
    char *sp = NULL;
    
    current_name = 1;
    names[0] = strdup(mib);
    types[0] = value_type;
    values[0] = strdup(value);
    
    /*
     * create PDU for SET request and add object names and values to request 
     */
    pdu = snmp_pdu_create(SNMP_MSG_SET);
    for (count = 0; count < current_name; count++) {
        name_length = MAX_OID_LEN;
        if (snmp_parse_oid(names[count], name, &name_length) == NULL) {
            sprintf(errorText, "Could not parse specified oid.");
            snmp_perror(names[count]);
            failures++;
        } else
            if (snmp_add_var
                (pdu, name, name_length, types[count], values[count])) {
                sprintf(errorText, "Could not set specified oid.");
                snmp_perror(names[count]);
                failures++;
            }
    }
    
    if (failures) {
        //snmp_close(ss);
        //SOCK_CLEANUP;
        //exit(1);
        return sp;
    }
    
    /*
     * do the request 
     */
    status = snmp_synch_response(ss, pdu, &response);
    if (status == STAT_SUCCESS) {
        if (response->errstat == SNMP_ERR_NOERROR) {
            if (!quiet) {
                for (vars = response->variables; vars;
                     vars = vars->next_variable)
                {
                    //print_variable(vars->name, vars->name_length, vars);
                    
                    char *mib_string = (char *)malloc(maxVariableSize);
                    int snStatus = snprint_variable(mib_string, maxVariableSize, vars->name, vars->name_length, vars);
                    
                    if (snStatus)
                    {
                        sp = mib_string;
                    }
                }
            }
        } else {
            sprintf(errorText, "Error in packet.\nReason: %s\n",
                    snmp_errstring(response->errstat));
            if (response->errindex != 0) {
                /*fprintf(stderr, "Failed object: ");
                 for (count = 1, vars = response->variables;
                 vars && (count != response->errindex);
                 vars = vars->next_variable, count++);
                 if (vars)
                 fprint_objid(stderr, vars->name, vars->name_length);
                 fprintf(stderr, "\n");*/
            }
            exitval = 2;
        }
    } else if (status == STAT_TIMEOUT) {
        sprintf(errorText, "Timeout: No Response from %s\n",
                session.peername);
        
        exitval = 1;
    } else {                    /* status == STAT_ERROR */
        sprintf(errorText, "Unknown error has occured");
        snmp_sess_perror("snmpset", ss);
        exitval = 1;
    }
    
    if (response)
        snmp_free_pdu(response);
    
    return sp;
}

char **snmp_mod_walk(netsnmp_session *ss, const char *baseOid)
{
    netsnmp_pdu    *pdu, *response;
    netsnmp_variable_list *vars;
    oid             name[MAX_OID_LEN];
    size_t          name_length;
    oid             root[MAX_OID_LEN];
    size_t          rootlen;
    int             count;
    int             running;
    int             status;
    int             check;
    int             exitval = 0;
    int             numprinted = 0;
    
    /*
     * get the initial object and subtree 
     */
    
    rootlen = MAX_OID_LEN;
    if (snmp_parse_oid(baseOid, root, &rootlen) == NULL) {
        snmp_perror(baseOid);
        //exit(1);
    }
    
    /*
     * get first object to start walk 
     */
    memmove(name, root, rootlen * sizeof(oid));
    name_length = rootlen;
    
    running = 1;
    //static char *mib_strings[1024];
    char **mib_strings = (char **)malloc(1024 * sizeof(char *));
    int i=0;
    
    while (running) {
        /*
         * create PDU for GETNEXT request and add object name to request 
         */
        pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
        snmp_add_null_var(pdu, name, name_length);
        
        /*
         * do the request 
         */
        status = snmp_synch_response(ss, pdu, &response);
        if (status == STAT_SUCCESS) {
            if (response->errstat == SNMP_ERR_NOERROR) {
                /*
                 * check resulting variables 
                 */
                //snmp_log(LOG_DEBUG, "is a success");
                for (vars = response->variables; vars;
                     vars = vars->next_variable) {
                    

			if ((vars->name_length < rootlen)
                	        || (memcmp(root, vars->name, rootlen * sizeof(oid)) != 0)) {
                        /*
                         * not part of this subtree 
                         */
                        running = 0;
                        continue;
			}

                    numprinted++;
                    print_variable(vars->name, vars->name_length, vars);
                    
                    if ((vars->type != SNMP_ENDOFMIBVIEW) &&
                        (vars->type != SNMP_NOSUCHOBJECT) &&
                        (vars->type != SNMP_NOSUCHINSTANCE)) {
                        
                        // not an exception value 
                        //snmp_log(LOG_DEBUG, "not an exception value");
                        if (check
                            && snmp_oid_compare(name, name_length,
                                                vars->name,
                                                vars->name_length) >= 0) {
                                fprintf(stderr, "Error: OID not increasing: ");
                                fprint_objid(stderr, name, name_length);
                                fprintf(stderr, " >= ");
                                fprint_objid(stderr, vars->name,
                                             vars->name_length);
                                fprintf(stderr, "\n");
                                running = 0;
                                exitval = 1;
                            }
                        memmove((char *) name, (char *) vars->name,
                                vars->name_length * sizeof(oid));
                        name_length = vars->name_length;
                        //snmp_log(LOG_DEBUG, "MIB LENGTH: %ld\n", name_length);
                        
                        /*char *mib_name = (char *)malloc((vars->name_length + 100) * sizeof(char));
                         snprint_objid(mib_name, vars->name_length + 100, vars->name, vars->name_length);
                         
                         char *mib_value = get_string_from_asn_type(vars);
                         
                         char *mib_string = (char *)malloc((strlen(mib_name) + strlen(mib_value)) * sizeof(char));
                         sprintf(mib_string, "%s = %s", mib_name, mib_value);*/
                        
                        char *mib_string = (char *)malloc(maxVariableSize);
                        int snStatus = snprint_variable(mib_string, maxVariableSize, vars->name, vars->name_length, vars);
                        
                        if (snStatus > 0) {
                            mib_strings[i] = mib_string;
                        }
                        
                        i++;
                        
                    } else
                    {
                        // an exception value, so stop 
                        //snmp_log(LOG_DEBUG, "an exception value");
                        running = 0;
                    }
                }
            } else {
                /*
                 * error in response, print it 
                 */
                running = 0;
                if (response->errstat == SNMP_ERR_NOSUCHNAME) {
                    printf("End of MIB\n");
                } else {
                    fprintf(stderr, "Error in packet.\nReason: %s\n",
                            snmp_errstring(response->errstat));
                    if (response->errindex != 0) {
                        fprintf(stderr, "Failed object: ");
                        for (count = 1, vars = response->variables;
                             vars && count != response->errindex;
                             vars = vars->next_variable, count++)
                        /*EMPTY*/;
                        if (vars)
                            fprint_objid(stderr, vars->name,
                                         vars->name_length);
                        fprintf(stderr, "\n");
                    }
                    exitval = 2;
                }
            }
        } else if (status == STAT_TIMEOUT) {
            fprintf(stderr, "Timeout: No Response from %s\n",
                    ss->peername);
            running = 0;
            exitval = 1;
        } else {                /* status == STAT_ERROR */
            snmp_sess_perror("snmpwalk", ss);
            running = 0;
            exitval = 1;
        }
        if (response)
            snmp_free_pdu(response);
    }
    
    mib_strings[i] = '\0';
    
    return mib_strings;
}
