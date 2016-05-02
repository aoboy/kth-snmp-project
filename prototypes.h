/*KTH - Royal Institute of Technology - Stockholm
 * School of Electrical Engineering
 *   Master Program in Network Services and Systems
 *     Management of Networks and Networked Systems
 *   
 */
/* 
 * File:   prototypes.h
 * Author: ant�nio gonga
 *
 * Created on 19 de Setembro de 2008, 12:51
 */

#ifndef _PROTOTYPES_H
#define _PROTOTYPES_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "headers.h"

  ipRouteTable_t* createIPRouteTable();
  
  ipAddrTable_t* createIPAddrTable();
    
  Host_t* createHostStructure();
  
  Host_t* createHost(string host_ip);
  
  ipRouteTable_t* createIpRouteEntry(int index, string ip_addr);
  
  void setIpAddrTableIfIndex(Host_t *, int index, int ifIndex);
  
  void setIpRTableIfIndex(Host_t *, int index, int ifIndex);
  
  ipAddrTable_t* createIfEntry(int index, string ip_addr);
  
  int hostIPExists(string host_ip);

  int hostIpAddrEntryIsNotIfEntryOfExistingHost(string ip_addr);
  
  int ipRouteEntryExists(ipRouteTable_t *entry, string ip_addr);
  
  int ifTableEntryExists(ipAddrTable_t *entry, string ip_addr);
  
  ipRouteTable_t *removeIpRouteEntry(ipRouteTable_t *entry);
  
  ipAddrTable_t* removeIpAddrEntry(ipAddrTable_t * entry);
  
  void removeIpRouteTableEntries(ipRouteTable_t *entry);
  
  void removeIfTableEntries(ipAddrTable_t *entry);
    
  Host_t* Remove(Host_t *host);
          
  void RemoveHost(Host_t *hostListBase, string host_ip);
  
  void AddHost(Host_t *hostListBase, string ip_addr);
  
  void AddIfTableEntryToHost(Host_t *host, string ip_addr, int index);
  
  void AddRouteTableEntryToHost(Host_t *host, string ip_addr, int Index );
  
  void PrintHostIfTableEntries(ipAddrTable_t *entry);
  
  void PrintHostIpRouteTableEntries(ipRouteTable_t * entry);
  
  void printfInitMessage();

  void PrintHosts();
   
  void generateErrorMessages(int reason);

  FILE *create_file(string mode);

  void DisplayNetworkTopologyFile();

  void generateErrorMessages(int reason);

  void menu();

  void help();

  void menu2();

  void termination_handler ();
  
  /**
   *
   *
   */
  void parseInputArgs(int argc, char **argv);
  
  
  void mainLoop(string host_name, string host_ip, string host_pass, int deltaSamples, int nIntervals);
  
  /*netsnmp_pdu *create_snmp_pdu_type(Host_t *host, struct my_oids_t *op);*/
  

  
  //void getNetworkTopology();

  void getNetworkTopology(Host_t *host);

  void getHostsLoad();
  void compute_load();
  void init_load_array();
  void generate_histogram();
  /**
   *
   **/
   /*void initialize_oids(struct my_oids_t *oids);*/

   void createSNMPv3Session(Host_t *host);

   void setRemoteHostSystemName(netsnmp_pdu *response, Host_t *host);

   void setRemoteHostIfNumber(netsnmp_pdu *response, Host_t *host);
   
   void getRemoteHostIpRTableIPs(Host_t *host, netsnmp_pdu *response);

   void getRemoteHostIpRTableIfIndexs(Host_t *host, netsnmp_pdu *response);

   void getRemoteHostIpAddrTableIPs(Host_t *host, netsnmp_pdu *response);

   void getRemoteHostIpAddrTableIfIndexs(Host_t *host, netsnmp_pdu *response);

   void getRemoteHostOutOctets(Host_t *host, netsnmp_pdu *response);

   void getRemoteHostInOctets(Host_t *host, netsnmp_pdu *response);

   void getRemoteHostIfSpeed(Host_t *host, netsnmp_pdu *response);
    
  
  
#ifdef	__cplusplus
}
#endif

#endif	/* _PROTOTYPES_H */

