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
package com.luxoft.ivilink.samples.notifications.server;

import oauth.signpost.commonshttp.CommonsHttpOAuthConsumer;
import oauth.signpost.commonshttp.CommonsHttpOAuthProvider;
import oauth.signpost.exception.OAuthCommunicationException;
import oauth.signpost.exception.OAuthExpectationFailedException;
import oauth.signpost.exception.OAuthMessageSignerException;
import oauth.signpost.exception.OAuthNotAuthorizedException;
import twitter4j.Twitter;
import twitter4j.TwitterFactory;
import twitter4j.http.AccessToken;
import android.app.Activity;
import android.app.Dialog;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.webkit.WebView;
import android.widget.Toast;

import com.luxoft.ivilink.samples.notifications.IntentTags;
import com.luxoft.ivilink.samples.notifications.server.twitter.TweetsActivity;
import com.luxoft.ivilink.samples.splash.Splash;
import com.luxoft.ivilink.samples.splash.Splash.Length;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

public class AuthActivity extends Activity {
	private final static String tag = Commons.tag + "AuthActivity";

	WebView loginView;
	BroadcastReceiver shutDownReceiver;
	Handler poster = new Handler();

	@Override
	public void onCreate(Bundle savedInstanceState) {
		System.setProperty("http.keepAlive", "false");
		Log.i(tag, "created!");
		super.onCreate(savedInstanceState);
		setContentView(R.layout.main_oauth);
		loginView = (WebView) findViewById(R.id.loginScreen);
		shutDownReceiver = new ShutdownReceiver();
		registerReceiver(shutDownReceiver, new IntentFilter("finish()"));
		Commons.getInstance().setContext(this);
		if (!Commons.hasAuthActivityStarted) {
			Commons.hasAuthActivityStarted = true;
			startService();
			Splash.displaySplashScreen(this, new Runnable() {
				public void run() {
					checkForSavedLogin();
				}
			}, Length.SHORT);
		} else {
			Log.e(tag, "for some reason getting onCreate twice");
			if (Commons.getInstance().getTwitter() != null) {
				// this means we are authorized
				startActivity(new Intent(this, TweetsActivity.class));
				finish();
			}
		}
	}

	@Override
	public void onBackPressed() {

	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		if (loginView != null) {
			loginView.setVisibility(View.GONE);
		}
		unregisterReceiver(shutDownReceiver);
		Log.w(tag, "onDestroy()");
	}

	private void checkForSavedLogin() {
		Log.v(tag, "checkForSavedLogin()");
		AccessToken a = Commons.getInstance().getAccessToken();
		if (a == null) {
			Log.i(tag, "no access token saved, will try to login");
			askOAuth();
		} else {
			Log.i(tag, "has an access token saved!");
			Twitter twitter = (Twitter) (new TwitterFactory().getInstance());
			twitter.setOAuthConsumer(Commons.CONSUMER_KEY, Commons.CONSUMER_SECRET);
			twitter.setOAuthAccessToken(a);
			Commons.getInstance().setTwitter(twitter);
			finish();
		}
	}

	private void askOAuth() {
		Log.v(tag, "askOAuth()");
		new GetAuthUrlTask().execute((Void) null);
	}

	private void startService() {
		Log.v(tag, "startService()");
		startService(new Intent(this, DataServiceServerImplementation.class).putExtra(
				IntentTags.launchInfo, ForApp.getLaunchInfo(this)).putExtra(IntentTags.serviceName,
				"NotificationsServerService"));
	}

	class ShutdownReceiver extends BroadcastReceiver {
		@Override
		public void onReceive(Context context, Intent intent) {
			AuthActivity.this.finish();
		}
	}

	class GetAuthUrlTask extends AsyncTask<Void, Void, String> {
		Dialog progressDialog;

		@Override
		public void onPreExecute() {
			progressDialog = ProgressDialog.show(AuthActivity.this, "",
					"Preparing Twitter authentication...");
			Toast.makeText(AuthActivity.this, "Please authorize this app!", Toast.LENGTH_LONG)
					.show();
		}

		@Override
		protected String doInBackground(Void... params) {
			Log.v(tag, "doInBackground(Void... )");
			CommonsHttpOAuthConsumer consumer = new CommonsHttpOAuthConsumer(Commons.CONSUMER_KEY,
					Commons.CONSUMER_SECRET);
			Commons.getInstance().setConsumer(consumer);
			CommonsHttpOAuthProvider provider = new CommonsHttpOAuthProvider(
					"https://api.twitter.com/oauth/request_token",
					"http://api.twitter.com/oauth/access_token", "http://api.twitter.com/oauth/authorize");
			Commons.getInstance().setProvider(provider);
			String authUrl;
			try {
				authUrl = provider.retrieveRequestToken(consumer, Commons.CALLBACK_URL);
				Log.v(tag, "authUrl: " + authUrl);
				return authUrl;
			} catch (OAuthMessageSignerException e) {
				e.printStackTrace();
			} catch (OAuthNotAuthorizedException e) {
				e.printStackTrace();
			} catch (OAuthExpectationFailedException e) {
				e.printStackTrace();
			} catch (OAuthCommunicationException e) {
				e.printStackTrace();
			}
			return null;
		}

		@Override
		public void onPostExecute(String result) {
			progressDialog.dismiss();
			if (result != null) {
				loginView.loadUrl(result);
			} else {
				Toast.makeText(AuthActivity.this, "Failed to retrieve Auth URL!", Toast.LENGTH_LONG)
						.show();
				finish();
			}
		}

	}
}
