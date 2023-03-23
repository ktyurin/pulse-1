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

#include "gpio.h"


void GPIO_Initialize(void)
{
    LATA = 0x00;
    LATC = 0x00;

    TRISA = 0x3B;
    TRISC = 0x3F;

    ANSELC = 0x00;
    ANSELA = 0x33;

    WPUA = 0x00;
    WPUC = 0x00;

    ODCONA = 0x00;
    ODCONC = 0x00;

    SLRCONA = 0x37;
    SLRCONC = 0x3F;

    INLVLA = 0x3F;
    INLVLC = 0x3F;

    // interrupt on change - negative
    IOCCNbits.IOCCN4 = 1;
    IOCCNbits.IOCCN5 = 1;
    
    // interrupt on change - positive
    IOCCPbits.IOCCP4 = 0;
    IOCCPbits.IOCCP5 = 1;

    // Enable IOCI interrupt 
    PIE0bits.IOCIE = 1; 
    
}


void GPIO_IOC(void)
{   
	// interrupt on change for pin IOCCF4
    if(IOCCFbits.IOCCF4 == 1)
    {
        IOCCF4_ISR();  
    }	
	// interrupt on change for pin IOCCF5
    if(IOCCFbits.IOCCF5 == 1)
    {
        IOCCF5_ISR();  
    }	
}


void IOCCF4_ISR(void)
{
    IOCCFbits.IOCCF4 = 0;
}


void IOCCF5_ISR(void)
{
    IOCCFbits.IOCCF5 = 0;
}
