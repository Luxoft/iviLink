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
 

#include <cassert>

#include "CAndroidGuiControllerImpl.hpp"
#include "JniThreadHelper.hpp"
#include "StringConversion.hpp"

CAndroidGuiControllerImpl::CAndroidGuiControllerImpl(JavaVM * pJM, jobject callbacks)
:
	mpJM(pJM),
	mjCallbacks(callbacks),
	mLogger(Logger::getInstance("iviLink.samples.Media.CAndroidGuiControllerImpl")),
	hasRequestedLock(false)
{
	LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	assert(mpJM);
	iviLink::Android::JniThreadHelper jth(mpJM);
	JNIEnv * env = jth.getEnv();
	jclass claz = env->GetObjectClass(mjCallbacks);
	midSetSyncState = env->GetMethodID(claz, "setSyncState", "(Z)V");
	assert(midSetSyncState != NULL);
	midSetToggleState = env->GetMethodID(claz, "setToggleState", "(Z)V");
	assert(midSetToggleState != NULL);
	midShowPauseButton = env->GetMethodID(claz, "pauseVisible", "()V");
	assert(midShowPauseButton != NULL);
	midShowPlayButton = env->GetMethodID(claz, "playVisible", "()V");
	assert(midShowPlayButton != NULL);
	midClearList = env->GetMethodID(claz, "clearPlayList", "()V");
	assert(midClearList != NULL);
	midPopulateList = env->GetMethodID(claz, "populateList", "(Ljava/lang/String;Z)V");
	assert(midPopulateList != NULL);
	midHighlightPosition = env->GetMethodID(claz, "setSelectedPosition", "(I)V");
	assert(midHighlightPosition != NULL);

	midShowPlayIcon  = env->GetMethodID(claz, "showPlayIcon", "()V");
	midShowPauseIcon  = env->GetMethodID(claz, "showPauseIcon", "()V");
	midHideIcon   = env->GetMethodID(claz, "showNoIcon", "()V");
	midHighlightToggle   = env->GetMethodID(claz, "highlightToggle", "()V");

	midUnlockGUI = env->GetMethodID(claz, "unlockGUI", "()V");
	midLockGUI = env->GetMethodID(claz, "lockGUI", "()V");
}

void CAndroidGuiControllerImpl::setSync()
{
	callBoolVoidJavaMethod(midSetSyncState, true);
}

void CAndroidGuiControllerImpl::setUnsync()
{
	callBoolVoidJavaMethod(midSetSyncState, false);
}

void CAndroidGuiControllerImpl::deactivateToggle()
{
	callBoolVoidJavaMethod(midSetToggleState, false);
}

void CAndroidGuiControllerImpl::highlightToggle()
{
    callVoidVoidJavaMethod(midHighlightToggle);
}

void CAndroidGuiControllerImpl::showPlayIcon()
{
    callVoidVoidJavaMethod(midShowPlayIcon);
}

void CAndroidGuiControllerImpl::hideIcon()
{
    callVoidVoidJavaMethod(midHideIcon);
}

void CAndroidGuiControllerImpl::showPauseIcon()
{
    callVoidVoidJavaMethod(midShowPauseIcon);
}

void CAndroidGuiControllerImpl::activateToggle()
{
	callBoolVoidJavaMethod(midSetToggleState, true);
}

void CAndroidGuiControllerImpl::showPlayButton()
{
	callVoidVoidJavaMethod(midShowPlayButton);
}

void CAndroidGuiControllerImpl::showPauseButton()
{
	callVoidVoidJavaMethod(midShowPauseButton);
}

void CAndroidGuiControllerImpl::audioListClear()
{
	callVoidVoidJavaMethod(midClearList);
}


void CAndroidGuiControllerImpl::audioListPopulate(const std::string& songName, const bool isLocal)
{
	iviLink::Android::JniThreadHelper jth(mpJM);
	JNIEnv * env = jth.getEnv();
	jstring param = iviLink::Android::StringConversion::stdToJ(songName, env);
	env->CallVoidMethod(mjCallbacks, midPopulateList, param, static_cast<jboolean>(isLocal));
}

void CAndroidGuiControllerImpl::playlistPosChanged(int position)
{
	iviLink::Android::JniThreadHelper jth(mpJM);
	JNIEnv * env = jth.getEnv();
	env->CallVoidMethod(mjCallbacks, midHighlightPosition, static_cast<jint>(position));
}

void CAndroidGuiControllerImpl::callVoidVoidJavaMethod(jmethodID mid)
{
	iviLink::Android::JniThreadHelper jth(mpJM);
	JNIEnv * env = jth.getEnv();
	env->CallVoidMethod(mjCallbacks, mid);
}

void CAndroidGuiControllerImpl::callBoolVoidJavaMethod(jmethodID mid, bool param)
{
	iviLink::Android::JniThreadHelper jth(mpJM);
	JNIEnv * env = jth.getEnv();
	env->CallVoidMethod(mjCallbacks, mid, static_cast<jboolean>(param));
}

void CAndroidGuiControllerImpl::lockGUI()
{
	callVoidVoidJavaMethod(midLockGUI);
}

void CAndroidGuiControllerImpl::unlockGUI()
{
	callVoidVoidJavaMethod(midUnlockGUI);
}
