/*KTH - Royal Institute of Technology - Stockholm
 * School of Electrical Engineering
 *   Master Program in Network Services and Systems
 *     Management of Networks and Networked Systems
 * 
 * Lecturer : Prof. Rolf Stadler/Alberto Gonzalez
 */
/* 
 * File:   histogram.c
 * Author: antonio gonga
 * 831223-A635
 * Created on 19 de Setembro de 2008, 13:59
 */

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

#include "prototypes.h"

/**
 * Parse the input parameters
 * argc - number of input parameters entered
 * argv - the array of string that contains the input paramaters
 * returns - void
 */
void parseInputArgs(int argc, char **argv){ 
    int intSamples;
    int intHistogram;    
    
    if(argc > 6){
        generateErrorMessages(0);
    }else if(argc < 6){
        generateErrorMessages(1);
    }else{        
        intSamples = atoi(argv[1]);
	 if(intSamples == 0){
		generateErrorMessages(2);
	 }

        printf("\n\tExtracting sampling interval: %2d\n", intSamples);
        intHistogram = atoi(argv[2]);
	 if(intHistogram <= 0){
		generateErrorMessages(3);
	 }
	 printf("\tExtracting histogram intervals: %2d\n\n",intHistogram);
 
        if((strcmp(argv[3], "2G1332_student") != 0) || (strcmp(argv[4], "netmanagement") != 0)){
		generateErrorMessages(4);
	 }
	  
        printf("\n\n\tStarting Node SNMP Info:\n\tIP Addr: %s\n", argv[5]);
        printf("\tSNMP security userame: %s\n", argv[3]);
        printf("\tSNMP security passphrase: %s\n", argv[4]);

	 printf("\n\n\tInput Parameters verification OK ......\n");                       

	 menu();

        printf("\n\n\tStarting Program...............\n");
        sleep(3);  //pause a bit for 3 seconds
        mainLoop(argv[3], argv[5], argv[4], intSamples, intHistogram);        
    }
       
}

/*
 * Main Program invocation fucntion. In any C program, this is the first function to 
 * be invoked when the program starts
 */
int main(int argc, char** argv) {
        
    signal(SIGINT, termination_handler); //handle the interrupt signal CTRL-C

    hostsList = createHostStructure(); /*Create the HostsList Head*/    
         
    parseInputArgs(argc, argv);     /*parse input parameters*/      

    return 0;
}


