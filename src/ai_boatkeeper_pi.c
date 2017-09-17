/*
 * Copyright © 2017 Astonish Inc.  All Rights Reserved.
 * 
 * Source file for Boat Keeper on a Raspberry Pi.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  https://github.com/chainlinq/boatkeeper/blob/master/LICENSE
 *
 * This file is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES 
 * OR CONDITIONS OF ANY KIND, either express or implied. See the License 
 * for the specific language governing permissions and limitations 
 * under the License.
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

#include "aws_iot_log.h"
#include "aws_iot_mqtt_client_interface.h"

#include "ai_aws_iot.h"
#include "ai_boatkeeper_pi.h"

char g_serial_number[SERIAL_NUMBER_LENGTH] = DUMMY_SERIAL_NUMBER;

const char * read_serial_number ()
{
  FILE * p_file;

  p_file = fopen( RASPBERRYPI_SERIAL_NUMBER_FILENAME, "r");
          
  if (p_file == NULL) // if did not open, alert and use the default number
  {
    IOT_ERROR("Failed to open serial number file - %s", RASPBERRYPI_SERIAL_NUMBER_FILENAME);
  }
            
  else
  {
    char line_buffer [1024];
    char * number_line;
    size_t bytes_read;
    bool keep_reading = true; 

    do
    {
      bytes_read = fread( line_buffer, 1, sizeof (line_buffer)-1, p_file);

      if (bytes_read == 0)
      {
        IOT_ERROR("Failed to read serial number file - %s", RASPBERRYPI_SERIAL_NUMBER_FILENAME);
        keep_reading = false;
      }

      else
      {
        number_line = strstr(line_buffer, "Serial");
        
        if (number_line != NULL)
        {
          char * number;

          // Line is something like "Serial   :  999999"
          // To get the number find ':' and work towards the number
          number = strchr( number_line, ':');

          if (number == NULL)
          {
            IOT_ERROR("Failed to find serial number deliminater ':'");
            keep_reading = false;
          }

          else
          {
            // move off ':'
            number++;

            //strip leading white space
            while ( isspace((unsigned char)*number))
              number++;

            // replace CRLF (if any) with NULL
            number[strcspn(number, "\r\n")] = 0;

            // copy number into g_serial_number
            strcpy ( g_serial_number, number);

            keep_reading = false;
          }
        }
      }
    } while (keep_reading);
  }

  IOT_INFO("Serial Number - %s", g_serial_number);
  return g_serial_number;
}

IoT_Error_t publish_shore_power_status (AWS_IoT_Client *p_client, QoS qos)
{
  IoT_Error_t result = FAILURE;
     
  char payload[100];
  
  const char * p_shore_power_status_string = read_shore_power_status_string();
             
  time_t raw_time;
  
  time(&raw_time);
  
  sprintf( payload, "{ \"serial_number\": \"%s\", \"shore_power\": \"%s\", \"timestamp\":\"%ld\" }", g_serial_number, p_shore_power_status_string, raw_time); 
   
  result = mqtt_publish ( p_client, qos, payload, AI_BOATKEEPER_STATUS_TOPIC);
  
  return result;
}


