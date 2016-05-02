/*KTH - Royal Institute of Technology - Stockholm
 * School of Electrical Engineering
 *   Master Program in Network Services and Systems
 *     Management of Networks and Networked Systems
 *   
 */
/* 
 * File:   headers.h
 * Author: ant�nio gonga
 *
 * Created on 25 de Setembro de 2008, 19:15
 */

#ifndef _HEADERS_H
#define _HEADERS_H


/*#ifdef	__cplusplus
extern "C" {
#endif*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <signal.h>



#define MAX_IP_ADDR_LEN     17    /*Ex: 192.168.254.250*/

#define GET_BLK_MAX_REPEAT  30

#define MAX_ULONG           4294967295

#define N_XBARS   26


/*#define */
#define EXISTS  1

#define strEquals(A , B) strcmp(A , B) == 0 ? EXISTS : 0
#define intEquals(A , B) (A) == (B) ?  EXISTS : 0


#define  SEP_BAR      "\t*---------------------------------------------------------------------------*\n"
#define  SEP_DIV_BAR  "------------------------------------------------------------------------------------\n"

#define  LD1           "\t*---------------------------------------------*\n"
#define  LD2           "\t|                                             |\n"
#define  LD3           "\t|            NETWORK LOAD HISTOGRAM           |\n"
#define  LD4           "\t|               NETWORK TOPOLOGY              |\n"
      
    
            
    typedef char* string;            	//typed definition of char*
  
    string        host_uname;        	//pointer to the host_name
    string        host_passphrase;   	//pointer to the hosts_snmp_passphrase
    string        host_snmp_name;    	//pointer to the hosts_snmp_name
    int           delta_samples;     	//global variable to the sampling interval
    int           hist_intervals;    	//global variable to the histogram intervals
    int           init_ld;           	//    
    float         load_array[N_XBARS];
    int           hist_array[N_XBARS];
  
    /*
     * IpRouteTable - defines the Ip Routing Table structure     
     */
    typedef struct _ipRouteTable_t{
        int index;                       //index 
        int ifIndex;                     //ifIndex of the IP in the Neighbor Router
        char ip_addr[MAX_IP_ADDR_LEN];   // the neighbor IP Address
        struct _ipRouteTable_t *next;    //pointer to the next element
    }ipRouteTable_t;

    /**
     *IPAddrTable - defines the Ip Addressing Table (Interfaces Table)
     */
    typedef struct _ipAddrTable_t{
        int ifIndex;                      //interface index/number
        string ifDesc;                    //interface description
        unsigned long ifSpeed;            //interface speed
        char if_ip_addr[MAX_IP_ADDR_LEN]; //interface IP Address
        struct _ipAddrTable_t *_next;     //pointer to the next element
    }ipAddrTable_t;
    
    /**
     * Load_t defines the load for a Host
     */
    typedef struct load_t{
        unsigned long inOctets0;     //Total Octets received  in time Ti
        unsigned long inOctets1;     //Total Octets received in time Ti+1
        float load_val;      // load value
    }load_t;
        
    /**
     * Host_t - defines the structure for each host/node in the network.
     * It has a pointer to the routing table, a pointer to the Ip Addr table
     * and a pointer to the next element. 
     * Define the host/node's number of interfaces, node IP addr, node SNMP name
     * node SNMP passphrase, and a pointer to the node load. 
     */
    typedef struct Host_t{
        int             ifNumber;        //total number of interfaces
        string          host_name;       // host/node name
        string          host_snmpName;   //node SNMP_V3 name
        string          host_pwrd;       //node SNMP_V3 passphrase
        char            host_ip_addr[MAX_IP_ADDR_LEN];  //one of the node's interface IP addr
        ipRouteTable_t *routeTable;      // node routing table
        ipAddrTable_t *ifTable;          // node IP interfaces Table
        load_t          *load;           // node load
        struct Host_t  *next_host;       // next element 
    }Host_t;             
    
    
    Host_t* hostsList;     //global variable that defines the hosts/nodes list
    
    netsnmp_session session, *ss;   //global variable that declares the snmp session 
   
#define NULLOID          0  // NULL - defines end of OID
#define IPRTBL_NXTHOP    1  // identifies the Routing table Oid type
#define IPRTBL_IFIDX     2  // identifies the RTable interface Index Oid type
#define SYSTEM_IFNBR     3  // identifies the node's number of interfaces Oid type
#define SYSTEM_NAME      4  // identifies node's name Oid type  
#define IPADDRTBL_IP     5  // IP Addr table Oid Type
#define IPADDRTBL_IF     6  // IP Addr table interfaces's Oid type
#define IPADDRTBL_IFDESC 7  // IP Addr table interface description oid type
#define IFINOCTETS       8  // IP Addr table octets received Oid Type
#define IFOUTOCTETS      9  // IP Addr Table octets sent out Oid type
#define IFSPEED          10 // IP Addr Table interfaces speed oid type 
      

FILE *fp;          //output file - File that will contains the network topology

#ifdef	__cplusplus
}
#endif

#endif	/* _HEADERS_H */

