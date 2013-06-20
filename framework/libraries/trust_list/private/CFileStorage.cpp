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

#include <set>
#ifndef __APPLE__
#include "json/include/json/json.h"
#else
#include "json.h"
#endif
#include "CFileStorage.hpp"
#include "CMutex.hpp"

using namespace iviLink;
using std::ios;

static const char gsModuleName[] = "CFileStorage";
static const char gsTrustedTag[] = "trusted";
static const char gsOurTag[] = "our";
static const char gsLastTag[] = "last";
static const char gsNameTag[] = "Name";
static const char gsUidTag[] = "uid";
static const char gsTimeTag[] = "lasttime";
static CMutex fileMutex;

Logger CFileStorage::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profiles.CAuthenticationProfileImpl.trust.CFileStorage"));

CTrustListError CFileStorage::connect()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   
   MutexLocker lock(mFileMutex);

   if (isReady())
   {
      return CTrustListError::NoTLError(gsModuleName);
   }

   file.open(mFilename.c_str());

   if (isReady())
   {
      return CTrustListError::NoTLError(gsModuleName);
   }
   else
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "file is not ready");
   }
}

CTrustListError CFileStorage::disconnect()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   MutexLocker lock(mFileMutex);
   file.close();
   return CTrustListError::NoTLError(gsModuleName);
}

bool CFileStorage::isReady() const
{
   return file.good();
}

bool CFileStorage::reopen(std::ios_base::openmode flags)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   MutexLocker lock(mFileMutex);
   file.close();
   file.open(mFilename.c_str(), flags);
   return isReady();
}

bool read(std::fstream& file, Json::Value& root)
{
   MutexLocker lock(fileMutex);
   file.seekg(0, ios::beg);
   Json::Reader reader;
   return reader.parse(file, root);
}

bool write(std::fstream& file, Json::Value const& root)
{
   MutexLocker lock(fileMutex);
   Json::StyledWriter writer;
   std::string val = writer.write(root);
   LOG4CPLUS_INFO(CFileStorage::msLogger, val);
   file << val;
   file.flush();
   file.seekg(0, ios::beg);
   return file.good();
}

CTrustListError CFileStorage::readAll(tNameUidVector& uids)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   // if (!isReady())
   //    return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "file is not ready");
   
   uids.clear();

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
   }

   Json::StyledWriter writer;
   std::string val = writer.write(root);
   LOG4CPLUS_INFO(msLogger, "Read all trust list with names: " + val);
   

   const Json::Value trusted = root[gsTrustedTag];
   if (trusted.isNull())
   {
      return CTrustListError::NoTLError(gsModuleName);
   }


   for (size_t i = 0; i < trusted.size(); ++i)
   {
      tNameUid values;
      values.first = trusted[i][gsUidTag].asString();
      values.second = trusted[i][gsNameTag].asString();
      uids.push_back(values);
   }

   return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::insert(BaseUid const& uid)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   // if (!isReady())
   //    return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "file is not ready");

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
   {
      //return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
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
          element.second = trusted[i][gsNameTag].asString();
          uids.insert(element);
      }
   }

   uids.insert(tNameUid(uid, ""));
   
   trusted.clear(); 
   
   for (std::set<tNameUid>::const_iterator it = uids.begin(); it != uids.end(); ++it)
   {
      Json::Value UidName;
      UidName[gsUidTag] = it->first.value();
      UidName[gsNameTag] = it->second;
      trusted.append(UidName);
   }

   root[gsTrustedTag] = trusted;

   if (!reopen(ios::in | ios::out | ios::trunc))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   }
   
   if (!write(file, root))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");
   }

   return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::remove(BaseUid const& uid)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   // if (!isReady())
   //    return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "file is not ready");

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
   {
      //return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
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
	 element.second = trusted[i][gsNameTag].asString();
         uids.insert(element);
      }
   }

   std::set<tNameUid>::iterator i;
   bool deleteFlag = false;
   for(i = uids.begin(); i != uids.end(); ++i)
   {
      if( i->first == uid )
      {
          uids.erase(i);
          deleteFlag = true;
      }
   }
   if ((i == uids.end())&& !deleteFlag)
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

   if (!reopen(ios::in | ios::out | ios::trunc))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   }
   
   if (!write(file, root))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");
   }

   return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::clear()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   // if (!isReady())
   //    return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "file is not ready");

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
   {
      //return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
      LOG4CPLUS_WARN(msLogger, "unable to parse file");
   }

   root[gsTrustedTag] = Json::Value();

   if (!reopen(ios::in | ios::out | ios::trunc))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   }
   
   if (!write(file, root))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");
   }

   return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::getOurUid(BaseUid& uid)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   // if (!isReady())
   //    return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "file is not ready");

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
   }

   const Json::Value our = root[gsOurTag][gsUidTag];
   if (our.isNull())
   {
      return CTrustListError(CTrustListError::ERROR_UNKNOWN_UID, gsModuleName, "no our uid");
   }
   uid = BaseUid(our.asString());

   return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::setOurUid(BaseUid const& uid)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   // if (!isReady())
   //    return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "file is not ready");

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
   {
      //return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
      LOG4CPLUS_WARN(msLogger, "unable to parse file");
   }

   root[gsOurTag][gsUidTag] = uid.value();
   
   if (!reopen(ios::in | ios::out | ios::trunc))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   }
   
   if (!write(file, root))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");
   }

   return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::getOurName(std::string& name)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
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
   if (!reopen(ios::in) || !read(file, root))
   {
      LOG4CPLUS_WARN(msLogger, "unable to parse file");
   }

   root[gsOurTag][gsNameTag] = name;
   
   if (!reopen(ios::in | ios::out | ios::trunc))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   }
   
   if (!write(file, root))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");
   }

   return CTrustListError::NoTLError(gsModuleName);
}

CTrustListError CFileStorage::setLastConnection(BaseUid const& uid, time_t timestamp)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
   {
      LOG4CPLUS_WARN(msLogger, "unable to parse file");
   }

   root[gsLastTag][gsTimeTag]=static_cast<int>(timestamp);
   root[gsLastTag][gsUidTag]=uid.value();


   if (!reopen(ios::in | ios::out | ios::trunc))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   }

   if (!write(file, root))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");
   }

   return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::getLastConnection(BaseUid & uid, time_t &timestamp)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");
   }

   timestamp = root[gsLastTag][gsTimeTag].asInt();
   uid = root[gsLastTag][gsUidTag].asString();
   
   return CTrustListError::NoTLError(gsModuleName);
}

CTrustListError CFileStorage::updateName(BaseUid const& uid, std::string const& name)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
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
   
   if (!reopen(ios::in | ios::out | ios::trunc))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   }
   
   if (!write(file, root))
   {
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");
   }

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