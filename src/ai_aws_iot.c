/*
 * Copyright © 2017 Astonish Inc.  All Rights Reserved.
 * 
 * Main source file for communicationg with AWS IoT.
 * 
 * Mostly taking from AWS example published by Amazon, modified as needed. 
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
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
    
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

#include "ai_aws_iot.h"

/**
 * @brief Default cert location
 */
char g_cert_directory[PATH_MAX + 1] = "/../../.aws";

/**
 * @brief Default MQTT HOST URL is pulled from the aws_iot_config.h
 */
char g_host_address[255] = AWS_IOT_MQTT_HOST;

/**
 * @brief Default MQTT port is pulled from the aws_iot_config.h
 */
uint32_t g_port = AWS_IOT_MQTT_PORT;

void subscribe_callback_handler (AWS_IoT_Client *p_client, char *p_topic_name, uint16_t topic_name_len, IoT_Publish_Message_Params *p_params, void *p_data) 
{
  IOT_UNUSED(p_data);
  IOT_UNUSED(p_client);
  IOT_INFO("Subscribe callback");
  IOT_INFO("%.*s\t%.*s", topic_name_len, p_topic_name, (int) p_params->payloadLen, (char*)(p_params->payload));
}

void disconnect_callback_handler (AWS_IoT_Client *p_client, void *p_data) 
{
  IOT_WARN("MQTT Disconnect");
  IoT_Error_t rc = FAILURE;

  if (NULL == p_client) 
  {
    return;
  }

  IOT_UNUSED(p_data);

  if (aws_iot_is_autoreconnect_enabled(p_client)) 
  {
    IOT_INFO("Auto Reconnect is enabled, Reconnecting attempt will start now");
  } 

  else 
  {
    IOT_WARN("Auto Reconnect not enabled. Starting manual reconnect...");
    rc = aws_iot_mqtt_attempt_reconnect(p_client);
    
    if (NETWORK_RECONNECTED == rc) 
    {
      IOT_WARN("Manual Reconnect Successful");
    } 
    else 
    {
      IOT_WARN("Manual Reconnect Failed - %d", rc);
    }
  }
}

//TODO: add ability to pass cert, root_CA and client ID names
void parse_input_args_for_connect_params(int argc, char **argv) 
{
  int opt;

  while(-1 != (opt = getopt(argc, argv, "h:p:c:x:")))  
  {
    switch(opt) 
    {
      case 'h':
        strcpy(g_host_address, optarg);
        IOT_DEBUG("Host %s", optarg);
        break;

      case 'p':
        g_port = atoi(optarg);
        IOT_DEBUG("arg %s", optarg);
        break;

      case 'c':
        strcpy(g_cert_directory, optarg);
        IOT_DEBUG("cert root directory %s", optarg);
        break;

/* left here for example, needs to be added to calling main program        
      case 'x':
        m_publish_count = atoi(optarg);
        IOT_DEBUG("publish %s times\n", optarg);
        break;
*/
      case '?':
        if(optopt == 'c') 
        {
          IOT_ERROR("Option -%c requires an argument.", optopt);
        } 
        
        else if(isprint(optopt)) 
        {
          IOT_WARN("Unknown option `-%c'.", optopt);
        } 
        
        else 
        {
          IOT_WARN("Unknown option character `\\x%x'.", optopt);
        }
        break;

      default:
        IOT_ERROR("Error in command line argument parsing");
        break;
    }
  }
}

IoT_Error_t init_mqtt (int argc, char **argv, AWS_IoT_Client * p_client, IoT_Client_Init_Params * p_mqtt_init_params)
{
  IoT_Error_t rc = FAILURE;

  char root_CA[PATH_MAX + 1];
  char client_CRT[PATH_MAX + 1];
  char client_key[PATH_MAX + 1];
  char current_working_dir[PATH_MAX + 1];

  parse_input_args_for_connect_params(argc, argv);

  getcwd(current_working_dir, sizeof(current_working_dir));
  snprintf(root_CA, PATH_MAX + 1, "%s/%s/%s", current_working_dir, g_cert_directory, AWS_IOT_ROOT_CA_FILENAME);
  snprintf(client_CRT, PATH_MAX + 1, "%s/%s/%s", current_working_dir, g_cert_directory, AWS_IOT_CERTIFICATE_FILENAME);
  snprintf(client_key, PATH_MAX + 1, "%s/%s/%s", current_working_dir, g_cert_directory, AWS_IOT_PRIVATE_KEY_FILENAME);

  IOT_DEBUG("root_CA %s", root_CA);
  IOT_DEBUG("client_CRT %s", client_CRT);
  IOT_DEBUG("client_key %s", client_key);
  
  p_mqtt_init_params->enableAutoReconnect = false; // We enable this later below
  p_mqtt_init_params->pHostURL = g_host_address;
  p_mqtt_init_params->port = g_port;
  p_mqtt_init_params->pRootCALocation = root_CA;
  p_mqtt_init_params->pDeviceCertLocation = client_CRT;
  p_mqtt_init_params->pDevicePrivateKeyLocation = client_key;
  p_mqtt_init_params->mqttCommandTimeout_ms = 20000;
  p_mqtt_init_params->tlsHandshakeTimeout_ms = 5000;
  p_mqtt_init_params->isSSLHostnameVerify = true;
  p_mqtt_init_params->disconnectHandler = disconnect_callback_handler;
  p_mqtt_init_params->disconnectHandlerData = NULL;

  rc = aws_iot_mqtt_init( p_client, p_mqtt_init_params);

  return rc;
}

IoT_Error_t mqtt_connect (AWS_IoT_Client * p_client, IoT_Client_Connect_Params * p_connect_params, const char * p_serial_number) 
{
  IoT_Error_t rc = FAILURE;

  p_connect_params->keepAliveIntervalInSec = 10;
  p_connect_params->isCleanSession = true;
  p_connect_params->MQTTVersion = MQTT_3_1_1;
  p_connect_params->pClientID = p_serial_number;
  p_connect_params->clientIDLen = (uint16_t) strlen(p_serial_number);
  p_connect_params->isWillMsgPresent = false;

  IOT_INFO("Connecting");

  rc = aws_iot_mqtt_connect(p_client, p_connect_params);

  return rc;
}

//TODO: do  not pass topic_name_len, use strlen to determine it
IoT_Error_t mqtt_publish (AWS_IoT_Client *p_client, QoS qos, const char *p_payload, const char *p_topic_name, uint16_t topic_name_len)
{
  IoT_Error_t rc = FAILURE;

  IoT_Publish_Message_Params params;
  
  params.qos = qos;
  params.payload = (void *) p_payload;
  params.isRetained = 0;
  params.payloadLen = strlen(p_payload);

  IOT_INFO("Publishing %s", p_topic_name);
  rc = aws_iot_mqtt_publish( p_client, p_topic_name, topic_name_len, &params);
                    
  return rc;
}

