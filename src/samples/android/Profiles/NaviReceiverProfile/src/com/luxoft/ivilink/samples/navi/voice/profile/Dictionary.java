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
 
package com.luxoft.ivilink.samples.navi.voice.profile;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Queue;

import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class Dictionary {
	private static String tag = Dictionary.class.getName();
	private Map<String, String> translator = new HashMap<String, String>();

	private static Dictionary instance = new Dictionary();

	private Dictionary() {
		translator.put("cherez", "in");
		translator.put("dest", "destination is");
		translator.put("kilometer", "kilometer");
		translator.put("kilometer", "kilometer");
		translator.put("kilometra", "kilometers");
		translator.put("kilometrov", "kilometers");
		translator.put("levee", "left");
		translator.put("meter", "meter");
		translator.put("metra", "meters");
		translator.put("metrov", "meters");
		translator.put("nalevo", "left");
		translator.put("napravo", "right");
		translator.put("povernite", "turn");
		translator.put("pravee", "right");
		translator.put("prodolz_dvizhenie", "keep moving");
	}

	public static Dictionary getInstance() {
		return instance;
	}

	public String translate(String wordToTranslate) {
		Log.v(tag, Logging.getCurrentMethodName(wordToTranslate));
		return translator.get(wordToTranslate);
	}

	public String translatePhrase(List<String> phraseToTranslate) {
		BigLog.i(tag, Logging.getCurrentMethodName(phraseToTranslate), '!');
		StringBuffer translation = new StringBuffer();
		Queue<String> translationQueue = new LinkedList<String>(phraseToTranslate);
		while (!translationQueue.isEmpty()) {
			String translatedWord = translate(translationQueue.peek());
			if (translatedWord == null && translationQueue.peek().startsWith("d")) {
				// parsing multi-worded distance
				int distance = 0;
				while (translationQueue.peek().startsWith("d")
						&& !translationQueue.peek().equals("dest")) {
					try {
						distance += Integer.parseInt(translationQueue.poll().substring(1));
					} catch (NumberFormatException e) {
						e.printStackTrace();
					}
					Log.i(tag, "distance = " + distance);
				}
				translation.append(distance).append(" ");
			} else {
				translation.append(translatedWord).append(" ");
				translationQueue.poll();
			}
		}
		return translation.toString();
	}
}
