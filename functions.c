/*
 *This file contains mostly functions for manipulating host structure data. Such a functions include insertion, update
 *and removal of parameters. It also includes verification functions that helps to avoid duplicated parameters.
 */
/* 
 * File:   functions.c
 * Author: ant�nio gonga
 *
 * Created on 15 de Setembro de 2008, 12:51
 */


#include "prototypes.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 *creates  routing table structure entry and returns it
 */
ipRouteTable_t* createIPRouteTable(){

    ipRouteTable_t* ipRTable = (ipRouteTable_t*)malloc(sizeof(ipRouteTable_t));

    if(ipRTable == NULL){
        printf("\tunable to allocate memory");
        exit(1);
    }
    //initializes fiels
    ipRTable->ifIndex = 0;
    ipRTable->index   = 0;
    ipRTable->next    = NULL;
}

/**
 * creates a IP Addr table structure entry and returns it
 */ 
ipAddrTable_t* createIPAddrTable(){

    ipAddrTable_t *iptable = (ipAddrTable_t*)malloc(sizeof(ipAddrTable_t));

    if(iptable == NULL){
        printf("\tunable to allocate memory!\n");
        exit(1);
    }
    //initializes fields
    iptable->ifIndex = 0;
    iptable->ifSpeed = 0;
    iptable->ifDesc  = NULL;
    iptable->_next   = NULL;
}

/**
 * creates a host/node entry/space(empy) and returns it 
 */
Host_t* createHostStructure(){
   Host_t *hostbase;

        hostbase = (Host_t*)malloc(sizeof(Host_t));
    
        if(hostbase != NULL){
            hostbase->ifNumber = 0;
            hostbase->next_host  = NULL;
            return hostbase;
        }else{
            printf("\n\tMEMORY ALOCATION FAILURE!\n");
            exit(1);
        }        
        return NULL;
}

/**
 * creates a host entry, sets the host's IP address and returns it
 */
Host_t* createHost(string host_ip){
   
    Host_t *h   = createHostStructure();                             
    
    //creates memory for the host/node SNMP name
    h->host_snmpName = (char*)malloc((strlen(host_snmp_name)+2)*sizeof(char));
	
    //creates memory for the passphrase
    h->host_pwrd = (char*)malloc((strlen(host_passphrase)+2)*sizeof(char));
    //creates the host Ip Addr table base pointer
    h->ifTable    = createIPAddrTable();    
    //creates the host Routing table base pointer
    h->routeTable = createIPRouteTable();
	     
    //creates the load pointer for the host
    h->load = (struct load_t*)malloc(1*sizeof(struct load_t));
    //initializes host's load fields
    h->load->inOctets0  = 0;       //number of received Octets in Ti
    h->load->inOctets1  = 0;	//number of received Octets in Ti+1
   
    //clears the hos_ip_addr field
    memset(h->host_ip_addr, (int)'\0' , MAX_IP_ADDR_LEN);
    strcpy(h->host_ip_addr, host_ip);   //sets host IP_Addr
    
    //set SNMP security info(name, pass)
    strcpy(h->host_pwrd, host_passphrase); 
    strcpy(h->host_snmpName, host_snmp_name);
       
    return h;
}
/**
 *creates an IP rounting table entry, assignes the IP address and returns it
 */
ipRouteTable_t* createIpRouteEntry(int index, string ip_addr){
    
        ipRouteTable_t* ipRoute_entry = createIPRouteTable();
        
        ipRoute_entry->index = index;
        strcpy(ipRoute_entry->ip_addr, ip_addr); //set the IP addr
        
    return ipRoute_entry;
}
//sets the correct interface index to the IpAddrTable entry
void setIpAddrTableIfIndex(Host_t *host, int index, int ifIndex){
    ipAddrTable_t *entry; 
    for(entry= host->ifTable->_next ;  entry != NULL; entry = entry->_next)
        if((entry->ifIndex + 1)== index){
            entry->ifIndex = ifIndex;
        }
}

void setIpRTableIfIndex(Host_t *host, int index, int ifIndex){
    ipRouteTable_t *entry;

    for(entry = host->routeTable->next ; entry != NULL; entry = entry->next){
        if(entry->index == index)
            entry->ifIndex = ifIndex;
    }
}
/**creates an interface entry and returns it
 */
ipAddrTable_t* createIfEntry(int index, string ip_addr){
        
        ipAddrTable_t* if_entry = createIPAddrTable();        
	 
        if_entry->ifIndex = index;
        if_entry->ifDesc  = NULL;        
        strcpy(if_entry->if_ip_addr, ip_addr);  
                   
    return if_entry;
}

/**
 *One of the core functions. It avoids both host/node interfaces being added as
 *separate nodes. It cheks if the host exists and at the same time if checks if
 *this host has an interface with the incoming ip address.
 * return - 1 if the IP addr already exists, 0 otherwise
 */
int hostIPExists(string host_ip){
    
    Host_t *list = hostsList->next_host;  /*Skip first element*/    
         
        while(list != NULL)
        {
            if(strEquals(list->host_ip_addr, host_ip)){
                return EXISTS;
            }else if(ifTableEntryExists(list->ifTable, host_ip) == 1){
		  return EXISTS;
	     }
              list = list->next_host; /*continue browsing the list*/              
        }
    
    return 0; /* does not exist*/   
}

/**
 * This function avoids duplicated entries in the node's routing table
 */
int ipRouteEntryExists(ipRouteTable_t *entry, string ip_addr){
    ipRouteTable_t *list = entry->next;
    if(list != NULL){
        while(list != NULL){
            if(strEquals(list->ip_addr, ip_addr)){                
                return EXISTS;
            }
            list = list->next;
        }
    }    
    return 0;    
}

/**
 * checks if for this host, the ip_addr already exists.
 * returns  1 if the entry already exists, 0 otherwise
 */
int ifTableEntryExists(ipAddrTable_t *entry, string ip_addr){
    ipAddrTable_t *list = entry->_next;
    
    if(list != NULL){
        while(list != NULL)
        {
            if(strEquals(list->if_ip_addr, ip_addr)){                
                return EXISTS;
            }  
          list = list->_next;      
        }        
    }
    
    return 0;
}

/**
 * removes an entry of a node's Routing table
 */
ipRouteTable_t *removeIpRouteEntry(ipRouteTable_t *entry){
    ipRouteTable_t *temp = entry->next;
    free(entry);
    
    return temp;
}
/**
 * removes an entry of a node's IP Addr table
 */
ipAddrTable_t* removeIpAddrEntry(ipAddrTable_t * entry){
    ipAddrTable_t *temp = entry->_next;
    free(entry);
    return temp;
}
/**
 * removes all entries of the routing table
 */
void removeIpRouteTableEntries(ipRouteTable_t *entry){
    ipRouteTable_t *list = entry;
    ipRouteTable_t *back;
    
    if(list == NULL){
        return;
    }else if(list->next == NULL){
        free(list);
    }else{        
        while(list != NULL){
            back = list;
            list = removeIpRouteEntry(back);
            /*list = list->next; */
        }
    }
}

/**
 * remove all entries of the IP Addr table
 */
void removeIfTableEntries(ipAddrTable_t *entry){
    ipAddrTable_t *list = entry;
    ipAddrTable_t *back;
    
     if(list == NULL){
        return;  
    }else if(list->_next == NULL){
        free(list); 
    }else{        
        while(list != NULL){
            back = list;
            list = list->_next;
            free(back);
        }
    }
}

Host_t* Remove(Host_t *host){
    Host_t *temp;
    
    removeIfTableEntries(host->ifTable);   
    removeIpRouteTableEntries(host->routeTable);
            
    temp = host->next_host;
    printf("\n\tHost with IP_ADDR: %s, removed\n",host->host_ip_addr);
    free(host);
    return temp;
}
/**
 *previously conceived to remove a host entry, it's now when invoked removes 
 *all the hosts/nodes list
 *return - void
 **/
void RemoveHost(Host_t *hostListBase, string host_ip){
    Host_t *list = hostListBase->next_host;
    Host_t *back = hostListBase;
    
    while(list != NULL){
        //if(strEquals(list->host_ip_addr, host_ip)){            
            back->next_host = Remove(list);
            //return;
        //}
        back = list;
        list = list->next_host;
    }        
}

/**
 * adds a node/host to the hosts list
 * returns - void
 */
void AddHost(Host_t *hostListBase, string ip_addr)
{
    Host_t *host = hostsList->next_host;      

    if(host != NULL){        
        //check first if the element is already in the list
        if(hostIPExists(ip_addr) != EXISTS){
            while(host->next_host !=NULL)
                host = host->next_host;   
            
            //Add/apped  the new element at the end
            host->next_host = createHost(ip_addr);           
        }         
    }else{
	//list empty, creates first valid entry
        hostListBase->next_host = createHost(ip_addr);          
    }
   
}

/**
 * add an entry of the Ip Addr Table to the hosts IP Addr Table
 */
void AddIfTableEntryToHost(Host_t *h, string ip_addr, int index)
{
    ipAddrTable_t *it = h->ifTable->_next;

    if(it != NULL){               
          while(it->_next != NULL)
             it = it->_next;   
         
         it->_next = createIfEntry(index, ip_addr);            
    }else{
        //add the first valid element
        h->ifTable->_next = createIfEntry(index, ip_addr);        
    }    
}

/**
 * add an elemeny to the Routing table 
 */
void AddRouteTableEntryToHost(Host_t *host, string ip_addr, int Index )
{
    ipRouteTable_t *list = host->routeTable->next;
    if(list != NULL)
    {
        //check first if this entry was entered already
        if(ipRouteEntryExists(host->routeTable, ip_addr) != EXISTS && strcmp(ip_addr, host->host_ip_addr) != 0){
            while(list->next != NULL)
                list = list->next;
            
            list->next = createIpRouteEntry(Index, ip_addr);
        }
    }else{
         if(strcmp(ip_addr, host->host_ip_addr) != 0)
             host->routeTable->next= createIpRouteEntry(Index, ip_addr);
    }
   
}

//print the Ip Addr Table on the creen and also write it to the output file
void PrintHostIfTableEntries(ipAddrTable_t *entry)
{
    ipAddrTable_t *list;
       
    for(list = entry->_next; list != NULL; list = list->_next){
        printf("\t     %2d \t     %s\t\t%u\t\t%s\n", list->ifIndex, list->ifDesc, list->ifSpeed, list->if_ip_addr); 
        fprintf(fp, "\t     %2d \t     %s\t\t%u\t\t%s\n", list->ifIndex, list->ifDesc, list->ifSpeed, list->if_ip_addr);
    }    
}

//printf the Routing table on the screen and also write it to the output file
void PrintHostIpRouteTableEntries(ipRouteTable_t * entry){
    ipRouteTable_t * list = entry->next;
      
    for( ; list != NULL; list = list->next){
        printf("\t       %2d    \t\t%s\n", list->ifIndex, list->ip_addr); 
        fprintf(fp, "\t       %2d    \t\t%s\n", list->ifIndex, list->ip_addr);
    }
    printf("%s",LD1);    
    fprintf(fp, "%s",LD1);
    
}

/**
 * print all host, host's ip Addr table, and host's routing table
 * returns - void
 */
void PrintHosts(){
    int k = 1;
    Host_t *host = hostsList->next_host;
    
    puts("\n");     

    printfInitMessage();

    for(  ; host != NULL; host = host->next_host, k++){
        printf("%s",SEP_DIV_BAR);
        printf("%s",SEP_DIV_BAR);
        fprintf(fp, "%s", SEP_DIV_BAR);
        fprintf(fp, "%s", SEP_DIV_BAR);
	 printf("NODE_NR: %2d\n", k, host->host_ip_addr); 
        fprintf(fp,"NODE_NR: %2d\n", k, host->host_ip_addr);       
        printf("HOST_NAME: %s\nHOST_IP_ADDR: %s\n", host->host_name, host->host_ip_addr);
        fprintf(fp, "HOST_NAME: %s\nHOST_IP_ADDR: %s\n", host->host_name, host->host_ip_addr);
        printf("HOST_IF_NUMBER: %2d\n\n", host->ifNumber);
        fprintf(fp, "HOST_IF_NUMBER: %2d\n\n", host->ifNumber);
        
        printf("\t\t\tIP_ADDR_TABLE\n");
        fprintf(fp,"%s", "\t\t\tIP_ADDR_TABLE\n");
        printf("%s",SEP_BAR);
        fprintf(fp, "%s",SEP_BAR);
	 printf("\t|  INTERFACE Nr  |      IF_DESCR      |  IF_SPEED[Mbps]  |  IF_IP_ADDRRESS  |\n");
        fprintf(fp, "\t|  INTERFACE Nr  |      IF_DESCR      |  IF_SPEED[Mbps]  |  IF_IP_ADDRRESS  |\n");
        printf("%s",SEP_BAR);
        fprintf(fp, "%s",SEP_BAR);
        PrintHostIfTableEntries(host->ifTable);
        printf("%s",SEP_BAR);
        fprintf(fp, "%s",SEP_BAR);
	 printf("\n\t\t\tIP_ROUTING_TABLE\n");
        fprintf(fp, "\n\t\t\tIP_ROUTING_TABLE\n");
        printf("%s",LD1);
        fprintf(fp, "%s",LD1);
	 printf("\t| INTERFACE Nr |     NEIGHBOR IP ADDRESS      |\n");
        fprintf(fp, "\t| INTERFACE Nr |     NEIGHBOR IP ADDRESS      |\n");
        printf("%s",LD1);
        fprintf(fp, "%s",LD1);
        PrintHostIpRouteTableEntries(host->routeTable);        
    }    
    printf("%s",SEP_DIV_BAR);
    printf("%s",SEP_DIV_BAR);
    printf("\n\tTOTAL ROUTERS/NODES: %2d\n", k-1);
    fprintf(fp, "%s%s", SEP_DIV_BAR, SEP_DIV_BAR);
    fprintf(fp,"TOTAL ROUTERS/NODES: %2d\n", k-1);
    puts("\n\tLaunching Traffic Monitor..............\n");   
}

//creates an output file containg the network topology
FILE *create_file(string mode){
   FILE *fd = fopen("output.txt", mode);
      
     return fd; 
}

/// close the output file
void close_file(){
  fclose(fp);
}

void printfInitMessage(){
  printf("\n\n\t*-------------------------------------------------------------------------------*\n");
  fprintf(fp, "\t*-------------------------------------------------------------------------------*\n");
  printf("\t|                                                                               |\n");
  fprintf(fp, "\t|                                                                               |\n");
  printf("\t|                       KTH - ROYAL INSTITUTE OF TECHNOLOGY                     |\n");
  fprintf(fp, "\t|                       KTH - ROYAL INSTITUTE OF TECHNOLOGY                     |\n");
  printf("\t|                                                                               |\n");
  fprintf(fp, "\t|                                                                               |\n");
  printf("\t*-------------------------------------------------------------------------------*\n");
  fprintf(fp, "\t*-------------------------------------------------------------------------------*\n");
  printf("\t|              SCHOOL OF ELECTRICAL ENGINEERING                                 |\n");
  fprintf(fp, "\t|              SCHOOL OF ELECTRICAL ENGINEERING                                 |\n");
  printf("t|                   MASTER PROGRAM IN NETWORK SERVICES AND SYSTEMS              |\n");
  fprintf(fp, "\t|                   MASTER PROGRAM IN NETWORK SERVICES AND SYSTEMS              |\n");
  printf("\t|                       MANAGEMENT OF NETWORKS AND NETWORKED SYSTEMS - EP2300   |\n");
  fprintf(fp, "\t|                       MANAGEMENT OF NETWORKS AND NETWORKED SYSTEMS - EP2300   |\n");
  printf("\t*-------------------------------------------------------------------------------*\n");
  fprintf(fp, "\t*-------------------------------------------------------------------------------*\n");
  printf("\t|                                                                               |\n");
  fprintf(fp, "\t|                                                                               |\n");
  printf("\t|             COMPUTING HISTOGRAMS FOR ROUTER LOADS USING SNMP                  |\n");
  fprintf(fp, "\t|             COMPUTING HISTOGRAMS FOR ROUTER LOADS USING SNMP                  |\n");
  printf("\t|                                                                               |\n");
  fprintf(fp, "\t|                                                                               |\n");
  printf("\t*-------------------------------------------------------------------------------*\n\n");
  fprintf(fp, "\t*-------------------------------------------------------------------------------*\n\n");
  fprintf(fp, "\t\t%s\t\t%s\t\t%s\t\t%s\t\t%s\n\n", LD1,LD2, LD4,LD2, LD1);  
  printf("\t\t%s\t\t%s\t\t%s\t\t%s\t\t%s\n\n", LD1,LD2, LD3,LD2, LD1);    
}

//displays/prints the network topology file
void DisplayNetworkTopologyFile(){
 char buf[254];
 int count = 0;

  fp = (FILE*)create_file("r");

  if(fp){
     while(fgets(buf, 256, fp) != NULL){
	printf("%s", buf);
       if(++count%20 == 0){
         getchar();
	}
     }
  }
  close_file();
  menu2();
  
}


void generateErrorMessages(int reason){
    switch(reason){
      case 0: printf("\n\tThere are more than 6 input parameters\n\tAborting......!\n"); break;
      case 1: printf("\n\tThere are less  than 6 input parameters\n\tAborting.......!\n"); break;
      case 2: perror("\n\n\tError......\tInvalid sampling period\n"); break;
      case 3: perror("\n\n\tError......\tInvalid number of histogram intervals \n"); break;
      case 4: perror("\n\n\tSNMP username or passphrase error\n"); break;  
   }
  printf("\n\tExample: ./histogram 2 5 2G1332_student netmanagement 192.168.1.1\n");
  exit(1);
}

void help(){
    printf("\nThis Software was developed in order to help you to monitor the load in the network "
            " The software is also able to discover your network topology."
            " When discovering a network topology, it provides informations such as the Hostname, the host numer "
            " of interfaces, the host interfaces decription and finally the host routing table.\n");
    
    printf("In order to run the program the following input paramameters should be given in the command line: \n"
            " ./histogram <samplingTime> <number intervals in the hostogram> <SNMP username> <SNMP passphrase> <first node IP ADDR>\n"
            " The ./histogram is the program name. The sampling time is the intervall between a new polling is made to the network in order to"
            "provide new values for the load. The number of intervals in the histogram represents the Y-axis scale which represents the number of"
            " nodes that at givem time will have (X-axis)Kbps of load. ");
    printf(" The SNMP username is the authentication name because this software uses SNMPv3. The SNMP passphrase is the password that "
            "together with the username certfies if the user has access to the remote host. The IP address represents the starting point "
            "in the network.\n");
}

void menu2(){
 printf("\n\t*--------------------------------------------*\n");
   printf("\t|    OPTIONS     |     DESCRIPTION           |\n");
   printf("\t*--------------------------------------------*\n");
   printf("\t|       H,h      |     Print Help Message    |\n");
  printf("\t*--------------------------------------------*\n");
   printf("\t|       S,s      | Exit Menu Start Software  |\n");
   printf("\t*--------------------------------------------*\n");
   printf("\t|       O,op     |     Print Options Menu    |\n");
   printf("\t*--------------------------------------------*\n");
   printf("\t|       F, nf    |Opens The Network Top. File|\n");
   printf("\t*--------------------------------------------*\n");
   printf("\t|       Q, q     | Quit/Terminates Execution |\n");
   printf("\t*--------------------------------------------*\n");
}

void menu(){
    char opt[5];
    printf("\n\t-----------------------------*\n");
      printf("\t|**WELCOME TO THE HELP MENU**|\n");
      printf("\t-----------------------------*\n");
    
    menu2();
    
    while(1){
        memset(opt, (int)'\0', sizeof(opt));
        	 
        printf("\n\t-------------------\n\tSelect an Option:> ");
	 fgets(opt, 10, stdin);	 
        if( strcmp(opt,"h\n") == 0 || strcmp(opt, "H\n") == 0){
            help();
        }else if(strcmp(opt, "S\n")==0 || strcmp(opt, "s\n")==0){
            break;
        }else if( strcmp(opt,"O\n")==0 || strcmp(opt,"op\n")==0){
	    menu2();
	 }else if(strcmp(opt, "Q\n")== 0 || strcmp(opt, "q\n")== 0){
		printf("\tTerminating Program....\n");
		exit(1);
	 }else if(strcmp(opt, "F\n")== 0 || strcmp(opt, "nf\n") == 0){
	       DisplayNetworkTopologyFile();
	 }
	 else{
            printf("\tInvalid Option....\n");	
	     menu2();     
        }       
    }
}

//handles the interrupt signal. CTRL-C
void termination_handler ()
{
  signal(SIGINT, termination_handler); /* reset signal */
  menu(); 
}
