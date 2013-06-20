/*
 *
 * iviLINK SDK, version 1.1.2
 * http://www.ivilink.net
 * Cross Platform Application Communication Stack for In-Vehicle Applications
 *
 * Copyright (C) 2012, Luxoft Professional Corp., member of IBS group
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
 *
 */

/**
 * @ingroup iOS Port
 * @author Bratanova Elena <ebratanova@luxoft.com>
 * @date 24.05.2013
 */

#import <UIKit/UIPasteboard.h>
#include <set>

#include "json.h"
#include "CFileStorage.hpp"

using namespace iviLink;
using std::ios;

static const char gsModuleName[] = "CFileStorage";

static const char gsTrustedTag[] = "trusted";
static const char gsOurTag[] = "our";
static const char gsNameTag[] = "Name";
static const char gsUidTag[] = "uid";
static const char gsLastTag[] = "last";
static const char gsTimeTag[] = "lasttime";

#define PASTEBOARD_NAME @"com.Luxoft.iviLink"
#define PASTEBOARD_KEY @"trust_list"
#define PASTEBOARD_TYPE @"com.Luxoft.iviLink.trust_list"

Logger CFileStorage::msLogger = Logger::getInstance("Authentication.Trust_list.CFileStorage");

CTrustListError CFileStorage::connect()
{
    return CTrustListError::NoTLError(gsModuleName);
}

CTrustListError CFileStorage::disconnect()
{
    return CTrustListError::NoTLError(gsModuleName);
}

bool CFileStorage::isReady() const
{
    return true;
}

bool read(Json::Value& root)
{
    UIPasteboard * board = [UIPasteboard pasteboardWithName:PASTEBOARD_NAME create:YES];
    NSData * result = [board valueForPasteboardType: PASTEBOARD_TYPE];
    if (result == nil)
    {
        return false;
    }
    NSString * resultStr = [[NSString alloc] initWithData:result encoding:NSUTF8StringEncoding];
    NSLog(@"read from trust list: %@", resultStr);
    Json::Reader reader;
    return reader.parse([resultStr cStringUsingEncoding:NSUTF8StringEncoding], root);
}

bool write(Json::Value const& root)
{
    Json::StyledWriter writer;
    std::string val = writer.write(root);
    NSLog(@"will be saving trust list: %s", val.c_str());
    UIPasteboard * board = [UIPasteboard pasteboardWithName:PASTEBOARD_NAME create:YES];
    [board setPersistent: YES];
    [board setValue: [NSString stringWithFormat: @"%s", val.c_str()] forPasteboardType: PASTEBOARD_TYPE];
    return true;
}

CTrustListError CFileStorage::readAll(tNameUidVector& uids)
{
    uids.clear();
    Json::Value root;
    if (!read(root))
    {
        return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
    }
    
    const Json::Value trusted = root[gsTrustedTag];
    if (!trusted.isNull())
    {
        for (size_t i = 0; i < trusted.size(); ++i)
        {
            tNameUid values;
            values.first = trusted[i][gsUidTag].asString();
            values.second = trusted[i][gsNameTag].asString();
            uids.push_back(values);
        }
    }
    return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::insert(BaseUid const& uid)
{
    Json::Value root;
    if (!read(root))
    {
        LOG4CPLUS_WARN(msLogger, "unable to parse file");
    }
    
    std::set<tNameUid> uids;
    Json::Value trusted = root[gsTrustedTag];
    
    bool found = false;
    if (!trusted.isNull())
    {
        for (size_t i = 0; i < trusted.size(); ++i)
        {
            tNameUid element;
            element.first = trusted[i][gsUidTag].asString();
            if (element.first == uid.value())
            {
                found = true;
            }
            element.second = trusted[i][gsNameTag].asString();
            uids.insert(element);
        }
    }
    
    if (!found)
    {
        uids.insert(tNameUid(uid, ""));
    }
    trusted.clear();
    for (std::set<tNameUid>::const_iterator it = uids.begin(); it != uids.end(); ++it)
    {
        Json::Value uidName;
        uidName[gsUidTag] = it->first.value();
        uidName[gsNameTag] = it->second;
        trusted.append(uidName);
    }
    root[gsTrustedTag] = trusted;
    write(root);
    return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::remove(BaseUid const& uid)
{
    Json::Value root;
    if (!read(root))
    {
        LOG4CPLUS_WARN(msLogger, "unable to parse file");
    }
    
    std::set<tNameUid> uids;
    Json::Value trusted = root[gsTrustedTag];
    if (trusted.isNull())
    {
        return CTrustListError(CTrustListError::ERROR_UNKNOWN_UID, gsModuleName, "unknown uid");
    }
    for (size_t i = 0; i < trusted.size(); ++i)
    {
        tNameUid element;
        element.first = trusted[i][gsUidTag].asString();
        element.second = trusted[i][gsNameTag].asString();
        uids.insert(element);
    }
    
    std::set<tNameUid>::iterator i = uids.begin();
    bool deleteFlag = false;
    while(i != uids.end())
    {
        std::set<tNameUid>::iterator current = i++;
        if( current->first == uid )
        {
            uids.erase(current);
            deleteFlag = true;
        }
    }
    if (!deleteFlag)
    {
        return CTrustListError(CTrustListError::ERROR_UNKNOWN_UID, gsModuleName, "unknown uid");
    }
    trusted.clear();
    for (std::set<tNameUid>::const_iterator it = uids.begin(); it != uids.end(); ++it)
    {
        Json::Value UidName;
        UidName[gsUidTag] = it->first.value();
        UidName[gsNameTag] = it->second;
        trusted.append(UidName);
    }
    
    root[gsTrustedTag] = trusted;
    write(root);
    return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::clear()
{
    Json::Value root;
    if (!read(root))
    {
        LOG4CPLUS_WARN(msLogger, "unable to parse file");
    }
    root[gsTrustedTag] = Json::Value();
    write(root);
    return CTrustListError::NoTLError(gsModuleName);
}

CTrustListError CFileStorage::getOurUid(BaseUid& uid)
{  
    Json::Value root;
    if (!read(root))
    {
        return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
    }
    const Json::Value our = root[gsOurTag][gsUidTag];
    if (our.isNull())
    {
        return CTrustListError(CTrustListError::ERROR_UNKNOWN_UID, gsModuleName, "no our uid");
    }
    uid = BaseUid(our.asString());
    return CTrustListError::NoTLError(gsModuleName);
}

CTrustListError CFileStorage::setOurUid(BaseUid const& uid)
{    
    Json::Value root;
    if (!read(root))
    {
        LOG4CPLUS_WARN(msLogger, "unable to parse file");
    }
    
    root[gsOurTag][gsUidTag] = uid.value();
    write(root);
    return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::getOurName(std::string& name)
{
    Json::Value root;
    if (!read(root))
    {
        return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
    }
    
    const Json::Value our = root[gsOurTag][gsNameTag];
    if (our.isNull())
    {
        return CTrustListError(CTrustListError::ERROR_UNKNOWN_UID, gsModuleName, "no our name");
    }
    name = our.asString();
    return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::setOurName(std::string const& name)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    Json::Value root;
    if (!read(root))
    {
        LOG4CPLUS_WARN(msLogger, "unable to parse file");
    }
    root[gsOurTag][gsNameTag] = name;
    write(root);
    return CTrustListError::NoTLError(gsModuleName);
}

CTrustListError CFileStorage::updateName(BaseUid const& uid, std::string const& name)
{
    LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
    
    Json::Value root;
    if (!read(root))
    {
        LOG4CPLUS_WARN(msLogger, "unable to parse file");
    }
    
    std::set<tNameUid> uids;
    Json::Value trusted = root[gsTrustedTag];
    
    if (!trusted.isNull())
    {
        for (size_t i = 0; i < trusted.size(); ++i)
        {
            tNameUid element;
            element.first = trusted[i][gsUidTag].asString();
            if(element.first == uid )
            {
                element.second = name;
            }
            else
            {
                element.second = trusted[i][gsNameTag].asString();
            }
            uids.insert(element);
        }
    }
    
    trusted.clear();
    
    for (std::set<tNameUid>::const_iterator it = uids.begin(); it != uids.end(); ++it)
    {
        Json::Value UidName;
        UidName[gsUidTag] = it->first.value();
        UidName[gsNameTag] = it->second;
        trusted.append(UidName);
    }
    
    root[gsTrustedTag] = trusted;
    write(root);
    return CTrustListError::NoTLError(gsModuleName);
}

CTrustListError CFileStorage::setLastConnection(BaseUid const& uid, time_t timestamp)
{    
    Json::Value root;
    if (!read(root))
    {
        LOG4CPLUS_WARN(msLogger, "unable to parse file");
    }
    
    root[gsLastTag][gsTimeTag]=static_cast<int>(timestamp);
    root[gsLastTag][gsUidTag]=uid.value();
    write(root);
    return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::getLastConnection(BaseUid & uid, time_t &timestamp)
{
    Json::Value root;
    if (!read(root))
    {
        return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
    }
    timestamp = root[gsLastTag][gsTimeTag].asInt();
    uid = root[gsLastTag][gsUidTag].asString();
    return CTrustListError::NoTLError(gsModuleName);
}

CFileStorage::CFileStorage(std::string const& pathToStorage) :
mFilename(pathToStorage + "/trust.db")
{
}

CFileStorage::~CFileStorage()
{
    disconnect();
}
