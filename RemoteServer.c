//----------------------------------------------------------------------------//
//  Application: RemoteServer for AXIOM Cameras                               //
//                                                                            //
//  Author:   Phil Kerr                                                       //
//  Company:  Plus24 Systems Ltd.                                             //
//  GitHub:   https://github.com/Plus24Systems/RemoteServer                   //
//  Copyright (C) 2016 - 2017 Phil Kerr - Plus24 Systems Ltd.                 //
//                                                                            //
//   This program is free software: you can redistribute it and/or modify     //
//   it under the terms of the GNU General Public License as published by     //
//   the Free Software Foundation, either version 3 of the License, or        //
//   (at your option) any later version.                                      //
//                                                                            //
//   This program is distributed in the hope that it will be useful,          //
//   but WITHOUT ANY WARRANTY; without even the implied warranty of           //
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            //
//   GNU General Public License for more details.                             //
//                                                                            //
//   You should have received a copy of the GNU General Public License        //
//   along with this program.  If not, see <http://www.gnu.org/licenses/>.    //
//                                                                            //
//                                                                            //
//----------------------------------------------------------------------------//

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <pthread.h>
#include <errno.h>
//----------------------------------------------------------------------------//
// Defines which end with a _REQUEST are Signal Messages 
// received from the RemotePlus.
//
// Defines which end with an _RESPONSE are Response Messages 
// sent back to the RemotePlus.

// Values from 0x01 to 0x7f are for _REQUEST messages, values 
// from 0x80 to 0xff are for _RESPONSE messages.

// This section MUST be replicated exactly in RemotePlusDefines.h

#define GET_AXIOM_TEMP_REQUEST 0x01
#define GET_AXIOM_TEMP_RESPONSE 0x80

#define GET_AXIOM_STATUS_REQUEST 0x02
#define GET_AXIOM_STATUS_RESPONSE 0x81

#define GET_GAIN_REQUEST 0x03
#define GET_GAIN_RESPONSE 0x82

#define SET_GAIN_REQUEST 0x04
#define SET_EXPOSURE_REQUEST 0x05
#define SET_VIDEO_MODE_REQUEST 0x06
#define SET_OVERLAYS_REQUEST 0x07
#define SET_HDMI_LIVE_STREAM_REQUEST 0x08

// Used for development, can be commented out if not required.
#define DEBUG 1

//----------------------------------------------------------------------------//
// Globals

int uart0 = -1;	
FILE *fp;
char shellOutput [1000];

pthread_t uartTXThread;
pthread_t getFPSThread;

// Flag to see if kick_manual.sh has been run (required for some scripts)
int cameraRunning = 0; 

void uart_tx (int msgType, unsigned char* data);

//----------------------------------------------------------------------------//

void uart_init () 
{
	uart0 = open ("/dev/ttyPS0", O_RDWR | O_NOCTTY | O_NDELAY);		 	 
	
	if (uart0 == -1) {
	    #if DEBUG
		printf ("Error - Unable to open UART.\n");
		#endif
	}

    // Set UART params
    struct termios options;
    
    tcgetattr (uart0, &options);
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;		 
    options.c_iflag = IGNPAR;
    options.c_iflag = ICRNL;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush (uart0, TCIFLUSH);
    tcsetattr (uart0, TCSANOW, &options);
}

//----------------------------------------------------------------------------//
// Placeholder for the get_fps.sh script, will be implemented in the future.

void getFPS (void) 
{
    if (cameraRunning == 1) {
	    #if DEBUG
        printf ("get FPS\n");
        #endif     
    }
}
//----------------------------------------------------------------------------//

void uart_tx (int msgType, unsigned char* data) 
{
    int datalen, loop;
        
    datalen = strlen (data);

    unsigned char message [datalen + 4];
    
    message [0] = msgType;
    
    #if DEBUG
    if (message [0] == GET_AXIOM_TEMP_RESPONSE) printf ("Temp Msg response\n");
    if (message [0] == GET_AXIOM_STATUS_RESPONSE) printf ("AxiomSys Msg response\n");
    if (message [0] == GET_GAIN_RESPONSE) printf ("Get Gain Msg response\n");    
    #endif
    
    for (loop = 1 ; loop <= datalen + 1 ; loop++) 
        message [loop] = data [loop - 1];
        
    if (uart0 != -1) { 
        int count = write (uart0, message, (datalen + 3));      

        if (count < 0) {
            #if DEBUG
            printf ("UART TX error\n");
            #endif
            }
        }
}

//----------------------------------------------------------------------------//

int main (void)
{
    int status;
    status = 0;
   
    int uartTXThreadstatus;
    uartTXThreadstatus = -1;
 
    int getFPSThreadstatus;
    getFPSThreadstatus = -1;
 
    // UART init
    uart_init ();
    
    #if DEBUG      
    printf ("UART INIT Done\n");
    #endif        

    unsigned char rx_buffer [4096];

    // Create UART transmit thread and check status
    uartTXThreadstatus = pthread_create (&uartTXThread, NULL, uart_tx, NULL);

    if (uartTXThreadstatus == -1) {
        fprintf (stderr, "Error: unable to create uart_tx thread.\n");
    } 

    // Create get FPS thread and check status
    getFPSThreadstatus = pthread_create (&getFPSThread, NULL, getFPS, NULL);

    if (getFPSThreadstatus == -1) {
        fprintf (stderr, "Error: unable to create getFPS thread.\n");
    } 

    while (1) {  
    	if (uart0 != -1) {
		
    	    int rx_length = read (uart0, (void*) rx_buffer, 255);
		
    		if (rx_length < 0) {
    			// An error occured (will occur if there are no bytes)
    		}
    		else if (rx_length == 0) {
    			// No data - waiting
    		} else {
    			// Bytes received
    			rx_buffer [rx_length] = '\0';
    			
                #if DEBUG    			
                printf ("%i bytes read : (hex = %X)\n", rx_length, rx_buffer [0]);

                if (rx_buffer [0] == GET_AXIOM_TEMP_REQUEST) printf ("Temp Msg request\n");
                if (rx_buffer [0] == GET_AXIOM_STATUS_REQUEST) printf ("AxiomSys Msg request\n"); 
                if (rx_buffer [0] == SET_EXPOSURE_REQUEST) printf ("Set Exposure request\n");  
                #endif
    		}
    		
		    /////////////////////////////////////////////////////////		
		    // Start Camera
            if (rx_buffer [0] == GET_AXIOM_STATUS_REQUEST) { 
            
                #if DEBUG
                printf ("Starting AXIOM Firmware\n");
                #endif
                
                if (cameraRunning == 0) {
                    pid_t pid = fork ();

                    if (pid == -1) {
                    // error, failed to fork ()
                    } else if (pid > 0) {
                        int status;
                        waitpid (pid, &status, 0);
                    } else {
                        // Send two messages with a 100ms delay in between
    			        usleep (10000);                           
                        uart_tx (GET_AXIOM_STATUS_RESPONSE, "STARTING");
    			        usleep (10000);      			                    
                        uart_tx (GET_AXIOM_STATUS_RESPONSE, "STARTING");                    
                              
                        // This is the main call to start the AXIOM Firmware
                             
                        // As there's been an OS update to the AXIOM Camera that automatically starts this it's disabled
                        // by default here as starting the firmware more than once may cause issues or damage.  
                        // If you have an AXIOM Beta where you have to manually start the firmware
                        // uncomment the following line out.
                         
                        status = execv ("/root/kick_manual.sh", NULL);
                        
                        // And comment out the following line if the above is uncommented.
                        //status = 0;

                        sleep (4);
                        _exit (EXIT_FAILURE);
                    }
                }

			    if (status == -1) {
                    #if DEBUG
			        printf ("Failed to run GET_AXIOM_STATUS_REQUEST command\n" );
			        #endif
			        
			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR");			        
			    } else { 
			        if (cameraRunning == 0) {  // Checks if the AXIOM Firmware is running, if not sets the flag.
                        cameraRunning = 1;
                        
                        #if DEBUG
                        printf ("RUNNING\n");                        
                        #endif
                        
    			        usleep (20000);
    			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "RUNNING");    			        
                    } else { // The AXIOM Firmware is running
                        #if DEBUG
                        printf ("RUNNING\n");  
                        #endif
                        
    			        usleep (20000);
    			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "RUNNING");      			                         
                    }
			    }
		    } ///////////////////////////////////////////////////////	    		

		    /////////////////////////////////////////////////////////
            // Get Sensor Temp - zynq_info.sh
            if (rx_buffer [0] == GET_AXIOM_TEMP_REQUEST) {  
			    fp = popen ("/root/get_temp.sh -s", "r");  
			    
			    if (fp == NULL) {
                    #if DEBUG
			        printf ("Failed to run get_temp.sh command\n" );
                    #endif
                    
                    uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR"); // We send back an AXIOM Status Error                    
			    } else {
    		        while (fgets (shellOutput, sizeof (shellOutput) - 1, fp) != NULL) {
                        #if DEBUG
			            printf ("%s", shellOutput);
                        #endif

                        // Send Temp response back
			            uart_tx (GET_AXIOM_TEMP_RESPONSE, shellOutput);
			        }
		        }	
		        pclose (fp);	
		    } ///////////////////////////////////////////////////////	

		    /////////////////////////////////////////////////////////
            // Get Gain
            if (rx_buffer [0] == GET_GAIN_REQUEST) {
                #if DEBUG
                printf ("Get Gain\n");
                #endif
                
			    if (cameraRunning == 1) {
    			    fp = popen ("/root/get_gain.sh", "r");

    			    if (fp == NULL) {
                        #if DEBUG
    			        printf ("Failed to run get_gain.sh command\n" );
    			        #endif
			        
    			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR"); // We send back an AXIOM Status Error
    			    } else {
        		        while (fgets (shellOutput, sizeof (shellOutput) -1, fp) != NULL) {
                            #if DEBUG
                            printf ("Get Gain: %s\n", shellOutput);
                            #endif
                        
    			            uart_tx (GET_GAIN_RESPONSE, shellOutput); // We send back the gain setting
    			        }
    		        }	
    		        pclose (fp);
                } else 
                    printf ("AXIOM not running - get_gain command not executed\n");
	
		    } ///////////////////////////////////////////////////////	


		    /////////////////////////////////////////////////////////
            // Get FPS - not currently implemented
            if (rx_buffer [0] == 0x73) { 

	            if (cameraRunning == 1) {
        	        //getFPS ();
                } else {
      			    uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR"); // We send back an AXIOM Status Error
                }
			    
		    } ///////////////////////////////////////////////////////	

		    
		    /////////////////////////////////////////////////////////
            // Set Gain
            if (rx_buffer [0] == SET_GAIN_REQUEST) {
                #if DEBUG            
                printf ("Set Gain %d\n", rx_buffer [1]);
                #endif
                
                char *strings[5] = { "1", "2", "3/3", "3", "4"};
                
			    if (cameraRunning == 1) {            
                
                    if (rx_buffer [1] == 0)
		                fp = popen ("/root/set_gain.sh 1", "w");    
	                
                    else if (rx_buffer [1] == 1)
		                fp = popen ("/root/set_gain.sh 3/3", "w"); 

                    else if (rx_buffer [1] == 2)
		                fp = popen ("/root/set_gain.sh 2", "w"); 

                    else if (rx_buffer [1] == 3)
		                fp = popen ("/root/set_gain.sh 3", "w"); 

                    else if (rx_buffer [1] == 4)
		                fp = popen ("/root/set_gain.sh 4", "w"); 	                            
                } else {
                    #if DEBUG
                    printf ("AXIOM not running - set_gain command not executed\n");
                    #endif                
			    }
			    
			    if (fp == NULL) {
                    #if DEBUG			    
			        printf ("Failed to run set_gain.sh command\n" );
			        #endif
			        
			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR"); // We send back an AXIOM Status Error
			    } else {
                    // Do nothing for now, a return message will be added later.
		        }	
		        pclose (fp);	
		    } ///////////////////////////////////////////////////////			    
		    		    
		    /////////////////////////////////////////////////////////
		    // Set Exposure
            if (rx_buffer [0] == SET_EXPOSURE_REQUEST) {
                #if DEBUG
                printf ("Set Exposure %d\n", rx_buffer [1]);
                #endif

                int set_exposure_status;                
                
			    if (cameraRunning == 1) {                
                    pid_t pid = fork ();

                    if (pid == -1) {
                        // error, failed to fork()
                   } else if (pid > 0) {
                        waitpid (pid, &status, 0);
                    } else {
                        char* set_exposure_arg [2];                 
                   
                        // Build up the args for the command
                        set_exposure_arg [0] = "/root/set_exposure.sh";
                   
                        if (rx_buffer [1] == 0)
                            set_exposure_arg [1] = "0";

                        if (rx_buffer [1] == 1)
                            set_exposure_arg [1] = "1";

                        if (rx_buffer [1] == 2)
                            set_exposure_arg [1] = "2";

                        if (rx_buffer [1] == 3)
                            set_exposure_arg [1] = "3";

                        if (rx_buffer [1] == 4)
                            set_exposure_arg [1] = "4";

                        if (rx_buffer [1] == 5)
                            set_exposure_arg [1] = "5";

                        if (rx_buffer [1] == 6)
                            set_exposure_arg [1] = "6";

                        if (rx_buffer [1] == 7)
                            set_exposure_arg [1] = "7";

                        set_exposure_arg [2] = NULL;

                        // Execute the command
                        set_exposure_status = execv (set_exposure_arg [0], (char * const*) set_exposure_arg);
                        exit (EXIT_FAILURE);                        
                    }
                }

			    if (set_exposure_status == -1) {
                    #if DEBUG			    
			        printf ("Failed to run set_exposure.sh command\n" );
			        #endif
			        
			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR"); // We send back an AXIOM Status Error
			    } else {
			        if (cameraRunning == 1) {
                        #if DEBUG
                        printf ("Set Exposure Done\n");
	    		        #endif
	    		        
	    		        // A returm message will be added later.
                    } else {
    			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR"); // We send back an AXIOM Status Error
                    }
			    }
		    } ///////////////////////////////////////////////////////	

		    /////////////////////////////////////////////////////////
		    // Set HDMI Live Stream
            if (rx_buffer [0] == SET_HDMI_LIVE_STREAM_REQUEST) {
                #if DEBUG            
                printf ("Set HDMI Live Stream %d\n", rx_buffer [1]);
                #endif    
                
                int set_hdmi_status;                  
                
			    if (cameraRunning == 1) {                
                    pid_t pid = fork ();

                    if (pid == -1) {
                        // error, failed to fork ()
                   } else if (pid > 0) {
                        waitpid (pid, &status, 0);
                    } else {
                        char* set_hdmi_arg [2];                 
                   
                        // Build up the args for the command
                        set_hdmi_arg [0] = "/root/set_hdmi_live_stream.sh";

                        if (rx_buffer [1] == 0)
                            set_hdmi_arg [1] = "ON";

                        if (rx_buffer [1] == 1)
                            set_hdmi_arg [1] = "OFF";

                        set_hdmi_arg [2] = NULL;

                        // Execute the command
                        set_hdmi_status = execv (set_hdmi_arg [0], (char * const*) set_hdmi_status);
                        exit (EXIT_FAILURE);                        
                    }
                }

			    if (set_hdmi_status == -1) {
                    #if DEBUG   			    
			        printf ("Failed to run set_hdmi_live_stream.sh command\n" );
                    #endif

			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR"); // We send back an AXIOM Status Error

			    } else {
			        if (cameraRunning == 1) {
                        #if DEBUG
                        printf ("Set HDMI Live Stream Done\n"); 
                        #endif
                        
	    		        // A returm message will be added later.
                    } else {
                        #if DEBUG
                        printf ("Set HDMI Live Stream Not Done\n");                    
                        #endif

    			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR"); // We send back an AXIOM Status Error
                    }
			    }
		    } ///////////////////////////////////////////////////////	

		    /////////////////////////////////////////////////////////
		    // Set Overlays
            if (rx_buffer [0] == SET_OVERLAYS_REQUEST) {
                #if DEBUG
                printf ("Set Overlays %d\n", rx_buffer [1]);
                #endif

                int set_overlays_status;
                
			    if (cameraRunning == 1) {                
                    pid_t pid = fork ();

                    if (pid == -1) {
                        // error, failed to fork ()
                   } else if (pid > 0) {
                       waitpid (pid, &status, 0);
                    } else {

                        char* set_overlays_arg [2];   
                        
                        // Build up the args for the command                        
                        set_overlays_arg [0] = "/root/set_overlays.sh";
                        
                        if (rx_buffer [1] == 1)
                            set_overlays_arg [1] = "ON";

                        if (rx_buffer [1] == 0)
                            set_overlays_arg [1] = "OFF";

                        if (rx_buffer [1] == 2)
                            set_overlays_arg [1] = "CL";

                        set_overlays_arg [2] = NULL;                            

                        // Execute the command
                        set_overlays_status = execv (set_overlays_arg [0], (char * const*) set_overlays_arg);
                        exit (EXIT_FAILURE);                        
                    }
                }

			    if (set_overlays_status == -1) {
                    #if DEBUG			    
			        printf ("Failed to run set_overlays.sh command\n" );
                    #endif
                    
			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR"); // We send back an AXIOM Status Error
			    } else {
			        if (cameraRunning == 1) {
                        #if DEBUG			        
                        printf ("Set Overlays Done\n");  
                        #endif
                        
	    		        // A returm message will be added later.
                    } else {
                        #if DEBUG                    
                        printf ("Set Overlays Error\n");                    
    			        #endif
    			        
    			        uart_tx (GET_AXIOM_STATUS_RESPONSE, "ERROR"); // We send back an AXIOM Status Error
                    }
			    }
		    } ///////////////////////////////////////////////////////	

		    /////////////////////////////////////////////////////////
            // Set Video Mode
            if (rx_buffer [0] == SET_VIDEO_MODE_REQUEST) {
                #if DEBUG
                printf ("Set Video Mode (disabled for now) %d\n", rx_buffer [1]);
                #endif
            }

        // NULL the rx_buffer		
		rx_buffer [0] = 0;

    	}       
    }

   return 0;
}

//----------------------------------------------------------------------------//    
