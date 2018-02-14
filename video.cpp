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

#include "video.h"

VE_VMS_VIDEO::VE_VMS_VIDEO(VE_VMS_RAM *_ram)
{
	ram = _ram;
}

VE_VMS_VIDEO::~VE_VMS_VIDEO()
{
}

void VE_VMS_VIDEO::drawFrame(uint16_t *buffer)
{
	//Read XRAM buffer from RAM (WRAM starts at 180), each 6 bytes make 1 horizontal line on-screen
	//Each bit declares whether the pixel is on or off
	//There is a 4-byte empty space between each two lines (96 bytes) of XRAM buffer.

	if((ram->readByte_RAW(MCR) & 8) == 0)
		return;


	size_t XRAMAddress;
	byte MSB = 0x80;	//10000000b
	//Draw pixels (48x32) for banks 0 and 1 ("i" selects bank)
	for(int i = 0, c = 0; i < 2; ++i) 
	{
		XRAMAddress = 0x180;
		for (int y = 0; y < 16; y++) 
		{
			byte *pixelLine = new byte[6];

			if((y % 2 == 0) && (y > 0)) XRAMAddress += 4;

			for(int p = 0; p < 6; p++)
				pixelLine[p] = ram->readByteXRAM(XRAMAddress++, i);

			//Big-Endian
			for (int x = 0; x < 6; x++)
				for (int j = 0; j < 8; j++)
					buffer[c++] = ((pixelLine[x] & (MSB >> j)) != 0) ? 0 : 0xFFFF;
					
			delete []pixelLine;
		}
	}

	//Draw pixels for bank 2 (BIOS Icons)
	/*
	 *	BIOS Icons not needed when HLE is used
	 * 
	scaleX /= 2;    //Since icons are more pixel dense
	scaleY /= 2;
	scaleXM8 = 8*scaleX;
	float scaleXM24 = 24*scaleX;
	float scaleYM66 = 66*scaleY;

	for(int i = 0; i < 4; i++) {
		if(ram->readByteXRAM(0x181 + i, 2) == 0x00) continue; //Only draw icons shown in XRAM bank 2

		int []icon = ICONS[i];
		for (int y = 0; y < 32; y++) {
			int []pixelLine = new int[3];

			System.arraycopy(icon, y * 3, pixelLine, 0, 3);

			for (int x = 0; x < 3; x++)
				for (int j = 0; j < 8; j++)
					screenCanvas.drawRect(marginX + ((x * scaleXM8) + (j * scaleX)) + (i * scaleXM24), marginY + (scaleYM66 + (y * scaleY)), marginX + ((x * scaleXM8) + (j * scaleX)) + scaleX + (i * scaleXM24), marginY + (scaleYM66 + (y * scaleY)) + scaleY, ((pixelLine[x] & (0x80 >> j)) != 0)?pixelColor:noPixelColor);
		}
	}*/
}

