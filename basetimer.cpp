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

#include "basetimer.h"

VE_VMS_BASETIMER::VE_VMS_BASETIMER(VE_VMS_RAM *_ram, VE_VMS_INTERRUPTS *_intHandler, VE_VMS_CPU *_cpu)
{
	ram = _ram;
	intHandler = _intHandler;
	cpu = _cpu;
	BTR = 0;
}

VE_VMS_BASETIMER::~VE_VMS_BASETIMER()
{
}

void VE_VMS_BASETIMER::runTimer() 
{
	int BTCR_data = ram->readByte_RAW(BTCR);

	bool BTStarted = (BTCR_data & 64) != 0;
	bool Int0Enabled = (BTCR_data & 1) != 0;
	bool Int1Enabled = (BTCR_data & 4) != 0;
	int int1cycle;

	int cycleControl = ((BTCR_data & 16) >> 4) | ((BTCR_data & 32) >> 4);

	switch (cycleControl)
	{
		case 0:
			int1cycle = 32;
			break;
		case 1:
			int1cycle = 128;
			break;
		case 2:
			int1cycle = 512;
			break;
		case 3:
			int1cycle = 2048;
			break;
		default:
			int1cycle = 2048;
	}

	if(BTStarted)
	{
		BTR += 32786.0 / cpu->getCurrentFrequency();

		//Throw interrupt 1 source when cycle chosen is reached
		if(BTR >= int1cycle) 
		{
			BTCR_data |= 8;
			ram->writeByte_RAW(BTCR, BTCR_data);

			if (Int1Enabled) 
				intHandler->setINT3();

		}


		if((BTR > 16383) || (BTR > 63 && ((BTCR_data & 128) != 0)))
		{
			//Throw full (14-bit) overflow interrupt (Interrupt 0 source)
			BTCR_data |= 2;
			ram->writeByte_RAW(BTCR, BTCR_data);

			if(Int0Enabled)
				intHandler->setINT3();

			if(BTR > 16383) BTR = 0;
		}
	}
}
