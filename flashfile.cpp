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

#include "flashfile.h"


VE_VMS_FLASH_FILE::VE_VMS_FLASH_FILE()
{

}

VE_VMS_FLASH_FILE::VE_VMS_FLASH_FILE(int index, VMS_FILE_TYPE t, int firstblock, char *name, int size, int header)
{ 
	//fileNumber means directory entry
	fileIndex = index;
	type = t;
	startBlock = firstblock;
	fileName = name;
	fileSize = size;
	headerBlock = header;
}

///Copy constructor
VE_VMS_FLASH_FILE::VE_VMS_FLASH_FILE(VE_VMS_FLASH_FILE &c)
{
	fileIndex = c.getFileIndex();
	type = c.getType();
	startBlock = c.getStartBlock();
	fileName = c.getFileName();
	fileSize = c.getFileSize();
	headerBlock = c.getHeaderBlock();
}

VMS_FILE_TYPE VE_VMS_FLASH_FILE::getType(int d)
{
	d &= 0xFF;
	if(d == 0x00) return NONE;
	else if(d == 0x33) return DATA;
	else if(d == 0xCC) return GAME;
	else return UNKNOWN;
}

//Setters and getters
VMS_FILE_TYPE VE_VMS_FLASH_FILE::getType()
{
	return type;
}

int VE_VMS_FLASH_FILE::getStartBlock()
{
	return startBlock;
}

char * VE_VMS_FLASH_FILE::getFileName()
{
	return fileName;
}

int VE_VMS_FLASH_FILE::getFileSize()
{
	return fileSize;
}

int VE_VMS_FLASH_FILE::getHeaderBlock()
{
	return headerBlock;
}

int VE_VMS_FLASH_FILE::getFileIndex() 
{ 
	return fileIndex; 
}

void VE_VMS_FLASH_FILE::setType(VMS_FILE_TYPE t)
{
	type = t;
}

void VE_VMS_FLASH_FILE::setStartBlock(int s)
{
	startBlock = s;
}

void VE_VMS_FLASH_FILE::setFileName(char *name)
{
	fileName = name;
}

void VE_VMS_FLASH_FILE::setFileSize(int size)
{
	fileSize = size;
}

void VE_VMS_FLASH_FILE::setHeaderBlock(int header)
{
	headerBlock = header;
}

void VE_VMS_FLASH_FILE::setFileIndex(int index) 
{
	fileIndex = index;
}
