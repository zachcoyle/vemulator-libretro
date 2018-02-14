/*
    VeMUlator - A Dreamcast Visual Memory Unit emulator for libretro
    Copyright (C) 2018  Mahmoud Jaoune

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include <math.h>
#include "libretro.h"
#include "common.h"
#include "cpu.h"
#include "ram.h"

class VE_VMS_AUDIO
{
public:
    VE_VMS_AUDIO(VE_VMS_CPU *_cpu, VE_VMS_RAM *_ram);
    
    ~VE_VMS_AUDIO();

    void generateSignal(retro_audio_sample_t &audio_cb);
    
    void setAudioFrequency(double f);

    void setT1(int b);

    void setT1C(int b);
    
    void setEnabled(bool e);

    void runAudioCheck();
    
    int16_t *getSignal();

private:
	int T1LR_reg;
	int T1LC_reg;
	bool IsEnabled;
	int T1LR_old;
	
	double frequency;	//This is supposed to be the CPU's frequency, but we have made the CPU's clock fixed
	
	int16_t *sampleArray;
	
	VE_VMS_CPU *cpu;
	VE_VMS_RAM *ram;
};

#endif // _AUDIO_H_
