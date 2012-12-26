/* 
 * iviLINK SDK, version 1.1.19
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
 */ 
 

#include <sys/time.h>
#include <fstream>

#include "Types.hpp"
#include "CRSAEncryptDecrypt.hpp"

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

RSA::PublicKey CRSAEncryptDecrypt::getPublicKey(std::string pathToKeyStorage)
{
   LOG4CPLUS_INFO(msLogger, "getPublicKey()");

   RSA::PublicKey rsaPublic(getPrivateKey(pathToKeyStorage));

   LOG4CPLUS_INFO(msLogger, "Successfully generated and saved RSA keys");

   return rsaPublic;
}

RSA::PrivateKey CRSAEncryptDecrypt::getPrivateKey(std::string pathToKeyStorage)
{
   LOG4CPLUS_INFO(msLogger, "getPrivate()");
   timeval begin, end;
   double elapsedTime;
   gettimeofday(&begin, NULL);
   if(!privateKeyGenerated)
   {
      //try to read it from file
      std::string filePath = pathToKeyStorage+"/private.k";
      LOG4CPLUS_INFO(msLogger, "getPrivate(): filepath="+filePath);

      ifstream inFile;
      size_t size = 0;

      inFile.open(filePath.c_str(), ios::in|ios::binary|ios::ate );
      if(inFile.good()) // the file is there and is readable
      {
         LOG4CPLUS_INFO(msLogger, "getPrivate(): retrieving previously stored data");
         char* signedData = 0;
         inFile.seekg(0, ios::end); // set the pointer to the end
         size = inFile.tellg() ; // get the length of the file
         inFile.seekg(0, ios::beg); // set the pointer to the beginning

         signedData = new char[size]; 
         inFile.read( signedData, size );
         inFile.close();
         UInt8 * unSignedData = new UInt8[size];
         memcpy(unSignedData, signedData, size);
         CryptoPP::ByteQueue queue;
         queue.Put(unSignedData,  size);
         rsaPrivate.Load(queue);
         delete[] signedData;
         delete[] unSignedData;
      } 
      else // the file is not there
      {
         LOG4CPLUS_INFO(msLogger, "getPrivate(): no key stored, generating");
         AutoSeededRandomPool rnd;
         rsaPrivate.GenerateRandomWithKeySize(rnd, 2048);
         // storing in file
         LOG4CPLUS_INFO(msLogger, "getPrivate(): saving generated file");
         CryptoPP::ByteQueue queue;
         rsaPrivate.Save(queue);
         UInt32 queueSize = queue.CurrentSize();
         UInt8* outgoingData = new UInt8[queueSize];
         queue.Get(outgoingData, queueSize);
         std::ofstream outputBufferHere(filePath.c_str(), std::ios::binary|std::ios::out);
         char * signedOutputData = new char[queueSize];
         memcpy(signedOutputData, outgoingData, queueSize);
         outputBufferHere.write(signedOutputData, queueSize);
         outputBufferHere.close();
         delete[] outgoingData;
         delete[] signedOutputData;
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

string CRSAEncryptDecrypt::encrypt(string stringToEncrypt, RSA::PublicKey publicKey)
{
   LOG4CPLUS_INFO(msLogger, "encrypt()");

   AutoSeededRandomPool rng;
   string cipher;

   LOG4CPLUS_INFO(msLogger, "Input string to encrypt: " + stringToEncrypt);

   ////////////////////////////////////////////////
   // Encryption
   RSAES_OAEP_SHA_Encryptor e(publicKey);

   StringSource(stringToEncrypt, true,
       new PK_EncryptorFilter(rng, e,
       new StringSink(cipher)
    ) // PK_EncryptorFilter
   ); // StringSource

   LOG4CPLUS_INFO(msLogger, "Output encrypted string: " + string(cipher.c_str()));

   return cipher;
}

string CRSAEncryptDecrypt::decrypt(string stringToDecrypt, RSA::PrivateKey privateKey)
{
   LOG4CPLUS_INFO(msLogger, "decrypt()");

   AutoSeededRandomPool rng;
   string recovered;

   LOG4CPLUS_INFO(msLogger, "Input string to decrypt: " + stringToDecrypt);

   ////////////////////////////////////////////////
   // Decryption
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
