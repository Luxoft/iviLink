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

#include "CGuiController.hpp"

CGuiController::CGuiController()
{
	mLogger = Logger::getInstance("samples.Applications.Media.CMediaSource.CEmitter");
}

void CGuiController::setSync()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalSetSync();
}

void CGuiController::setUnsync()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalSetUnsync();
}

void CGuiController::setSyncDisabled()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
    emit signalSetSyncDisabled();
}

void CGuiController::deactivateToggle()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalDeactivateToggle();
}

void CGuiController::activateToggle()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalActivateToggle();
}

void CGuiController::highlightToggle()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalHighlightToggle();
}

void CGuiController::showPlayButton()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalShowPlayButton();
}

void CGuiController::showPlayIcon()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalShowPlayIcon();
}

void CGuiController::showPauseIcon()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalShowPauseIcon();
}

void CGuiController::hideIcon()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalHideIcon();
}

void CGuiController::showPauseButton()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalShowPauseButton();
}

void CGuiController::playlistPopulate(const std::string& songName, const bool isLocal, const bool isAudio)
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	QString name = QString::fromStdString(songName);
	emit signalAudioListPopulate(name, isLocal);
}

void CGuiController::playlistClear()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	emit signalAudioListClear();
}

void CGuiController::playlistPosChanged(int pos)
{
	emit signalPlaylistPosChanged(pos);
}

void CGuiController::lockGUI()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	LOG4CPLUS_WARN(mLogger, "Empty stub");
}

void CGuiController::unlockGUI()
{
    LOG4CPLUS_TRACE_METHOD(mLogger, __PRETTY_FUNCTION__);
	LOG4CPLUS_WARN(mLogger, "Empty stub");
}
