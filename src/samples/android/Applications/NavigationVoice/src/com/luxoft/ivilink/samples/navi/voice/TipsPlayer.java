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
 
package com.luxoft.ivilink.samples.navi.voice;

import java.util.Locale;

import android.content.Context;
import android.media.AudioManager;
import android.os.Handler;
import android.speech.tts.TextToSpeech;
import android.speech.tts.TextToSpeech.OnInitListener;
import android.util.Log;
import android.view.Gravity;
import android.widget.Toast;

import com.luxoft.ivilink.samples.navi.voice.api.NaviVoiceAPI.NaviVoiceApplicationAPI;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class TipsPlayer implements TextToSpeech.OnInitListener, NaviVoiceApplicationAPI {
	private final static String tag = TipsPlayer.class.getName();

	private TextToSpeech tts;
	private Handler poster;
	private Context context;

	private static TipsPlayer instance;

	private TipsPlayer(Context context, Handler poster) {
		this.context = context;
		this.poster = poster;
		tts = new TextToSpeech(context, (OnInitListener) this);
		BigLog.e(tag, tts.getDefaultEngine(), '1');
	}

	public static TipsPlayer getInstance(Context context, Handler poster) {
		synchronized (TipsPlayer.class) {
			if (instance == null) {
				instance = new TipsPlayer(context, poster);
			}
		}
		return instance;
	}

	public void receiveTip(final String phrase) {
		Log.v(tag, Logging.getCurrentMethodName(phrase));
		poster.post(new Runnable() {
			public void run() {
				tts.speak(phrase, TextToSpeech.QUEUE_FLUSH, null);
				Toast noti = Toast.makeText(context,
						phrase.substring(0, 1).toUpperCase() + phrase.substring(1),
						Toast.LENGTH_LONG);
				noti.setGravity(Gravity.CENTER_HORIZONTAL | Gravity.CENTER_VERTICAL, 0, 0);
				noti.show();
			}
		});
	}

	public void onInit(int status) {
		Log.v(tag, Logging.getCurrentMethodName(status));
		if (status != TextToSpeech.SUCCESS) {
			Log.e(tag, "TTS not avaliable!");
			android.os.Process.killProcess(android.os.Process.myPid());
		} else {
			tts.setLanguage(Locale.UK);
			AudioManager am = (AudioManager) context.getSystemService(Context.AUDIO_SERVICE);
			int amStreamMusicMaxVol = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
			am.setStreamVolume(AudioManager.STREAM_SYSTEM,
					(int) ((double) amStreamMusicMaxVol * 2.0), 0);
		}
	}
}
