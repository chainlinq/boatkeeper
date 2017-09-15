/*
 * Copyright © 2017 Astonish Inc.  All Rights Reserved.
 * 
 * Include file for Boat Keeper on a Raspberry Pi.
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
#ifndef AI_BOATKEEPER_PI_H_
#define AI_BOATKEEPER_PI_H_

// Serial number on a Raspberry Pi is stored in the 'cpuinfo' file and 
// is 16 chars long
#define RASPBERRYPI_SERIAL_NUMBER_FILENAME "/proc/cpuinfo"
#define DUMMY_SERIAL_NUMBER "DUMMY_SERIAL_NUM"
#define SERIAL_NUMBER_LENGTH 17 // 16 digit serial number plus NULL

const char * read_serial_number ();

#endif /* AI_BOATKEEPER_PI_H */
