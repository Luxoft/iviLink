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
#ifndef DIRECTORIES_SCANNER_HPP
#define DIRECTORIES_SCANNER_HPP

#include <list>
#include <string>

#include "Logger.hpp"
#include "CMutex.hpp"

class DirectoriesScanner
{
public:
    DirectoriesScanner() {}
    /**
     * Returns a list of files that suit the filter and are found in the rootDir 
     * or its subdirectories (no deeper than the depth parameter). One instance can perform only one scan at a time - 
     * calls to this method from other threads will wait until the first one finished.
     */
    std::list<std::string> scan(const char* rootDir, const std::list<std::string>& filenamesFilters, 
                                const std::list<std::string>& foldersBlacklist, const int depth);

private:
    static Logger mLogger;
    CMutex mMutex;
    std::list<std::string> mInternalResult; // all data will be appended here to avoid excess copying
    std::list<std::string> mFilenamesFilter;
    std::list<std::string> mFoldersBlacklist;
    void scan(const char* rootDir, const int depth);
};

#endif //DIRECTORIES_SCANNER_HPP
