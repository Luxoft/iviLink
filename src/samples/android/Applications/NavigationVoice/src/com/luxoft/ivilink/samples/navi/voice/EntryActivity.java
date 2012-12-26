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

import com.luxoft.ivilink.sdk.android.nonnative.CApp;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Process;
import android.speech.tts.TextToSpeech;

public class EntryActivity extends Activity {
	@Override
	public void onCreate(Bundle icicle) {
		super.onCreate(icicle);
		CApp app = new CApp(this, Shared.SERVICE_NAME);
		Shared.cappInstance = app;
		startService(new Intent(this, NaviVoiceService.class));
		/*
		 * Intent checkTTSIntent = new Intent();
		 * checkTTSIntent.setAction(TextToSpeech.Engine.ACTION_CHECK_TTS_DATA);
		 * startActivityForResult(checkTTSIntent, 0);
		 */
	}

	@Override
	public void onResume() {
		super.onResume();
		finish();
	}

	/*
	 * protected void onActivityResult(int requestCode, int resultCode, Intent
	 * data) { if (resultCode == TextToSpeech.Engine.CHECK_VOICE_DATA_PASS) {
	 * finish(); } else { // no data - install it now Intent installTTSIntent =
	 * new Intent();
	 * installTTSIntent.setAction(TextToSpeech.Engine.ACTION_INSTALL_TTS_DATA);
	 * startActivity(installTTSIntent); } }
	 */
}
