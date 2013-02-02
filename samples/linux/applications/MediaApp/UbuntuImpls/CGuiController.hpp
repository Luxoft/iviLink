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
#ifndef C_GUICONTROLLER_HPP
#define C_GUICONTROLLER_HPP

#include <QObject>
#include <QString>
#include <string>

#include "IGuiController.hpp"
#include "Logger.hpp"

class CGuiController : public QObject,
				 public IGuiController
{
	Q_OBJECT
public:
	CGuiController();

	virtual void setSync();
	virtual void setUnsync();
    virtual void setSyncDisabled();
	virtual void deactivateToggle();
	virtual void activateToggle();
    virtual void highlightToggle();
	virtual void showPlayButton();
	virtual void showPauseButton();
	virtual void playlistPopulate(const std::string& songName, const bool isLocal, const bool isAudio);
	virtual void playlistClear();
	virtual void playlistPosChanged(int pos);
    virtual void showPlayIcon();
    virtual void hideIcon();
    virtual void showPauseIcon();
    virtual void lockGUI();
    virtual void unlockGUI();

	virtual ~CGuiController() {}

signals:

    void signalSetSync();
    void signalSetUnsync();
    void signalSetSyncDisabled();
    void signalDeactivateToggle();
    void signalHighlightToggle();
    void signalActivateToggle();
    void signalShowPlayButton();
    void signalShowPauseButton();

    void signalAudioListPopulate(QString songName, bool isLocal);
    void signalAudioListClear();
    void signalPlaylistPosChanged(int pos);
    void signalShowPlayIcon();
    void signalHideIcon();
    void signalShowPauseIcon();

private:
	Logger mLogger;
};

#endif //C_GUICONTROLLER_HPP
