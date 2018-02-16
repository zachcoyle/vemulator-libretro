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

#include "libretro.h"
#include "vmu.h"

retro_environment_t environment_cb;
retro_video_refresh_t video_cb;
retro_audio_sample_t audio_cb;
retro_audio_sample_batch_t audio_batch_cb;
retro_input_poll_t inputPoll_cb;
retro_input_state_t inputState_cb;

struct retro_variable options[2];

VMU *vmu;
uint16_t *frameBuffer;
byte *romData;


RETRO_API void retro_set_environment(retro_environment_t env)
{
	environment_cb = env;
	
	//Set variables (Options)
	options[0].key = "enable_flash_write"; 
	options[0].value = "Enable flash write (.bin, requires restart); enabled|disabled";
	
	options[1].key = NULL;
	options[1].value = NULL;
	
	env(RETRO_ENVIRONMENT_SET_VARIABLES, options);
}

RETRO_API void retro_set_video_refresh(retro_video_refresh_t vr)
{
	video_cb = vr;
}

RETRO_API void retro_set_audio_sample(retro_audio_sample_t sample)
{
	audio_cb = sample;
}

RETRO_API void retro_set_audio_sample_batch(retro_audio_sample_batch_t batch)
{
	audio_batch_cb = batch;
}

RETRO_API void retro_set_input_poll(retro_input_poll_t ipoll)
{
	inputPoll_cb = ipoll;
}

RETRO_API void retro_set_input_state(retro_input_state_t istate)
{
	inputState_cb = istate;
}

RETRO_API void retro_init(void)
{
	frameBuffer = (uint16_t*)calloc(SCREEN_WIDTH*SCREEN_HEIGHT, sizeof(uint16_t));
	vmu = new VMU(frameBuffer);
}

RETRO_API void retro_deinit(void)
{
	delete vmu;
	if(frameBuffer != NULL) free(frameBuffer);
	if(romData != NULL) free(romData);
}

RETRO_API unsigned retro_api_version(void)
{
	return RETRO_API_VERSION;
}

RETRO_API void retro_get_system_info(struct retro_system_info *info)
{
	info->library_name = "VeMUlator";
	info->library_version = "0.1";
	info->valid_extensions = "vms|bin|dci";
	info->need_fullpath = true;
	info->block_extract = false;
}

RETRO_API void retro_get_system_av_info(struct retro_system_av_info *info)
{
	info->geometry.base_width = SCREEN_WIDTH;
	info->geometry.base_height = SCREEN_HEIGHT;
	info->geometry.max_width = SCREEN_WIDTH;
	info->geometry.max_height = SCREEN_HEIGHT;
	info->geometry.aspect_ratio = 0;
	
	info->timing.fps = FPS;
	info->timing.sample_rate = SAMPLE_RATE;
}

RETRO_API void retro_set_controller_port_device(unsigned port, unsigned device)
{
	
}
 
void processInput()
{
	inputPoll_cb();
	
	if(!vmu->cpu->P3_taken) return;	//Don't accept new input until previous is processed
	
	byte P3_reg = vmu->ram->readByte_RAW(P3);
	int pressFlag = 0;
	byte P3_int = vmu->ram->readByte_RAW(P3INT);
	
	P3_reg = ~P3_reg;	//Active low
	
	//Up
	if(inputState_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP))
	{
		P3_reg |= 1;
		pressFlag++;
	}
	else P3_reg &= 0xFE;
	
	//Down
	if(inputState_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN))
	{
		P3_reg |= 2;
		pressFlag++;
	}
	else P3_reg &= 0xFD;
	
	//Left
	if(inputState_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT))
	{
		P3_reg |= 4;
		pressFlag++;
	}
	else P3_reg &= 0xFB;
	
	//Right
	if(inputState_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT))
	{
		P3_reg |= 8;
		pressFlag++;
	}
	else P3_reg &= 0xF7;
	
	//A
	if(inputState_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A))
	{
		P3_reg |= 16;
		pressFlag++;
	}
	else P3_reg &= 0xEF;
	
	//B
	if(inputState_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B))
	{
		P3_reg |= 32;
		pressFlag++;
	}
	else P3_reg &= 0xDF;
	
	//Start
	if(inputState_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START))
	{
		//Clicking MODE without a BIOS causes hang
		//P3_reg |= 64;
		//pressFlag++;
	}
	//else P3_reg &= 0xBF;
	
	P3_reg = ~P3_reg;
	
	vmu->ram->writeByte_RAW(P3, P3_reg);
	
	if(pressFlag)
	{
		vmu->ram->writeByte_RAW(P3INT, P3_int | 2);
		vmu->intHandler->setP3();
		vmu->cpu->P3_taken = false;
	}
	
}

RETRO_API void retro_reset(void)
{
	vmu->reset();
}

RETRO_API void retro_run(void)
{
	processInput();
	
	//Cycles passed since last screen refresh
	size_t cyclesPassed = vmu->cpu->getCurrentFrequency() / FPS;
	
	for(size_t i = 0; i < cyclesPassed; i++)
		vmu->runCycle();

	//Video
	vmu->video->drawFrame(frameBuffer);
	if(vmu->ram->readByte_RAW(MCR) & 8) video_cb(frameBuffer, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH * 2);
	
	//Audio
	vmu->audio->generateSignal(audio_cb);
}

RETRO_API size_t retro_serialize_size(void)
{
}


RETRO_API bool retro_serialize(void *data, size_t size)
{
}

RETRO_API bool retro_unserialize(const void *data, size_t size)
{
}

RETRO_API void retro_cheat_reset(void)
{
}

RETRO_API void retro_cheat_set(unsigned index, bool enabled, const char *code)
{
}

RETRO_API bool retro_load_game(const struct retro_game_info *game)
{
	//Set environment variables
	enum retro_pixel_format format = RETRO_PIXEL_FORMAT_RGB565;
	environment_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &format);
	
	//Opening file
	FILE *rom = fopen(game->path, "rb");
	if(rom == NULL) return false;
	fseek(rom, 0, SEEK_END);
	size_t romSize = ftell(rom);
	fseek(rom, 0 , SEEK_SET);

	romData = (byte *)malloc(romSize);
	for(size_t i = 0; i < romSize; i++)
		romData[i] = fgetc(rom);
	
	fclose(rom);
	
	//Check extension
	char *path = (char *)malloc(strlen(game->path) + 1);
	strcpy(path, game->path);
	char *ext = strchr(path, '.');
	
	//Check needed variables
	struct retro_variable var = {0};
	var.key = "enable_flash_write";
	environment_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var);
	
	//Loading ROM
	if(!strcmp(ext, ".bin") || !strcmp(ext, ".BIN")) 
	{
		//Check if user wants core to be able to write to flash
		if(!strcmp(var.value, "enabled")) vmu->flash->loadROM(romData, romSize, 0, game->path, true);
		else vmu->flash->loadROM(romData, romSize, 0, game->path, false);
	}
	else if(!strcmp(ext, ".vms") || !strcmp(ext, ".VMS")) vmu->flash->loadROM(romData, romSize, 1, game->path, false);
	else if(!strcmp(ext, ".dci") || !strcmp(ext, ".DCI")) vmu->flash->loadROM(romData, romSize, 2, game->path, false);
	
	free(path);
	
	//Initializing system
	vmu->startCPU();
	
	return true;
}

RETRO_API bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info)
{
	return false;
}

RETRO_API void retro_unload_game(void)
{
	vmu->reset();
}

RETRO_API unsigned retro_get_region(void)
{
}

RETRO_API void *retro_get_memory_data(unsigned id)
{
}

RETRO_API size_t retro_get_memory_size(unsigned id)
{
}
