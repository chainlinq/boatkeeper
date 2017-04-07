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
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#ifndef AI_AWS_IOT_H_
#define AI_AWS_IOT_H_

void init_mqtt_params (int argc, char **argv, IoT_Client_Init_Params * p_mqtt_init_params);
void init_client_connect_params (IoT_Client_Connect_Params * p_connect_params);


#endif /* AI_AWS_IOT_H_ */
