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
#ifndef I_GUI_CONTROLLER_HPP
#define I_GUI_CONTROLLER_HPP

#include <string>

class IGuiController
{
public:
    virtual ~IGuiController()
    {
    }
    virtual void setSync() = 0;
    virtual void setUnsync() = 0;
    virtual void deactivateToggle() = 0;
    virtual void activateToggle() = 0;
    virtual void highlightToggle() = 0;
    virtual void showPlayButton() = 0;
    virtual void showPauseButton() = 0;
    virtual void playlistPopulate(const std::string& name, const bool isLocal,
            const bool isAudio) = 0;
    virtual void playlistClear() = 0;
    virtual void playlistPosChanged(int pos) = 0;
    virtual void showPlayIcon() = 0;
    virtual void hideIcon() = 0;
    virtual void showPauseIcon() = 0;
    // show some pop up that the service is being loaded
    virtual void lockGUI() = 0;
    // hide the pop up
    virtual void unlockGUI() = 0;
};

#endif //I_GUI_CONTROLLER_HPP
