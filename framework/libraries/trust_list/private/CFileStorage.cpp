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


#include <set>

#include "json/include/json/json.h"

#include "CFileStorage.hpp"

using namespace iviLink;
using std::ios;

static const char gsModuleName[] = "CFileStorage";

static const char gsTrustedTag[] = "trusted";
static const char gsOurTag[] = "our";

Logger CFileStorage::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profiles.CAuthenticationProfileImpl.trust.CFileStorage"));

CTrustListError CFileStorage::connect()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   if (isReady())
      return CTrustListError::NoTLError(gsModuleName);

   file.open(mFilename.c_str());

   if (isReady())
      return CTrustListError::NoTLError(gsModuleName);
   else
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "file is not ready");
}

CTrustListError CFileStorage::disconnect()
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   file.close();
   return CTrustListError::NoTLError(gsModuleName);
}

bool CFileStorage::isReady() const
{
   return file.good();
}

bool CFileStorage::reopen(std::ios_base::openmode flags)
{
   file.close();
   file.open(mFilename.c_str(), flags);
   return isReady();
}

bool read(std::fstream& file, Json::Value& root)
{
   file.seekg(0, ios::beg);
   Json::Reader reader;
   return reader.parse(file, root);
}

bool write(std::fstream& file, Json::Value const& root)
{
   Json::StyledWriter writer;
   std::string val = writer.write(root);
   LOG4CPLUS_INFO(CFileStorage::msLogger, val);
   file << val;
   file.flush();
   file.seekg(0, ios::beg);
   return file.good();
}

CTrustListError CFileStorage::readAll(tUidVector& uids)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   // if (!isReady())
   //    return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "file is not ready");

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");

   Json::StyledWriter writer;
   std::string val = writer.write(root);
   LOG4CPLUS_INFO(msLogger, "Read all trust list: " + val);

   const Json::Value trusted = root[gsTrustedTag];
   if (trusted.isNull())
      return CTrustListError::NoTLError(gsModuleName);;

   for (size_t i = 0; i < trusted.size(); ++i)
   {
      uids.push_back(trusted[i].asString());
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

   std::set<BaseUid> uids;
   Json::Value trusted = root[gsTrustedTag];
   
   if (!trusted.isNull())
   {
      for (size_t i = 0; i < trusted.size(); ++i)
      {
         uids.insert(trusted[i].asString());
      }
   }

   uids.insert(uid);

   
   // if (trusted.isNull())
   // {
   //    trusted = Json::Value();
   // }
   // else
   // {
      trusted.clear();   
   // }
   
   for (std::set<BaseUid>::const_iterator it = uids.begin(); it != uids.end(); ++it)
   {
      trusted.append(it->value());
   }

   root[gsTrustedTag] = trusted;

   if (!reopen(ios::in | ios::out | ios::trunc))
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   
   if (!write(file, root))
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");

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

   std::set<BaseUid> uids;
   Json::Value trusted = root[gsTrustedTag];
   
   if (!trusted.isNull())
   {
      for (size_t i = 0; i < trusted.size(); ++i)
      {
         uids.insert(trusted[i].asString());
      }
   }

   if (uids.erase(uid) == 0)
      return CTrustListError(CTrustListError::ERROR_UNKNOWN_UID, gsModuleName, "unknown uid");

   
   // if (trusted.isNull())
   // {
   //    trusted = Json::Value();
   // }

   trusted.clear();
   for (std::set<BaseUid>::const_iterator it = uids.begin(); it != uids.end(); ++it)
   {
      trusted.append(it->value());
   }

   root[gsTrustedTag] = trusted;

   if (!reopen(ios::in | ios::out | ios::trunc))
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   
   if (!write(file, root))
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");

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
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   
   if (!write(file, root))
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");

   return CTrustListError::NoTLError(gsModuleName);;
}

CTrustListError CFileStorage::getOurUid(BaseUid& uid)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   // if (!isReady())
   //    return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "file is not ready");

   Json::Value root;
   if (!reopen(ios::in) || !read(file, root))
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to parse file");

   const Json::Value our = root[gsOurTag];
   if (our.isNull())
      return CTrustListError(CTrustListError::ERROR_UNKNOWN_UID, gsModuleName, "no our uid");
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

   root[gsOurTag] = uid.value();
   
   if (!reopen(ios::in | ios::out | ios::trunc))
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to reopen file");
   
   if (!write(file, root))
      return CTrustListError(CTrustListError::ERROR_STORAGE, gsModuleName, "unable to write file");

   return CTrustListError::NoTLError(gsModuleName);;
}

CFileStorage::CFileStorage(std::string const& pathToStorage) :
   mFilename(pathToStorage+"/trust.db")
{
}

CFileStorage::~CFileStorage()
{
   disconnect();
}
