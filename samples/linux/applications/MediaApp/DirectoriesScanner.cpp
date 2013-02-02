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
#include <dirent.h>
#include <fnmatch.h>
#include <errno.h>
#include <string.h>

#include <cstdio>

#include "DirectoriesScanner.hpp"

Logger DirectoriesScanner::mLogger = Logger::getInstance("samples.Applications.Media.DirectoriesScanner");


std::list<std::string> DirectoriesScanner::scan(const char* rootDir, const std::list<std::string>& filenamesFilter, 
                                                const std::list<std::string>& foldersBlacklist, const int depth)
{
    MutexLocker lock(mMutex);
    mInternalResult.clear();
    mFilenamesFilter = filenamesFilter;
    mFoldersBlacklist = foldersBlacklist;
    if (mFilenamesFilter.size() != 0) 
    {
        scan(rootDir, depth);
    }
    return mInternalResult;
}

void DirectoriesScanner::scan(const char* rootDir, const int depth)
{
    if (depth == 0)
    {
        return;
    }

    DIR * d;
    d = opendir (rootDir);
    if (!d) 
    {
        LOG4CPLUS_WARN(mLogger, "Cannot open directory " + std::string(rootDir));
        return;
    }
    
    struct dirent * entry;
    const char * d_name;
    while ((entry = readdir (d)) != false) 
    {
        d_name = entry->d_name;

        if (d_name[0] == '.')
        {
            // skipping hidden files and folders
            continue;
        }

        if (entry->d_type & DT_DIR) // is a directory
        {           
            if (strcmp (d_name, "..") != 0 && strcmp (d_name, ".") != 0) 
            {
                int path_length;
                char path[PATH_MAX];
 
                path_length = snprintf (path, PATH_MAX,
                                        "%s/%s", rootDir, d_name);
                if (path_length >= PATH_MAX) 
                {
                    LOG4CPLUS_WARN(mLogger, "Path length has got too long, will not proceed deeper");
                }
                else
                {
                    bool inBlacklist = false;
                    std::string d_nameStr(rootDir);
                    d_nameStr += "/";
                    d_nameStr += d_name;
                    d_nameStr += "/";
                    std::list<std::string>::const_iterator iter;
                    for(iter = mFoldersBlacklist.begin(); iter != mFoldersBlacklist.end(); iter ++)
                    {
                        if (std::string::npos != d_nameStr.find(*iter))
                        {
                            LOG4CPLUS_WARN(mLogger, "Folder " + d_nameStr + " is in the blacklist, will not check");
                            inBlacklist = true;
                            break;
                        }
                    }
                    if (!inBlacklist)
                    {
                        scan(path, depth - 1);
                    }
                }
            }
        }
        else // is a file, testing against the filter
        {
            std::list<std::string>::const_iterator iter;
            for (iter = mFilenamesFilter.begin(); iter != mFilenamesFilter.end(); iter++)
            {   
                if (fnmatch((*iter).c_str(), d_name, FNM_CASEFOLD) == 0) 
                {
                    mInternalResult.push_back(std::string(rootDir) + "/" + d_name);
                    break;
                }
            }
        }
    }
    closedir (d);
}
