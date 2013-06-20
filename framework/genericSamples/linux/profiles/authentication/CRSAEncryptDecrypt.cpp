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



#include <sstream>
#include <sys/time.h>

#include "Types.hpp"
#include "CRSAEncryptDecrypt.hpp"
#include "RSAFileHelper.hpp"

#include "filters.h"
using CryptoPP::PK_EncryptorFilter;
using CryptoPP::PK_DecryptorFilter;
using CryptoPP::StringSink;
using CryptoPP::StringSource;

#include "osrng.h"
using CryptoPP::AutoSeededRandomPool;

using CryptoPP::RSAES_OAEP_SHA_Encryptor;
using CryptoPP::RSAES_OAEP_SHA_Decryptor;

Logger CRSAEncryptDecrypt::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profiles.authentication.CRSAEncryptDecrypt"));
RSA::PrivateKey CRSAEncryptDecrypt::rsaPrivate = RSA::PrivateKey();
bool CRSAEncryptDecrypt::privateKeyGenerated = false;

RSA::PublicKey CRSAEncryptDecrypt::getPublicKey(const std::string& pathToKeyStorage)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(msLogger, "getPublicKey()");
   static RSA::PublicKey rsaPublic(getPrivateKey(pathToKeyStorage));
   return rsaPublic;
}

RSA::PrivateKey CRSAEncryptDecrypt::getPrivateKey(const std::string& pathToKeyStorage)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(msLogger, "getPrivate()");
   timeval begin, end;
   double elapsedTime;
   gettimeofday(&begin, NULL);
   if(!privateKeyGenerated)
   {
	  //try to read it from file
      std::string filePath = pathToKeyStorage + "/private.k";
      LOG4CPLUS_INFO(msLogger, "getPrivate(): filepath=" + filePath);
      RSAFileHelper fileHelper(filePath);

      if(fileHelper.isFilePresent() && fileHelper.getDataSize() != 0)
      {
    	 LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsStarted")), "RSAReadPreviouslyStoredKey");

    	 LOG4CPLUS_INFO(msLogger, "getPrivate(): retrieving previously stored data");
         char signedData[fileHelper.getDataSize()];
         fileHelper.readDataToBuffer(signedData, fileHelper.getDataSize());
         CryptoPP::ByteQueue queue;
         queue.Put(reinterpret_cast<UInt8*>(signedData),  sizeof(signedData));
         rsaPrivate.Load(queue);
         LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsOperable")), "RSAReadPreviouslyStoredKey");
      } 
      else // the file is not present or broken
      {
    	 LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsStarted")), "RSAGenerateAndStoreKey");
         remove(filePath.c_str());
         LOG4CPLUS_INFO(msLogger, "getPrivate(): no key stored, generating");
         AutoSeededRandomPool rnd;
         rsaPrivate.GenerateRandomWithKeySize(rnd, 2048);
         // storing in file
         LOG4CPLUS_INFO(msLogger, "getPrivate(): saving generated file");
         CryptoPP::ByteQueue queue;
         rsaPrivate.Save(queue);
         UInt32 queueSize = queue.CurrentSize();
         UInt8 outgoingData[queueSize];
         queue.Get(outgoingData, queueSize);
         RSAFileHelper::writeDataToFile(reinterpret_cast<const char*>(outgoingData), queueSize, filePath);
         LOG4CPLUS_TRACE(Logger::getInstance(LOG4CPLUS_TEXT("profiler.ComponentIsOperable")), "RSAGenerateAndStoreKey");
      }
      privateKeyGenerated = true;
   }

   gettimeofday(&end, NULL);
   elapsedTime = (end.tv_sec - begin.tv_sec) * 1000.0;      // sec to ms
   elapsedTime += (end.tv_usec - begin.tv_usec) / 1000.0;   // us to ms
   std::ostringstream s;
   s << "(" << elapsedTime << ") ms";
   LOG4CPLUS_INFO(msLogger, "Time elapsed generating private key: ");
   LOG4CPLUS_INFO(msLogger, s.str());
   return rsaPrivate;
}

std::string CRSAEncryptDecrypt::encrypt(const std::string& stringToEncrypt, const RSA::PublicKey& publicKey)
{
   LOG4CPLUS_TRACE_METHOD(msLogger, __PRETTY_FUNCTION__);
   LOG4CPLUS_INFO(msLogger, "encrypt()");

   AutoSeededRandomPool rng;
   std::string cipher;

   LOG4CPLUS_INFO(msLogger, "Input string to encrypt: " + stringToEncrypt);

   RSAES_OAEP_SHA_Encryptor e(publicKey);

   StringSource(stringToEncrypt, true,
       new PK_EncryptorFilter(rng, e,
       new StringSink(cipher)
    ) // PK_EncryptorFilter
   ); // StringSource

   LOG4CPLUS_INFO(msLogger, "Output encrypted string: " + cipher);

   return cipher;
}

std::string CRSAEncryptDecrypt::decrypt(const std::string& stringToDecrypt, const RSA::PrivateKey& privateKey)
{
   LOG4CPLUS_INFO(msLogger, "decrypt()");

   AutoSeededRandomPool rng;
   std::string recovered;

   LOG4CPLUS_INFO(msLogger, "Input string to decrypt: " + stringToDecrypt);

   RSAES_OAEP_SHA_Decryptor d(privateKey);

   StringSource(stringToDecrypt, true,
      new PK_DecryptorFilter(rng, d,
         new StringSink(recovered)
      ) // PK_DecryptorFilter
   ); // StringSource

   LOG4CPLUS_INFO(msLogger, "Output decrypted string: " + recovered);

   return recovered;
}

CRSAEncryptDecrypt::CRSAEncryptDecrypt()
{

}

CRSAEncryptDecrypt::~CRSAEncryptDecrypt()
{

}
