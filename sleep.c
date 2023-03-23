/*
BSD 3-Clause License

Copyright (c) 2023, Konstantin Tyurin <kt@pluraf.se> (Plurafe AB)

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


void sleep(uint32_t ms)
{
    uint8_t CKPS;
    uint16_t timer_count;
    uint32_t timer_cycles = 1;
    if (ms <= 131)
    {
        CKPS = 0b0000;  // 1:1 - 2us
        timer_count = (uint16_t) ((ms * 1000) / 2);
    }
    if (ms < 2000)
    {
        CKPS = 0b0100;  // 1:16 - 32us
        timer_count = (uint16_t) ((ms * 1000) / 32);
    }
    else if (ms < 67000)
    {
        CKPS = 0b1001; // 1:512 - 1024us
        timer_count = (uint16_t) ((ms * 1000) / 1024);
    }
    else if (ms < 4290000)
    {
        CKPS = 0b1111; // 1:32768 - 65536us
        timer_count = (uint16_t) ((ms * 1000) / 65536);
    }
    else 
    {
        CKPS = 0b0101; // 1:32 - 64us
        timer_count = (uint16_t) (1000000 / 64);
        timer_cycles = ms / 1000;
    }
    
    // T0CS MFINTOSC (500KHz); T0ASYNC not_synchronized; 
    T0CON1 = 0xB0 | CKPS;
    // +1 because timer fires when TMR0H:TMR0L rolls over
    uint16_t timer_val = 0xFFFF - timer_count + 1;
    timer_cycles = 1000;
    //for (; timer_cycles != 0; timer_cycles--)
    {
        TMR0H = timer_val >> 8;
        TMR0L = (uint8_t) timer_val;
        // Clear Interrupt flag before enabling the interrupt
        PIR0bits.TMR0IF = 0;
        // Enabling TMR0 interrupt.
        PIE0bits.TMR0IE = 1;
        T0CON0 = 0x90;  // T0OUTPS 1:1; T0EN enabled; T016BIT 16-bit;
        while (true)
        {
            SLEEP();
            if (!T0CON0bits.T0EN)
            {
                break;
            }
        }
        T0CON0 = 0x0;
    }
}
