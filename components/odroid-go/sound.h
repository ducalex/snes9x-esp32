/* 
 * This file is part of odroid-go-std-lib.
 * Copyright (c) 2019 ducalex.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _ODROID_SOUND_H_
#define _ODROID_SOUND_H_

#define ODROID_SAMPLE_RATE 22050
size_t * odroid_sound_write(void *buffer, size_t length);
void odroid_sound_set_sample_rate(uint32_t rate);
void odroid_sound_deinit(void);
void odroid_sound_init(void);

#endif