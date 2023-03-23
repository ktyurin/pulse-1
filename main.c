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
#include <builtins.h>
#include <stdbool.h>
#include <stdint.h>

#include "system.h"
#include "peripherals/gpio/gpio.h"
#include "peripherals/adc/adc.h"

#include "sleep.h"


#define SAMPLES_NUMBER 20
#define SPIKES_TOLERANCE 5


struct PulseRange
{
    uint32_t min;
    uint32_t max;
};


uint8_t get_pulse_range_code()
{
    return (uint8_t) (IO_RC0_GetValue() << 1 | IO_RC1_GetValue());
}


struct PulseRange get_pulse_range()
{
    switch (get_pulse_range_code())
    {
        case 0b00:
        default:
            return (struct PulseRange) {10, 1000};
        case 0b01:
            return (struct PulseRange) {1000, 10000};
        case 0b10:
            return (struct PulseRange) {10000, 60000};
        case 0b11:
            return (struct PulseRange) {60000, 600000};
    }
}


void wait_trigger()
{
    SLEEP();
    __delay_ms(5);
}


bool is_continuous_mode()
{
    return IO_RC2_GetValue();
}


uint32_t get_pulse_length()
{
    ADC_StartConversion();
    while (!ADC_IsConversionDone());
    adc_result_t adc_result = ADC_GetConversionResult();
    struct PulseRange pulse_range = get_pulse_range();
    uint32_t interval = pulse_range.max - pulse_range.min;
    return pulse_range.min + (adc_result * interval) / 0xFFC0;
}


void main(void)
{
    SYSTEM_Initialize();


    IO_RA2_SetLow();
    ADC_SelectChannel(5);
    
    wait_trigger();
    
    while (true)
    {
        uint8_t zeros = 0;
        for (uint8_t i = 0; i < SAMPLES_NUMBER; i++)
        {
            IO_RC4_GetValue() & (IO_RC5_GetValue() ^ IO_RC3_GetValue()) ? 1 : ++zeros;
        }
        if (zeros >= SAMPLES_NUMBER - SPIKES_TOLERANCE)
        {
            uint32_t pulse_length = get_pulse_length();
            IO_RA2_SetHigh();
            sleep(pulse_length);
            IO_RA2_SetLow();
            if (is_continuous_mode())
            {
                sleep(pulse_length);
            }
            else
            {
                wait_trigger();
            }
        }
        else if (zeros == 0)
        {
            wait_trigger();
        }
    }
}
