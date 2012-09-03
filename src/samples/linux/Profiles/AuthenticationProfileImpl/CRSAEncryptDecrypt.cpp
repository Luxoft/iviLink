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







#include "CRSAEncryptDecrypt.hpp"

#include "3rd_party/cryptopp/cryptopp-5.6.1/filters.h"
using CryptoPP::PK_EncryptorFilter;
using CryptoPP::PK_DecryptorFilter;
using CryptoPP::StringSink;
using CryptoPP::StringSource;

#include "3rd_party/cryptopp/cryptopp-5.6.1/osrng.h"
using CryptoPP::AutoSeededRandomPool;

using CryptoPP::RSAES_OAEP_SHA_Encryptor;
using CryptoPP::RSAES_OAEP_SHA_Decryptor;

Logger CRSAEncryptDecrypt::msLogger = Logger::getInstance(LOG4CPLUS_TEXT("profiles.authentication.CRSAEncryptDecrypt"));
RSA::PrivateKey CRSAEncryptDecrypt::rsaPrivate = RSA::PrivateKey();
bool CRSAEncryptDecrypt::privateKeyGenerated = false;

RSA::PublicKey CRSAEncryptDecrypt::getPublicKey()
{
   LOG4CPLUS_INFO(msLogger, "getPublicKey()");

   RSA::PublicKey rsaPublic(getPrivateKey());

   LOG4CPLUS_INFO(msLogger, "Successfully generated and saved RSA keys");

   return rsaPublic;
}

RSA::PrivateKey CRSAEncryptDecrypt::getPrivateKey()
{
   LOG4CPLUS_INFO(msLogger, "getPrivate()");

   if(!privateKeyGenerated)
   {
      AutoSeededRandomPool rnd;
      rsaPrivate.GenerateRandomWithKeySize(rnd, 2048);

      privateKeyGenerated = true;
   }

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
