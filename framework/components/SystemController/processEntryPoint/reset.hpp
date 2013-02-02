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


#ifndef RESET_HPP
#define RESET_HPP


#define SIZE_OF_PIDOF_RET	15
#define PIDOF_STR "pidof "

/**
 * Setup executable name for reset and stop procedure.
 * @param argv0 is the pointer argv[0], where argv is from main()
 */
void setupReset(char const * argv0);

/**
 * Performs hard reset of running system controller and all child processes.
 * First, it calls hardStop().
 * Then, it forks current process (parent). Parent exits right away and child 
 * initializes new process group and launches process from scratch.
 * In case of any error, exit() is called with non-zero code.
 *
 * setupReset() must be called first - its argv0 param is used in hardStop() and 
 * to determine the path to this process's executable file.
 *
 * This function does not return.
 *
 * @param internal_reset set to @c true if function is called from running system
 *    controller with (possible) bunch of child processes - they will be closed.
 *    Set to @c false if function is called from new system controller process.
 */
void hardReset(bool internal_reset);

/**
 * Searches for running processes with the same name as this and stops them with
 * all of their subprocesses.
 * 
 * setupReset() must be called first - its argv0 param is used to determine the 
 * name of current process.
 *
 * @retval 0 success
 * @retval otherwise some error
 */
int hardStop(bool internal_stop);

/**
 * Searches for running processes IVILinkSystemController and return number of
 * runnign process with the same name.
 */
int getQuantityOfRunningIVILink();

/**
 * Searches for running iviLink processes and shutdown them.
 */
void shutdownIviProcesses();

/**
 * Searches for running process and shutdown it.
 *
 * @param proc_name - name of searching for shutdown process
 *
 * @retval true success
 * @retval otherwise some error
 */
bool shutdownIviProcess(const char* proc_name);

#endif // RESET_HPP
