/*
 * Copyright © 2017 Astonish Inc.  All Rights Reserved.
 * 
 * Main include file for communicationg with AWS IoT.
 * 
 * Mostly taken from AWS example published by Amazon, modified as needed. 
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
#ifndef AI_AWS_IOT_H_
#define AI_AWS_IOT_H_

void subscribe_callback_handler (AWS_IoT_Client * p_client, char * p_topic_name, uint16_t topic_name_len, IoT_Publish_Message_Params * p_params, void * p_data);
void disconnect_callback_handler (AWS_IoT_Client * p_client, void * p_data); 
void parse_input_args_for_connect_params(int argc, char **argv); 

IoT_Error_t init_mqtt (int argc, char **argv, AWS_IoT_Client * p_client, IoT_Client_Init_Params * p_mqtt_init_params);
IoT_Error_t mqtt_connect (AWS_IoT_Client * p_client, IoT_Client_Connect_Params * p_connect_params, const char * p_serial_number); 
IoT_Error_t mqtt_publish (AWS_IoT_Client *p_client, QoS qos, const char *p_payload, const char *p_topic_name);

#endif /* AI_AWS_IOT_H_ */
