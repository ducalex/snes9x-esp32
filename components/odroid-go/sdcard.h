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

#ifndef _ODROID_SDCARD_H_
#define _ODROID_SDCARD_H_

void odroid_sdcard_init(void);
int odroid_sdcard_read_file(char const *name, char **buffer);
int odroid_sdcard_write_file(char const *name, void *source, int length);
int odroid_sdcard_mkdir(char const *path);
int odroid_sdcard_remove(char const *path);
//#undef fopen
//#define fopen 

#endif