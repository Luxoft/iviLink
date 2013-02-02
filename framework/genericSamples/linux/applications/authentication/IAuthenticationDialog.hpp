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
#ifndef I_AUTHENTICATION_DIALOG_HPP
#define I_AUTHENTICATION_DIALOG_HPP

namespace iviLink
{
namespace Authentication
{

class IAuthenticationDialog
{
public:
    /**
     * Show the dialog with inputbox
     */
    virtual void showNumPad() = 0;
    /**
     * Hide the dialog with inputbox
     */
    virtual void hideNumPad() = 0;
    /**
     * Make some notification to user that he should reattempt
     */
    virtual void showRetryPopup() = 0;
    /**
     * Line with entered pin should be cleared
     */
    virtual void resetNumPad() = 0;
    /**
     * Cannot press number buttons or ok,
     * can press Cancel though in this state
     */
    virtual void lockInput() = 0;
    /**
     * Can press number buttons, ok and cancel
     */ 
    virtual void unlockInput() = 0;
};

} //namespace Authentication
} //namespace iviLink

#endif //I_AUTHENTICATION_DIALOG_HPP
