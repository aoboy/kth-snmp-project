/*CORE_PROGRAM_MODULE
 *This file contains the heart of the program. The mainLoop function initializes the snmp library and it calls
 *getNetworkTopology in order to discover all the routers in the network. Once this step is done, there is a whil
 *loop that runs infinitelly untill the user presses Ctrl-C. The goal for this loop is to pool the routers periodically
 *when the sampling time has been reached. To do so, the sleep function was used in order to block the loop for 
 *sampling time seconds before performing another poll to the routers by invoking the function getHostsLoad(). 
 *Once the calculated for each host, other function is called to generate the network load histogram.
 *
 */
/* 
 * File:   core_program_module.c
 * Author: ant�nio gonga
 *
 * Created on 26 de Setembro de 2008, 12:51
 */


#include "prototypes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* my_oids_t describe the MIBs that will be queried for each host. 
 * 
 */
 struct my_oids_t{  
       int type;
       const string Name;
       oid Oid[MAX_OID_LEN];
       int OidLen;
 }topology_oids[] = {       
       { SYSTEM_IFNBR,      ".1.3.6.1.2.1.2.1"},           /*syetsm ifNumber*/
       { SYSTEM_NAME,       ".1.3.6.1.2.1.1.5"},           /*System name*/
       { IPADDRTBL_IP,      ".1.3.6.1.2.1.4.20.1.1"},      /*IpAddrTblAddr*/
       { IPADDRTBL_IF,      ".1.3.6.1.2.1.4.20.1.2"},      /*IpAddrTblIfIndex*/
       { IPRTBL_NXTHOP,     ".1.3.6.1.2.1.4.21.1.7"},      /*ipRTblNextHopIpAddr*/
       { IPRTBL_IFIDX,      ".1.3.6.1.2.1.4.21.1.2"},      /*ipRTblIfIndex*/
       { IPADDRTBL_IFDESC,  ".1.3.6.1.2.1.2.2.1.2"},       /*IpAddrTblIfDescr*/
       { IFSPEED,           ".1.3.6.1.2.1.2.2.1.5"},       /*ifSpeed -  the speed of the interface*/
       { IFINOCTETS,        ".1.3.6.1.2.1.2.2.1.10"},      /*IfInOctects*/
       { NULLOID,       	NULL}
   }, load_oids[] = {
       { IFINOCTETS,  ".1.3.6.1.2.1.2.2.1.10"},            /*IfInOctects - total n� of octets received from interfaces*/
       { NULLOID,     NULL}
   };     

/**
 *MainLoop - the core part of the program. It requests the network topology and 
 *the calculates the load. 
 * host_snmpname - the host/node snmp name
 * host_ip  - the stating node IP address
 * host_pass - The stating point snmp passphrase
 * deltaSamples - the sampling interval in order to display the load
 * nIntervals - histogram intervals
 * returns - void
 */
void mainLoop(string host_snmpname, string host_ip, string host_pass, int deltaSamples, int nIntervals){

    unsigned long  sample_time=0, count =1;  //elapsed time and sampling times count

    host_snmp_name  = host_snmpname;    //set the first node snmp name

    host_passphrase = host_pass;        // set the first node snmp password

    delta_samples  = deltaSamples;      // set the sampling period

    hist_intervals = nIntervals;        // set the nº of intervals in the histogram
    

    AddHost(hostsList,host_ip);         // Add the first host to the global list of nodes
   
    init_snmp("histogram");  /*Initialize the SNMP Library*/  
       
    fp = (FILE*)create_file("w");       //create a file to write the network topology(optional)

    init_ld = 1;

    getNetworkTopology(hostsList->next_host);     //retrieve the network topology

    PrintHosts();             //printf the network topology

    close_file();             //close the topology file

    init_ld = 0;

    //displays on the screen " Load Histogram"
    printf("\t\t%s\t\t%s\t\t%s\t\t%s\t\t%s\n\n", LD1,LD2, LD3,LD2, LD1);  
  
    //pause two seconds 
    sleep(delta_samples);                

    //enter in a interrupted infinite loop whever the clock fires (i.e each delta_samples) 
    while(1){  
        sample_time += delta_samples;        
        
        printf("\n\n\t----------------------------------------------\n");
        printf("\t Sample: %u (Elapsed Time = %u seconds)\n", count++, sample_time);
        printf("\t----------------------------------------------\n\n");

        //get the load for each node/host
        getHostsLoad();        	       
        //compute the load for each host
        compute_load();

        generate_histogram();
        //sleep delta_samples seconds
        sleep(delta_samples);                
    }
    //RemoveHost(hostsList, host_ip);  
}

/**
 * Initialize all the oids - the main goal is to set the correct oid_len, and to parse out the oid
 * oids -  the oid that will be initialized
 * returns -  void
 */
void initialize_oids(struct my_oids_t *oids)
{
    while(oids->Name){
       
        oids->OidLen = MAX_OID_LEN;
        
        //parse the oids
        if(!snmp_parse_oid(oids->Name, oids->Oid, &oids->OidLen)){
            snmp_perror(oids->Name);
            exit(1);
        }        
        oids++;
    }
}

/**
  *creates a SNMP_V3 session for a specific host
  *
  *host - the peer that we want to connect to. This contains the peername/ip_addr peer passphrase 
  *       and other necessary parameters in order to stablish the connection.
  *returns void
  **/
void createSNMPv3Session(Host_t *host){

    //netsnmp_session session;
    
    
     /* Initialize session*/
    snmp_sess_init(&session);
    
     /* set peer name*/
    session.peername = strdup(host->host_ip_addr);
    session.version  = SNMP_VERSION_3;    /*set SNMP version*/
    
    
    /* set SNMPv3 authentication parameters*/
    session.securityName         = strdup("2G1332_student");
    session.securityNameLen      = strlen(session.securityName);

    /*Set secuirity level to authenticated but not encrypted*/
    session.securityLevel        =  SNMP_SEC_LEVEL_AUTHNOPRIV;

    /*set the authentication method to MD5*/
    session.securityAuthProto = usmHMACMD5AuthProtocol;
    session.securityAuthProtoLen = sizeof(usmHMACMD5AuthProtocol)/sizeof(oid);
    session.securityAuthKeyLen = USM_AUTH_KU_LEN;
       
    if(generate_Ku(session.securityAuthProto,
                   session.securityAuthProtoLen,
                   (u_char*)host->host_pwrd, strlen(host->host_pwrd),
                   session.securityAuthKey,
                  &session.securityAuthKeyLen) != SNMPERR_SUCCESS){

        snmp_log(LOG_ERR, "\t Error while generating Ku from Authentication Pass Phrase!\n");
        exit(1);        
    }
    
    SOCK_STARTUP;
    /*open snmp session*/
    ss = snmp_open(&session);
    if(!ss){
        snmp_sess_perror("ack", &session);
        SOCK_CLEANUP
        exit(1);
    }
    /*if the session was successfully opened, return the pointer of this session*/    
}

/**
 * creates a SNMP_V3 session
 * host - the host/node who wants to stablish the connection
 * op - the oid' that the request pdu will requesting for
 * returns - pointer to the created snmp pdu
 */
netsnmp_pdu *create_snmp_pdu_type(Host_t *host, struct my_oids_t *op)
{
    netsnmp_pdu *req;
    
    switch(op->type){
        case IPRTBL_NXTHOP   :  
        case IPRTBL_IFIDX    :  req = snmp_pdu_create(SNMP_MSG_GETBULK); 
                                req->non_repeaters   = 0;
                                req->max_repetitions = GET_BLK_MAX_REPEAT;
                                break;
        case SYSTEM_NAME     :  
        case SYSTEM_IFNBR    :  req = snmp_pdu_create(SNMP_MSG_GETBULK);
				    req->non_repeaters   = 0;
                                req->max_repetitions = 1;
                                break;
        
        case IPADDRTBL_IP    :
        case IPADDRTBL_IF    :  
	 case IPADDRTBL_IFDESC:  req = snmp_pdu_create(SNMP_MSG_GETBULK);
                                req->non_repeaters   = 0;
                                req->max_repetitions = host->ifNumber; 
        			    break;

	 case IFSPEED         :
        case IFINOCTETS      :  req = snmp_pdu_create(SNMP_MSG_GETBULK);
				    req->non_repeaters   = 0;
				    req->max_repetitions = host->ifNumber-1; 
				    break;
    }
       
    return req;
}

/**
 *When invoked from MainLoop, this function returns the network topology, and 
 *host/node relative informations. When a new host is found, it's added to the 
 *global list, the procedure continues untill all the nodes have been found in 
 *the network.
 *The function itself its now a recursive function, it also works as a non-recursive 
 *function. The switch can be made easily by uncommenting some parts in the code and commenting others. The input paramater when running as recursive, is the head of the hosts list.
 * the first valid element, is the one created with the input parameters. The subsequent
 *elements are added as the program runs and discovers the network
 */
//void getNetworkTopology()
void getNetworkTopology(Host_t *host)
{        
    //Host_t *host;            //pointer to the hosts/nodes list
    int status, count, idx;    //some indexes
    struct variable_list *vp;  //pointer to variables list
    struct my_oids_t *op;      //pointer to my_oids_t structure
 
    //host = hostsList->next_host; //skip first element(list Base)
     if(host == NULL)
	 return;
        
    //while hosts/nodes list is not empty, get the Routing table, Ip Addr table,
    //host  name, hostIfNumber and relevant information. for each new host add 
    //to the same list. Duplicated hosts does not exist, and IP address belonging 
    //to the same host/node only one of them is entered in the list
    //for(  ; host != NULL; host = host->next_host)
    //{       
        //create SNMP_V3 session
	createSNMPv3Session(host);  
        
        //loop for all MIB OIDs that we want to request for
       for( count= 0, op = topology_oids; op->Name; op++, count++)
        {            
             //initializes the OID
	    initialize_oids(op);	              

            netsnmp_pdu  *response;
	    netsnmp_pdu *pdu;
            
            //create the appropriate PDU
            pdu = create_snmp_pdu_type(host, op);
	    //apped null variable to the PDU    
    	    snmp_add_null_var(pdu, op->Oid, op->OidLen);
            
            //get response and the satus of the response
            status = snmp_synch_response(ss, pdu, &response);	     
		           
            //check status
            if((status == STAT_SUCCESS) /*&& (response->errstat = SNMP_ERR_NOERROR)*/){
                
                //check requested MIB and select correct function
                switch(op->type){
                    case IPRTBL_NXTHOP   :  getRemoteHostIpRTableIPs(host, response); break;

                    case IPRTBL_IFIDX    :  getRemoteHostIpRTableIfIndexs(host, response); break;

                    case SYSTEM_IFNBR    :  setRemoteHostIfNumber(response, host); 					     
					         break;

                    case SYSTEM_NAME     :  setRemoteHostSystemName(response, host); break;

                    case IPADDRTBL_IP    :  getRemoteHostIpAddrTableIPs(host, response); break;

                    case IPADDRTBL_IF    :  getRemoteHostIpAddrTableIfIndexs(host, response); break;
		
                    case IPADDRTBL_IFDESC:  getRemoteHostIpAddrTableIfDescr(host, response);
						  break;
                 
                    case IFSPEED         :  getRemoteHostIfSpeed(host, response); 
                                             break;
		     
		    case IFINOCTETS      :  getRemoteHostInOctets(host, response);
						  break;
			
                }
	         //printf("SUCCEED to query Remote Agent with OidName: %s \tHostName: %s\n", op->Name, host->host_snmpName); 		  
     	
            }else{
			printf("FAILED to query Remote Agent with OidName: %s\tHostName: %s\n", op->Name, host->host_ip_addr);   

			if(status == STAT_SUCCESS){
				fprintf(stderr, "*****Error in packet --> Reason: %s*****\n", snmp_errstring(response->errstat));						
                            fprintf(stdout, "peername: %s, secName: %s, reason: %s\n", ss->peername, ss->securityName, snmp_errstring(response->errstat));
			}else if(status == STAT_TIMEOUT){
			    fprintf(stderr, "Timeout: No Response from %s.\n", session.peername);
			}else{
			      snmp_sess_perror("histogram", ss);		
			}   	
	     } //end of else (status STATSCCSS SNMP_ERR_NO ERR
            
            snmp_free_pdu(response);             

        }  //end of for loop oids

	snmp_close(ss);                    
    //}

     //browse recursivelly the network untill all routers have been found.
   return getNetworkTopology(host->next_host);  
   
}

/**
 * Retrieve the Load for each of the network host/node
 * returns - void
 */
void getHostsLoad()
{
  int status;
  Host_t *host; 
  struct my_oids_t *opx;
  struct variable_list *vp;  
     
  //poll each host/node in the network
  for(host = hostsList->next_host; host != NULL; host = host->next_host)
  {
     //creates a session for this host.
     createSNMPv3Session(host);  
    
     for(opx = load_oids; opx->Name; opx++)
     {
	    initialize_oids(opx);	              

           netsnmp_pdu  *response;
	    netsnmp_pdu *pdu;
           pdu = create_snmp_pdu_type(host, opx);  //create a specific PDU type
	       
            //add null var to the pdu
    	    snmp_add_null_var(pdu, opx->Oid, opx->OidLen);  
            
            //send request and get the response PDU
            status = snmp_synch_response(ss, pdu, &response);			
		
             //check the status of the received PDU
	     if((status == STAT_SUCCESS) /*&& (response->errstat = SNMP_ERR_NOERROR)*/)
	     {
		  switch(opx->type){
			case IFINOCTETS: getRemoteHostInOctets(host, response);
					   break;
                }
               //printf("SUCCEED to query Remote Agent with OidName: %s \tHostName: %s\n", opx->Name, host->host_ip_addr); 		  
	     }else{
		    	printf("FAILED to query Remote Agent with OidName: %s\tHostName: %s\n", opx->Name, host->host_ip_addr);   

			if(status == STAT_SUCCESS){
				fprintf(stderr, "*****Error in packet --> Reason: %s*****\n", snmp_errstring(response->errstat));						
                            fprintf(stdout, "peername: %s, secName: %s, reason: %s\n", ss->peername, ss->securityName, snmp_errstring(response->errstat));
			}else if(status == STAT_TIMEOUT){
			    fprintf(stderr, "Timeout: No Response from %s.\n", session.peername);
			}else{
			      snmp_sess_perror("histogram", ss);		
			}   	
	     }
		   
          snmp_free_pdu(response);             

     } //end of for oids

	 snmp_close(ss);                         
  }  //end of loop_Hosts

}  //end of getHostsLoad()


