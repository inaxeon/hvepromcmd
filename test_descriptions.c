/*
 *   File:   DESC_TEST_descriptions.c
 *   Author: Matthew Millman (inaxeon@hotmail.com)
 *
 *   1702A/270x/MCM6876x/MCS48 Programmer
 *
 *   Test description strings
 *
 *   This is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *   This software is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *   You should have received a copy of the GNU General Public License
 *   along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pch.h"

#include "project.h"
#include "test.h"

#define TEST_1702A_READ_PON     1
#define TEST_1702A_READ_CS      2
#define TEST_1702A_READ_AA      3
#define TEST_1702A_READ_55      4
#define TEST_1702A_READ_DATA    5
#define TEST_1702A_WRITE_PON    6
#define TEST_1702A_WRITE_PGM    7
#define TEST_1702A_WRITE_VDD    8
#define TEST_1702A_WRITE_AA     9
#define TEST_1702A_WRITE_55     10

#define TEST_270X_PON           1
#define TEST_270X_RD            2
#define TEST_270X_WR            3
#define TEST_270X_PE            4
#define TEST_270X_AA            5
#define TEST_270X_55            6
#define TEST_270X_DATA          7

#define TEST_MCM6876X_PON       1
#define TEST_MCM6876X_RD        2
#define TEST_MCM6876X_WR        3
#define TEST_MCM6876X_P         4
#define TEST_MCM6876X_AA        5
#define TEST_MCM6876X_55        6
#define TEST_MCM6876X_DATA      7

#define TEST_MCS48_PON          1
#define TEST_MCS48_EA_12V       2
#define TEST_MCS48_EA_18V       3
#define TEST_MCS48_EA_22V       4
#define TEST_MCS48_PROG         5
#define TEST_MCS48_VDD_21V      6
#define TEST_MCS48_VDD_25V      7
#define TEST_MCS48_AA           8
#define TEST_MCS48_55           9
#define TEST_MCS48_DATA         10

#define DESC_TEST_1702A_READ_PON \
    "Connect your multimeter negative (black) probe to a GND pin on the Arduino header.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "24: VDD = -9V\r\n" \
    "16: VGG = -9V\r\n" \
    "23: VCC = +5V\r\n" \
    "22: VCC = +5V\r\n" \
    "12: VCC = +5V\r\n" \
    "15: VBB = +5V\r\n" \
    "13: PGM = +5V\r\n" \
    "14: CS  = +5V\r\n"

#define DESC_TEST_1702A_READ_CS \
    "Connect your multimeter negative (black) probe to a GND pin on the Arduino header.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "14: CS = 0V\r\n"

#define DESC_TEST_1702A_READ_AA \
    "Connect your multimeter negative (black) probe to a GND pin on the Arduino header.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    " 3: A0 = 0V\r\n" \
    " 1: A2 = 0V\r\n" \
    "20: A4 = 0V\r\n" \
    "18: A6 = 0V\r\n" \
    "\r\n" \
    " 2: A1 = +5V\r\n" \
    "21: A3 = +5V\r\n" \
    "19: A5 = +5V\r\n" \
    "17: A7 = +5V\r\n"

#define DESC_TEST_1702A_READ_55 \
    "Connect your multimeter negative (black) probe to a GND pin on the Arduino header.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    " 3: A0 = +5V\r\n" \
    " 1: A2 = +5V\r\n" \
    "20: A4 = +5V\r\n" \
    "18: A6 = +5V\r\n" \
    "\r\n" \
    " 2: A1 = 0V\r\n" \
    "21: A3 = 0V\r\n" \
    "19: A5 = 0V\r\n" \
    "17: A7 = 0V\r\n"

#define DESC_TEST_1702A_READ_DATA \
    "Use a 1K resistor connected to VDD (pin 24) - touch the other end on each data pin, ensuring that you witness each bit transition from HIGH to LOW: \r\n" \
    "\r\n" \
    "Note that the ""HIGH"" level voltage is expected to be only 0.7V - you do not need to measure it for this test.\r\n" \
    "\r\n" \
    "The display below is refreshed every second.\r\n" \
    "\r\n" \
    "[ 4] [ 5] [ 6] [ 7] [ 8] [ 9] [10] [11]\r\n"

#define DESC_TEST_1702A_WRITE_PON \
    "Connect your multimeter negative (black) probe to a GND pin on the Arduino header.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "*** ENSURE Q10 - Q18 DO NOT OVERHEAT. RUN TEST FOR AS LITTLE TIME AS POSSIBLE. ***\r\n" \
    "\r\n" \
    "24: VDD = +47V\r\n" \
    "16: VGG = +47V\r\n" \
    "23: VCC = +47V\r\n" \
    "22: VCC = +47V\r\n" \
    "12: VCC = +47V\r\n" \
    "15: VBB = +59V\r\n" \
    "13: PGM = +47V\r\n" \
    "14: CS  = +47V\r\n"

#define DESC_TEST_1702A_WRITE_PGM \
    "Connect your multimeter negative (black) probe to a GND pin on the Arduino header.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "*** ENSURE Q10 - Q18 DO NOT OVERHEAT. RUN TEST FOR AS LITTLE TIME AS POSSIBLE. ***\r\n" \
    "\r\n" \
    "13: PGM = 0V\r\n"

#define DESC_TEST_1702A_WRITE_VDD \
    "Connect your multimeter negative (black) probe to a GND pin on the Arduino header.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "*** ENSURE Q10 - Q18 DO NOT OVERHEAT. RUN TEST FOR AS LITTLE TIME AS POSSIBLE. ***\r\n" \
    "\r\n" \
    "24: VDD = 0V\r\n" \
    "16: VDD = +10V\r\n"

#define DESC_TEST_1702A_WRITE_AA \
    "Connect your multimeter negative (black) probe to a GND pin on the Arduino header.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "*** ENSURE Q10 - Q18 DO NOT OVERHEAT. RUN TEST FOR AS LITTLE TIME AS POSSIBLE. ***\r\n" \
    "\r\n" \
    " 3: A0 = +3.3V\r\n" \
    " 1: A2 = +3.3V\r\n" \
    "20: A4 = +3.3V\r\n" \
    "18: A6 = +3.3V\r\n" \
    "\r\n" \
    " 2: A1 = +47V\r\n" \
    "21: A3 = +47V\r\n" \
    "19: A5 = +47V\r\n" \
    "17: A7 = +47V\r\n"

#define DESC_TEST_1702A_WRITE_55 \
    "Connect your multimeter negative (black) probe to a GND pin on the Arduino header.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "*** ENSURE Q10 - Q18 DO NOT OVERHEAT. RUN TEST FOR AS LITTLE TIME AS POSSIBLE. ***\r\n" \
    "\r\n" \
    " 3: A0 = +47V\r\n" \
    " 1: A2 = +47V\r\n" \
    "20: A4 = +47V\r\n" \
    "18: A6 = +47V\r\n" \
    "\r\n" \
    " 2: A1 = +3.3V\r\n" \
    "21: A3 = +3.3V\r\n" \
    "19: A5 = +3.3V\r\n" \
    "17: A7 = +3.3V\r\n"


#define DESC_TEST_270X_PON \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "24: VCC   = +5V\r\n" \
    "21: VEE   = -5V\r\n" \
    "19: VDD   = +12V\r\n" \
    "20: CS/PE = +4.2V\r\n" \
    "18: VPP   = 0V\r\n"

#define DESC_TEST_270X_RD \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "20: CS/PE = 0V\r\n"

#define DESC_TEST_270X_PE \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "20: CS/PE = +12V\r\n"

#define DESC_TEST_270X_WR \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "18: VPP = +25.6V\r\n" \
    "\r\n" \
    "The VPP generator circuit will make a squealing noise during this test.\r\n"

#define DESC_TEST_270X_AA \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    " 8: A0  = 0V\r\n" \
    " 6: A2  = 0V\r\n" \
    " 4: A4  = 0V\r\n" \
    " 2: A6  = 0V\r\n" \
    "23: A8  = 0V\r\n" \
    "\r\n" \
    " 9: DQ0 = 0V\r\n" \
    "11: DQ2 = 0V\r\n" \
    "14: DQ4 = 0V\r\n" \
    "16: DQ6 = 0V\r\n" \
    "\r\n" \
    "7:  A1  = +5V\r\n" \
    "5:  A3  = +5V\r\n" \
    "3:  A5  = +5V\r\n" \
    "1:  A7  = +5V\r\n" \
    "22: A9  = +5V\r\n" \
    "\r\n" \
    "10: DQ1 = +5V\r\n" \
    "13: DQ3 = +5V\r\n" \
    "15: DQ5 = +5V\r\n" \
    "17: DQ7 = +5V\r\n"

#define DESC_TEST_270X_55 \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    " 8: A0  = +5V\r\n" \
    " 6: A2  = +5V\r\n" \
    " 4: A4  = +5V\r\n" \
    " 2: A6  = +5V\r\n" \
    "23: A8  = +5V\r\n" \
    "\r\n" \
    " 9: DQ0 = +5V\r\n" \
    "11: DQ2 = +5V\r\n" \
    "14: DQ4 = +5V\r\n" \
    "16: DQ6 = +5V\r\n" \
    "\r\n" \
    "7:  A1  = 0V\r\n" \
    "5:  A3  = 0V\r\n" \
    "3:  A5  = 0V\r\n" \
    "1:  A7  = 0V\r\n" \
    "22: A9  = 0V\r\n" \
    "\r\n" \
    "10: DQ1 = 0V\r\n" \
    "13: DQ3 = 0V\r\n" \
    "15: DQ5 = 0V\r\n" \
    "17: DQ7 = 0V\r\n"

#define DESC_TEST_270X_DATA \
    "Use 8 10K resistors to tie all of the data pins LOW to GND (pin 12).\r\n" \
    "\r\n" \
    "Connect another 100ohm resistor to VCC (pin 24) and touch the other end on the data pins one by one, ensuring that you witness each bit in both LOW and HIGH states.\r\n" \
    "\r\n" \
    "The display below is refreshed every second.\r\n" \
    "\r\n" \
    "[ 9] [10] [11] [13] [14] [15] [16] [17]\r\n"


#define DESC_TEST_MCM6876X_PON \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "24: VCC   = +5V\r\n" \
    "20: G/VPP = +4.2V\r\n"

#define DESC_TEST_MCM6876X_RD \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "20: G/VPP = 0V\r\n"

#define DESC_TEST_MCM6876X_WR \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "20: G/VPP = +25V\r\n" \
    "\r\n" \
    "The VPP generator circuit will make a squealing noise during this test.\r\n"

#define DESC_TEST_MCM6876X_AA \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    " 8: A0  = 0V\r\n" \
    " 6: A2  = 0V\r\n" \
    " 4: A4  = 0V\r\n" \
    " 2: A6  = 0V\r\n" \
    "23: A8  = 0V\r\n" \
    "19: A10 = 0V\r\n" \
    "21: A12 = 0V\r\n" \
    "\r\n" \
    " 9: DQ0 = 0V\r\n" \
    "11: DQ2 = 0V\r\n" \
    "14: DQ4 = 0V\r\n" \
    "16: DQ6 = 0V\r\n" \
    "\r\n" \
    "7:  A1  = +5V\r\n" \
    "5:  A3  = +5V\r\n" \
    "3:  A5  = +5V\r\n" \
    "1:  A7  = +5V\r\n" \
    "22: A9  = +5V\r\n" \
    "18: A11 = +5V\r\n" \
    "\r\n" \
    "10: DQ1 = +5V\r\n" \
    "13: DQ3 = +5V\r\n" \
    "15: DQ5 = +5V\r\n" \
    "17: DQ7 = +5V\r\n"

#define DESC_TEST_MCM6876X_55 \
    "Connect your multimeter negative (black) probe to pin 12 on the EPROM socket.\r\n" \
    "\r\n" \
    "Check the following pins on the EPROM socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    " 8: A0  = +5V\r\n" \
    " 6: A2  = +5V\r\n" \
    " 4: A4  = +5V\r\n" \
    " 2: A6  = +5V\r\n" \
    "23: A8  = +5V\r\n" \
    "19: A10 = +5V\r\n" \
    "21: A12 = +5V\r\n" \
    "\r\n" \
    " 9: DQ0 = +5V\r\n" \
    "11: DQ2 = +5V\r\n" \
    "14: DQ4 = +5V\r\n" \
    "16: DQ6 = +5V\r\n" \
    "\r\n" \
    "7:  A1  = 0V\r\n" \
    "5:  A3  = 0V\r\n" \
    "3:  A5  = 0V\r\n" \
    "1:  A7  = 0V\r\n" \
    "22: A9  = 0V\r\n" \
    "18: A11 = 0V\r\n" \
    "\r\n" \
    "10: DQ1 = 0V\r\n" \
    "13: DQ3 = 0V\r\n" \
    "15: DQ5 = 0V\r\n" \
    "17: DQ7 = 0V\r\n"

#define DESC_TEST_MCM6876X_DATA \
    "Use 8 10K resistors to tie all of the data pins LOW to GND (pin 12).\r\n" \
    "\r\n" \
    "Connect another 100ohm resistor to VCC (pin 24) and touch the other end on the data pins one by one, ensuring that you witness each bit in both LOW and HIGH states.\r\n" \
    "\r\n" \
    "The display below is refreshed every second.\r\n" \
    "\r\n" \
    "[ 9] [10] [11] [13] [14] [15] [16] [17]\r\n"


#define DESC_TEST_MCS48_PON \
    "Connect your multimeter negative (black) probe to pin 20 on the MCU socket.\r\n" \
    "\r\n" \
    "Check the following pins on the MCU socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "40: VCC   = +5V\r\n" \
    " 7: EA    = +5V\r\n" \
    "25: PROG  = +5V\r\n" \
    "26: VDD   = +5V\r\n"

#define DESC_TEST_MCS48_EA_12V \
    "Connect your multimeter negative (black) probe to pin 20 on the MCU socket.\r\n" \
    "\r\n" \
    "Check the following pins on the MCU socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    " 7: EA    = +12V\r\n"

#define DESC_TEST_MCS48_EA_18V \
    "Connect your multimeter negative (black) probe to pin 20 on the MCU socket.\r\n" \
    "\r\n" \
    "Check the following pins on the MCU socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    " 7: EA    = +18V\r\n"

#define DESC_TEST_MCS48_EA_22V \
    "Connect your multimeter negative (black) probe to pin 20 on the MCU socket.\r\n" \
    "\r\n" \
    "Check the following pins on the MCU socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    " 7: EA    = +22V\r\n"

#define DESC_TEST_MCS48_PROG \
    "Connect your multimeter negative (black) probe to pin 20 on the MCU socket.\r\n" \
    "\r\n" \
    "Check the following pins on the MCU socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "25: PROG  = +18V\r\n"

#define DESC_TEST_MCS48_VDD_21V \
    "Connect your multimeter negative (black) probe to pin 20 on the MCU socket.\r\n" \
    "\r\n" \
    "Check the following pins on the MCU socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "26: VDD   = +21V\r\n"

#define DESC_TEST_MCS48_VDD_25V \
    "Connect your multimeter negative (black) probe to pin 20 on the MCU socket.\r\n" \
    "\r\n" \
    "Check the following pins on the MCU socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "26: VDD   = +25V\r\n"

#define DESC_TEST_MCS48_AA \
    "Connect your multimeter negative (black) probe to pin 20 on the MCU socket.\r\n" \
    "\r\n" \
    "Check the following pins on the MCU socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "21: P20     = 0V\r\n" \
    "23: P22     = 0V\r\n" \
    " 1: TEST0   = 0V\r\n" \
    "\r\n" \
    "12: D0      = 0V\r\n" \
    "14: D2      = 0V\r\n" \
    "16: D4      = 0V\r\n" \
    "18: D6      = 0V\r\n" \
    "\r\n" \
    "22: P21     = +5V\r\n" \
    " 4: RESET   = +5V\r\n" \
    "\r\n" \
    "13: D1      = +5V\r\n" \
    "15: D3      = +5V\r\n" \
    "17: D5      = +5V\r\n" \
    "19: D7      = +5V\r\n"

#define DESC_TEST_MCS48_55 \
    "Connect your multimeter negative (black) probe to pin 20 on the MCU socket.\r\n" \
    "\r\n" \
    "Check the following pins on the MCU socket are within 5% of the specified values: \r\n" \
    "\r\n" \
    "21: P20     = +5V\r\n" \
    "23: P22     = +5V\r\n" \
    " 1: TEST0   = +5V\r\n" \
    "\r\n" \
    "12: D0      = +5V\r\n" \
    "14: D2      = +5V\r\n" \
    "16: D4      = +5V\r\n" \
    "18: D6      = +5V\r\n" \
    "\r\n" \
    "22: P21     = 0V\r\n" \
    " 4: RESET   = 0V\r\n" \
    "\r\n" \
    "13: D1      = 0V\r\n" \
    "15: D3      = 0V\r\n" \
    "17: D5      = 0V\r\n" \
    "19: D7      = 0V\r\n"

#define DESC_TEST_MCS48_DATA \
    "Use 8 10K resistors to tie all of the data pins LOW to GND (pin 20).\r\n" \
    "\r\n" \
    "Connect another 100ohm resistor to VCC (pin 40) and touch the other end on the data pins one by one, ensuring that you witness each bit in both LOW and HIGH states.\r\n" \
    "\r\n" \
    "The display below is refreshed every second.\r\n" \
    "\r\n" \
    "[12] [13] [14] [15] [16] [17] [18] [19]\r\n"

const test_t _g_1702a_tests[] =
{
    { TEST_1702A_READ_PON, false, DESC_TEST_1702A_READ_PON },
    { TEST_1702A_READ_CS, false, DESC_TEST_1702A_READ_CS },
    { TEST_1702A_READ_AA, false, DESC_TEST_1702A_READ_AA },
    { TEST_1702A_READ_55, false, DESC_TEST_1702A_READ_55 },
    { TEST_1702A_READ_DATA, true, DESC_TEST_1702A_READ_DATA },
    { TEST_1702A_WRITE_PON, false, DESC_TEST_1702A_WRITE_PON },
    { TEST_1702A_WRITE_PGM, false, DESC_TEST_1702A_WRITE_PGM },
    { TEST_1702A_WRITE_VDD, false, DESC_TEST_1702A_WRITE_VDD },
    { TEST_1702A_WRITE_AA, false, DESC_TEST_1702A_WRITE_AA },
    { TEST_1702A_WRITE_55, false, DESC_TEST_1702A_WRITE_55 },
    { 0, false, NULL },
};

const test_t _g_270x_tests[] =
{
    { TEST_270X_PON, false, DESC_TEST_270X_PON },
    { TEST_270X_RD, false, DESC_TEST_270X_RD },
    { TEST_270X_WR, false, DESC_TEST_270X_WR },
    { TEST_270X_PE, false, DESC_TEST_270X_PE },
    { TEST_270X_AA, false, DESC_TEST_270X_AA },
    { TEST_270X_55, false, DESC_TEST_270X_55 },
    { TEST_270X_DATA, true, DESC_TEST_270X_DATA },
    { 0, false, NULL },
};

const test_t _g_mcm6876x_tests[] =
{
    { TEST_MCM6876X_PON, false, DESC_TEST_MCM6876X_PON },
    { TEST_MCM6876X_RD, false, DESC_TEST_MCM6876X_RD },
    { TEST_MCM6876X_WR, false, DESC_TEST_MCM6876X_WR },
    { TEST_MCM6876X_AA, false, DESC_TEST_MCM6876X_AA },
    { TEST_MCM6876X_55, false, DESC_TEST_MCM6876X_55 },
    { TEST_MCM6876X_DATA, true, DESC_TEST_MCM6876X_DATA },
    { 0, false, NULL },
};

const test_t _g_mcs48_tests[] =
{
    { TEST_MCS48_PON, false, DESC_TEST_MCS48_PON },
    { TEST_MCS48_VDD_21V, false, DESC_TEST_MCS48_VDD_21V },
    { TEST_MCS48_VDD_25V, false, DESC_TEST_MCS48_VDD_25V },
    { TEST_MCS48_EA_12V, false, DESC_TEST_MCS48_EA_12V },
    { TEST_MCS48_EA_18V, false, DESC_TEST_MCS48_EA_18V },
    { TEST_MCS48_EA_22V, false, DESC_TEST_MCS48_EA_22V },
    { TEST_MCS48_PROG, false, DESC_TEST_MCS48_PROG },
    { TEST_MCS48_AA, false, DESC_TEST_MCS48_AA },
    { TEST_MCS48_55, false, DESC_TEST_MCS48_55 },
    { TEST_MCS48_DATA, true, DESC_TEST_MCS48_DATA },
    { 0, false, NULL },
};
