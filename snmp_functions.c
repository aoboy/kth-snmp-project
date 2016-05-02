/* 
 * File:   snmp_functions.c
 * Author: ant�nio gonga
 *
 * Created on 23 de Setembro de 2008, 12:51
 */

#include "prototypes.h"

/**
 *Returns the remote host name. 
 * 
 *@response - the response pdu that contains the response for our request
 *@host     - the host structure that we are requesting the name for
 *returns   - void
 */
void setRemoteHostSystemName(netsnmp_pdu *response, Host_t *host)
{
    netsnmp_variable_list *vars;
    
    for(vars = response->variables; vars; vars = vars->next_variable){
        if(vars->type == ASN_OCTET_STR){
            host->host_name = (string)malloc(1+vars->val_len);
            strncpy(host->host_name, vars->val.string, vars->val_len);
            host->host_name[vars->val_len] = '\0';
            break;
        }
    }    
}
/**
 *Sets the remote host number of interfaces to the correspondent host structure
 *@response -  the response PDU 
 *@host     - the host structure that we want to set the ifNumber
 */
void setRemoteHostIfNumber(netsnmp_pdu *response, Host_t *host)
{
    netsnmp_variable_list *vars;
    
    for(vars = response->variables; vars; vars = vars->next_variable){
       if(vars->type == ASN_INTEGER){           
           host->ifNumber  = *(vars->val.integer);	                
        }
    }    
}

/*Returns the remote host routing Table. When invoked, this function, sets the routing table for a host 
 *in our hosts list.
 *host     - the host element that 
 *response - the responde PDU.
 */
void getRemoteHostIpRTableIPs(Host_t *host, netsnmp_pdu *response)
{
    netsnmp_variable_list *vars;
    int ifIndex = 0;
    for(vars = response->variables;  vars;  vars = vars->next_variable, ifIndex++){
        if(vars->type = ASN_IPADDRESS){
          string ip_addr = (string)malloc(MAX_IP_ADDR_LEN*sizeof(char));
          sprintf(ip_addr, "%d.%d.%d.%d", vars->val.bitstring[0], vars->val.bitstring[1], 
                  vars->val.bitstring[2], vars->val.bitstring[3]); 
          if(strstr(ip_addr, "0.0") == NULL){   //checks if the returned IP is valid                  	      
	       //if the routing table exists in the interfaces, exclude it
	      if(ifTableEntryExists(host->ifTable, ip_addr) == 0){                 
                 AddRouteTableEntryToHost(host, ip_addr, ifIndex); /*add new neighbor*/
              }
 		//whenever a new host is found, add it to the hosts list
                AddHost(hostsList, ip_addr); /*add new host if this IPaddr is new*/ 	      
          }
        }else{
	   break;
	}
    }
}

/*sets the routing table indexes. The indexes reprent the interface number in the remote
 *host that IP address is associated with
 *host    - the host structure 
 *response- the response PDU
 */
void getRemoteHostIpRTableIfIndexs(Host_t *host, netsnmp_pdu *response)
{    
    netsnmp_variable_list *vars;
    int ifIndex = 0;
 
    for(vars = response->variables; vars;  vars = vars->next_variable, ifIndex++){
        if(vars->type == ASN_INTEGER){
	  //set the ifIndex value to the correct IpAddress
          setIpRTableIfIndex(host, ifIndex, vars->val.bitstring[0]);	   
        }else{
	   break;
	}
    }
}

/*Sets the IpAddrTable to the host. This function sets the IP addresses for the Iinterfaces
 *host   - the host element that we want to set the IfIPs
 *response- the response PDU
 *returns - void
 */
void getRemoteHostIpAddrTableIPs(Host_t *host, netsnmp_pdu *response)
{    
    int ifIndex = 1;
    netsnmp_variable_list *vars;
    char ip_addr[MAX_IP_ADDR_LEN];

    for(vars = response->variables;  vars && (ifIndex <= host->ifNumber) ; vars = vars->next_variable)
    {
        if(vars->type = ASN_IPADDRESS)
	  {
             memset(ip_addr, (int)'\0', MAX_IP_ADDR_LEN);
             sprintf(ip_addr, "%d.%d.%d.%d", vars->val.bitstring[0], vars->val.bitstring[1], 
                  vars->val.bitstring[2], vars->val.bitstring[3]);
		
	     //if entry already exists, skip it
             if(ifTableEntryExists(host->ifTable, ip_addr) != EXISTS )
	      {   
		  //add a new Ip addr to the interface		  
          	  AddIfTableEntryToHost(host, ip_addr, ifIndex);
                
                 ifIndex++;
             }else{
               printf("IfIndex: %d\n", ifIndex);
	        break;
	      }
          }
    }
  
}

/*
 *sets the interface Indexes. When setting the IFIndexs, the number of indexes must be equal to
 *to the number of IP addresses already set.
 *host    - the host structure 
 *responde- the response PDU
 */
void getRemoteHostIpAddrTableIfIndexs(Host_t *host, netsnmp_pdu *response)
{
    int ifIndex = 0;
    ipAddrTable_t *entry = host->ifTable->_next; //access the IpAddrTable
    netsnmp_variable_list *vars;    
	
    for(vars = response->variables;  vars && entry;  vars = vars->next_variable)
    {
        if(vars->type == ASN_INTEGER){	   
	     ifIndex++;
	     entry->ifIndex = vars->val.bitstring[0];
	     entry = entry->_next;
        }else{
	    break;
	 }
    }
}

/*
 *returns the interfacs description. By interface descripion, means the type of interface
 *host    -
 *response-
 */
void getRemoteHostIpAddrTableIfDescr(Host_t *host, netsnmp_pdu *response){

  ipAddrTable_t *entry = host->ifTable->_next; //access the host's interfaces table
  netsnmp_variable_list *vars;  
   
     for(vars = response->variables;  vars && entry;  vars = vars->next_variable)
     {
        if(vars->type == ASN_OCTET_STR){
	     if(strstr(vars->val.string, "Null0") != NULL){
               entry->ifDesc = (string)malloc(strlen("FasterEthernet0/0")+1);
               entry->ifDesc[strlen("FasterEthernet0/0")+1] = '\0';
		 strcpy(entry->ifDesc, "----------------");
	     }else{
	     	 entry->ifDesc = (string)malloc(1+vars->val_len);
            	 strncpy(entry->ifDesc, vars->val.string, vars->val_len);
            	 entry->ifDesc[vars->val_len] = '\0';
            }
	     entry = entry->_next;
        }else{
	    break;
	 }
     }
}

/*
 *sets the Interface speed
 * host    -
 * response-
 */
void getRemoteHostIfSpeed(Host_t *host, netsnmp_pdu *response)
{
  netsnmp_variable_list *vars;    
  ipAddrTable_t *entry = host->ifTable->_next; //access the host's ipIfTable

  for(vars = response->variables;  vars  ;  vars = vars->next_variable)
  {
	if(vars->type == ASN_GAUGE){
     	     entry->ifSpeed =  *(vars->val.integer)/1000000; //convert the speed to Mbps
	     entry = entry->_next;
       }else{
	   break;
	}
  }
}

/*it retrieves  the input number of octets for a host. This is a sum of the input octets in all interfaces
 *host - 
 *responde - response PDU
 */
void getRemoteHostInOctets(Host_t *host, netsnmp_pdu *response)
{    
  netsnmp_variable_list *vars;  
  
  for(vars = response->variables;  vars ;  vars = vars->next_variable)
  {
	if(vars->type == ASN_COUNTER){
            if(init_ld){ 
	      //at beginning, inOctetsTi=InOctetsTi-1
               host->load->inOctets0 = host->load->inOctets0 + *(vars->val.integer);
	     }else{		
     	        host->load->inOctets1 = host->load->inOctets1 + *(vars->val.integer);
            }
       }else{
	   break;
	}
  }
}

/*
 *The goal for this function is to initialize the load vector that is represented in the X- axis.
 *This also clears up the histogram values after each polling. The X-scale is 5 in our case.
 *
 */
void init_load_array(){
  int k;
  for(k = 0; k < N_XBARS; k++){
	load_array[k] = (float)5*k;       
       hist_array[k] = 0;
  }
 }

/*
 *This function aims to generate the traffic histogram for the system after a polling has been made.
 *The ideia is to find how many routers (Y-axis) have the corresponding load (X-axis). I made an assumption 
 *based on an interval [val-delta, val+delta[: The goal is to approximate values to a defined value in the X-axis.
 *Lets assume that a host has a load of 4.05Kbps or 5.98Kbps respectivelly. These values would be aproximated to 5
 *once my defined delta is equal to 1. Shorting delta, gives a more acqurated values represented in the histogram
 *
 *Based on this idea, the loops counts the number of routers whose load is equal to the load array represeted in the 
 *X-axis, and the number of routers give the vertical bars that corresponds to the load histogram.
 */
void generate_histogram(){
 int i,k; 
 float delta = 1.0; 
 Host_t *host;  	

       init_load_array();
      
       for(i = 0; i < N_XBARS; i++)
       {	
          for( host = hostsList->next_host ; host != NULL;  host = host->next_host)
	   {	  
		//if the router's load fits in the array value, increments the number of routers that have these value.
	      if(( (load_array[i] - delta) <= host->load->load_val) && (host->load->load_val < (load_array[i]+delta) )){                
		  hist_array[i] = hist_array[i] + 1; 		 
	      }		
	   }        
       }
 
       //thsi for Loop, it merely checks the hist_array values and prints out the vertical bars on the screen
	printf("\nNr\n");
       for(i = hist_intervals;  i >= 0 ; i--)
       {   
	     printf("%d|", i);

            for(k = 0; k < N_XBARS; k++){		
		
 		 if(hist_array[k] != 0 && hist_array[k] >= i && hist_array[k] <= hist_intervals ){		 
                  printf("    *");
                }else{		  
                  printf("     ");
		  }
	     }
           printf("\n");
       }

	//prints the X bars divisions
    printf(" |");
    for( k = 0 ;  k <= N_XBARS; k++){
	 printf("----|");
    }
    //correct the numbers position in the X axis
    printf("\n  ");
    for(k = 0 ;  k <= N_XBARS; k++){
        if(k*5 < 10)
	  printf("   %2d", k*5);
        else if(k*5 >= 10 && k*5 <= 100)
	   printf("   %2d", k*5);
        else printf("  %2d", k*5);
    }
    
    printf("  [Kbps]\n\t\t\t\t\t\t\t\tLOAD\n");
 
  for( k = 0 ;  k <= N_XBARS+2; k++){
	 printf("=====");
  }
}

/**
  *Computes the Load for each Host:
  * 			     Sum(InOctets(Ti)) - Sum(InOctets(Ti-1))
  *    Host[i]_Load = 8* ---------------------------------------- [Kbps]
  *                            1000*sampling_period
  */
		
void compute_load()
{
   int count = 0;   
   unsigned int KB_DIV = 1000;
   unsigned long A;
   unsigned long B;

   Host_t * host = hostsList->next_host;


   printf("\n\t----------------\n\t Router's Load\t\t\tNETWORK TRAFFIC MONITOR\n\t----------------\n");
   for( ; host != NULL; host = host->next_host)
   {
       if(host->load->inOctets1 > host->load->inOctets0){
	     	A = host->load->inOctets1 - host->load->inOctets0;
	}else{
	       A = MAX_ULONG - host->load->inOctets0 + host->load->inOctets1;
	}                   

   	host->load->load_val = (8*((float)A))/(KB_DIV*delta_samples);  // 8* - convert to Kbits/sec
                            	
	printf("\t%s: %4.2f [Kbps] \n", host->host_name, host->load->load_val);	
         
        //Inoctets(Ti-1)=InOctets(Ti)
   	host->load->inOctets0  = host->load->inOctets1; 
       host->load->inOctets1  = 0;   
   }
    
   printf("\n");   
}
