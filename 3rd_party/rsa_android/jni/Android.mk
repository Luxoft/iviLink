#  iviLINK SDK, version 0.9 (for preview only)                                      
#    http://www.ivilink.net                                                         
#  Cross Platform Application Communication Stack for In-Vehicle Applications       
#                                                                                   
#  Copyright (C) 2012, Luxoft Professional Corp., member of IBS group               
#                                                                                   
#  This library is free software; you can redistribute it and/or                    
#  modify it under the terms of the GNU Lesser General Public                       
#  License as published by the Free Software Foundation; version 2.1.               
#                                                                                   
#  This library is distributed in the hope that it will be useful,                  
#  but WITHOUT ANY WARRANTY; without even the implied warranty of                   
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                
#  Lesser General Public License for more details.                                  
#                                                                                   
#  You should have received a copy of the GNU Lesser General Public                 
#  License along with this library; if not, write to the Free Software              
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA   
# 
# 
# 
# 

LOCAL_PATH := $(call my-dir)

#lib for authentication
include $(CLEAR_VARS)
LOCAL_MODULE := cryptoppRsa
PREFIX := ../../cryptopp/cryptopp-5.6.1
LOCAL_SRC_FILES += $(PREFIX)/algebra.cpp
LOCAL_SRC_FILES += $(PREFIX)/algparam.cpp
LOCAL_SRC_FILES += $(PREFIX)/asn.cpp
LOCAL_SRC_FILES += $(PREFIX)/basecode.cpp
LOCAL_SRC_FILES += $(PREFIX)/cpu.cpp
LOCAL_SRC_FILES += $(PREFIX)/cryptlib.cpp
LOCAL_SRC_FILES += $(PREFIX)/dll.cpp
LOCAL_SRC_FILES += $(PREFIX)/dsa.cpp
LOCAL_SRC_FILES += $(PREFIX)/ec2n.cpp
LOCAL_SRC_FILES += $(PREFIX)/ecp.cpp
LOCAL_SRC_FILES += $(PREFIX)/filters.cpp
LOCAL_SRC_FILES += $(PREFIX)/fips140.cpp
LOCAL_SRC_FILES += $(PREFIX)/gf2n.cpp
LOCAL_SRC_FILES += $(PREFIX)/gfpcrypt.cpp
LOCAL_SRC_FILES += $(PREFIX)/hex.cpp
LOCAL_SRC_FILES += $(PREFIX)/hmac.cpp
LOCAL_SRC_FILES += $(PREFIX)/hrtimer.cpp
LOCAL_SRC_FILES += $(PREFIX)/integer.cpp
LOCAL_SRC_FILES += $(PREFIX)/iterhash.cpp
LOCAL_SRC_FILES += $(PREFIX)/misc.cpp
LOCAL_SRC_FILES += $(PREFIX)/modes.cpp
LOCAL_SRC_FILES += $(PREFIX)/mqueue.cpp
LOCAL_SRC_FILES += $(PREFIX)/nbtheory.cpp
LOCAL_SRC_FILES += $(PREFIX)/oaep.cpp
LOCAL_SRC_FILES += $(PREFIX)/osrng.cpp
LOCAL_SRC_FILES += $(PREFIX)/pkcspad.cpp
LOCAL_SRC_FILES += $(PREFIX)/pubkey.cpp
LOCAL_SRC_FILES += $(PREFIX)/queue.cpp
LOCAL_SRC_FILES += $(PREFIX)/randpool.cpp
LOCAL_SRC_FILES += $(PREFIX)/rdtables.cpp
LOCAL_SRC_FILES += $(PREFIX)/rijndael.cpp
LOCAL_SRC_FILES += $(PREFIX)/rng.cpp
LOCAL_SRC_FILES += $(PREFIX)/rsa.cpp
LOCAL_SRC_FILES += $(PREFIX)/sha.cpp
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(PREFIX)
include $(BUILD_STATIC_LIBRARY)

#forces ndk-build actually do smth
include $(CLEAR_VARS)
LOCAL_MODULE := cryptoppRsaShared
LOCAL_STATIC_LIBRARIES := cryptoppRsa
include $(BUILD_SHARED_LIBRARY)



