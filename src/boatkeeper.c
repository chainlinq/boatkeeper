/*
 * Copyright © 2017 Astonish Inc.  All Rights Reserved.
 * 
 * Main source file for Boat Keeper.
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

/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>

#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

#include "ai_aws_iot.h"
#include "ai_boatkeeper_pi.h"

/**
 * This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
 */
//TODO: find a way to pass this with argv and still set the argv's in ai_aws_iot.c
uint32_t g_publish_count = 2;

int main(int argc, char **argv) 
{
  bool infinite_publish_flag = true;

  int32_t i = 0;

  IoT_Error_t rc = FAILURE;

  IOT_INFO("\nAWS IoT SDK Version %d.%d.%d-%s\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH, VERSION_TAG);

  const char * p_serial_number = read_serial_number();

  AWS_IoT_Client client;
  IoT_Client_Init_Params mqtt_init_params = iotClientInitParamsDefault;

  rc = init_mqtt(argc, argv, &client, &mqtt_init_params);

  if (SUCCESS != rc) 
  {
    IOT_ERROR("init_mqtt returned error : %d ", rc);
    return rc;
  }

  IOT_INFO("Connecting...");
  IoT_Client_Connect_Params connect_params = iotClientConnectParamsDefault;

  rc = mqtt_connect( &client, &connect_params, p_serial_number);
  
  if (SUCCESS != rc) 
  {
    IOT_ERROR("Error(%d) connecting to %s:%d", rc, mqtt_init_params.pHostURL, mqtt_init_params.port);
    return rc;
  }

  /*
  * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
  *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
  *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
  */
  rc = aws_iot_mqtt_autoreconnect_set_status(&client, true);

  if (SUCCESS != rc) 
  {
    IOT_ERROR("Unable to set Auto Reconnect to true - %d", rc);
    return rc;
  }

  /*
  IOT_INFO("Subscribing...");
  rc = aws_iot_mqtt_subscribe(&client, "sdkTest/sub", 11, QOS0, subscribe_callback_handler, NULL);
  if(SUCCESS != rc) {
  IOT_ERROR("Error subscribing : %d ", rc);
  return rc;
  }
  */


  if (g_publish_count != 0) 
  {
    infinite_publish_flag = false;
  }

  while((NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc) && (g_publish_count > 0 || infinite_publish_flag)) 
  {
    //Max time the yield function will wait for read messages
    rc = aws_iot_mqtt_yield(&client, 100);
    
    if(NETWORK_ATTEMPTING_RECONNECT == rc) 
    {
      // If the client is attempting to reconnect we will skip the rest of the loop.
      continue;
    }

    IOT_INFO("-->sleep");
    sleep(10);

    rc = publish_shore_power_status ( &client, QOS1);

    if (rc == MQTT_REQUEST_TIMEOUT_ERROR) 
    {
      IOT_WARN("QOS1 publish ack not received.\n");
      rc = SUCCESS;
    }

    if(g_publish_count > 0) 
    {
      g_publish_count--;
    }
  }

  if(SUCCESS != rc) 
  {
    IOT_ERROR("An error occurred in the loop.\n");
  } 
  
  else 
  {
    IOT_INFO("Publish done\n");
  }

  return rc;
}
