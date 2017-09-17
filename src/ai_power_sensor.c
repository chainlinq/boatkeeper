/*
 * Copyright © 2017 Astonish Inc.  All Rights Reserved.
 * 
 * Source file for dummy holder power sensor. Used for testing the rest 
 * of the system 
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
#include "ai_power_sensor.h"

Power_Status_t g_power_status = power_undefined;

void toggle_shore_power_status ()
{
  if (g_power_status == power_on)
  {
    g_power_status = power_off;
  }
 
  // If power status is undefined or off, set it to on 
  else
  {
    g_power_status = power_on;
  }
}

Power_Status_t read_shore_power_status ()
{
  return g_power_status;
}

const char * read_shore_power_status_string ()
{
  return g_power_status_strings[g_power_status];
}

