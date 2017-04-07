/*
 * Copyright © 2017 Astonish Inc.  All Rights Reserved.
 * 
 * Header file for dummy holder power sensor. Used for testing the rest 
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
#ifndef AI_POWER_SENSOR_H_
#define AI_POWER_SENSOR_H_

typedef enum 
{
  power_undefined,
  power_off,
  power_on
} Power_Status_t;

const char * m_power_status_strings[] = {"Undefined", "Off", "On" };


// Forward method declarations
void toggle_power_status ();
Power_Status_t read_power_status ();
const char * read_power_status_string ();

#endif /* AI_POWER_SENSOR_H_ */

