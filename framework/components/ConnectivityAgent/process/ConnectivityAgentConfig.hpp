/* 
 * iviLINK SDK, version 1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 * 
 * Copyright (C) 2012-2013, Luxoft Professional Corp., member of IBS group
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; version 2.1.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 * 
 */ 


#ifndef CONAGNTCONFIG_HPP
#define CONAGNTCONFIG_HPP


#if defined(NONUNIQUE_AXIS_QUEUE)
const char first_lsn_path[]  = "/tmp/axisd1_unix_sock";
const char second_lsn_path[] = "/tmp/axisd2_unix_sock";
#else
const char first_lsn_path[]  = "/tmp/axisd_unix_sock";
const char second_lsn_path[] = "/tmp/axisd_unix_sock";
#endif


#endif // AXISQUEUEPATH_HPP
