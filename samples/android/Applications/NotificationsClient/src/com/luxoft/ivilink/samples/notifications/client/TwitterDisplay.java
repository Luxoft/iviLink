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
package com.luxoft.ivilink.samples.notifications.client;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;

import org.json.JSONException;
import org.json.JSONObject;

import android.app.Activity;
import android.app.Dialog;
import android.content.Context;
import android.media.AudioManager;
import android.os.Bundle;
import android.speech.tts.TextToSpeech;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.TextView;
import android.widget.Toast;

import com.luxoft.ivilink.samples.notifications.JsonConst;
import com.luxoft.ivilink.samples.notifications.Tweet;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

public class TwitterDisplay extends Activity implements TextToSpeech.OnInitListener,
		OnClickListener {

	TextToSpeech tts;
	Tweet latestTweet;

	Dialog dialog;

	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		ForApp.setWindowFlagsToKeepDeviceOn(this);
	}

	@Override
	public void onResume() {
		super.onResume();
		if (getIntent() != null && getIntent().getStringExtra(JsonConst.tweet) != null) {
			try {
				tts = new TextToSpeech(this, this);
				latestTweet = new Tweet(new JSONObject(getIntent().getStringExtra(JsonConst.tweet)));
				dialog = new Dialog(this);

				dialog.setContentView(R.layout.twi_alert);
				dialog.setTitle("New Tweet by @" + latestTweet.getUserName());
				TextView text = (TextView) dialog.findViewById(R.id.message);
				text.setText(latestTweet.getStatusContent());
				LoaderImageView image = (LoaderImageView) dialog.findViewById(R.id.avatar);
				image.setImageDrawable(latestTweet.getAvatarUrl());
				TextView text2 = (TextView) dialog.findViewById(R.id.date);
				text2.setText(latestTweet.getTimeOfCreation());
				dialog.findViewById(R.id.retweet).setOnClickListener(this);
				dialog.findViewById(R.id.readAloud).setOnClickListener(this);
				dialog.findViewById(R.id.readAloud).setEnabled(false);
				dialog.findViewById(R.id.dismiss).setOnClickListener(this);
				dialog.show();
			} catch (JSONException e) {
				e.printStackTrace();
			}
		} else {
			finish();
		}
	}

	@Override
	public void onBackPressed() {
	}

	@Override
	public void onPause() {
		// on home button press or when a new instance is opening
		super.onPause();
		if (tts != null) {
			tts.stop();
			tts.shutdown();
		}
		finish();
	}

	@Override
	public void onDestroy() {
		if (dialog != null && dialog.isShowing()) {
			dialog.dismiss();
		}
		super.onDestroy();
	}

	public void onInit(int status) {
		if (status == TextToSpeech.SUCCESS) {
			dialog.findViewById(R.id.readAloud).setEnabled(true);
			tts.setLanguage(Locale.UK);
			AudioManager am = (AudioManager) getSystemService(Context.AUDIO_SERVICE);
			int amStreamMusicMaxVol = am.getStreamMaxVolume(AudioManager.STREAM_MUSIC);
			am.setStreamVolume(AudioManager.STREAM_SYSTEM,
					(int) ((double) amStreamMusicMaxVol * 2.0), 0);
			if (status == TextToSpeech.SUCCESS && latestTweet != null) {
				speak("A tweet by " + latestTweet.getUserName(), TextToSpeech.QUEUE_FLUSH);
			}
		} else {
			Toast.makeText(this, "For some reasons TTS is not avaliable", Toast.LENGTH_LONG).show();
		}
	}

	private void speak(String phrase, int mode) {
		String hashlessPhrase = phrase.replace("#", "");
		String[] words = hashlessPhrase.split(" ");
		List<String> clearPhrase = new ArrayList<String>();
		for (int i = 0; i < words.length; i++) {
			if (words[i].contains("http:/")) {
				continue;
			}
			if (words[i].equals("RT")) {
				clearPhrase.add("retweet");
			}
			clearPhrase.add(words[i]);
		}
		String result = new String();
		for (int i = 0; i < clearPhrase.size(); i++) {
			if (i == 0) {
				result = clearPhrase.get(i);
				continue;
			}
			result += " " + clearPhrase.get(i);
		}
		Log.i("Twitter Display", "message processing result: " + result);
		tts.speak(result, mode, null);
	}

	public void onClick(View v) {
		switch (v.getId()) {
		case R.id.retweet:
			TweetRequestManager instance;
			if ((instance = TweetRequestManager.getInstance()) != null) {
				instance.requestRetweet(latestTweet.getTweetID());
			}
			break;
		case R.id.readAloud:
			speak(latestTweet.getStatusContent(), TextToSpeech.QUEUE_FLUSH);
			break;
		case R.id.dismiss:
			finish();
			break;
		default:
		}
	}
}
