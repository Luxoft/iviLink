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
package com.luxoft.ivilink.samples.media.players.ipc;

import com.luxoft.ivilink.samples.media.interfaces.MediaGui;

public class CommunicationProtocol {
    // intent extra tags
    public final static String MESSAGE = "MESSAGE";
    public final static String EXTRA_INFO = "ADDINFO";

    // intent filters
    public final static String TO_SERVICE = FromLogicToPlayer.class.getName();
    public final static String TO_BUTTON_PROCESSOR = FromPlayerToPlayerLogic.class.getName();
    public final static String TO_GUI_SERVER = MediaGui.Server.class.getName();
    public final static String TO_GUI_CLIENT = MediaGui.Client.class.getName();
    public final static String METHOD = "METHOD";
    public final static String ARGS = "ARGS";
    public final static String AUDIO = "AUDIO";
    public final static String VIDEO = "VIDEO";

    public enum FromLogicToPlayer {
        PLAY, PAUSE, RESUME, STOP;

        public static FromLogicToPlayer fromString(String text) {
            return CommunicationProtocol.fromString(FromLogicToPlayer.class, text);
        }
    }

    public enum FromPlayerToPlayerLogic {
        COMPLETED, ERROR, CONNECTED;
        public static FromPlayerToPlayerLogic fromString(String text) {
            return CommunicationProtocol.fromString(FromPlayerToPlayerLogic.class, text);
        }
    }

    public static <T extends Enum<T>> T fromString(Class<T> c, String string) {
        if (c != null && string != null) {
            try {
                return Enum.valueOf(c, string.trim().toUpperCase());
            } catch (IllegalArgumentException ex) {
                ex.printStackTrace();
            }
        }
        return null;
    }
}
