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

import oauth.signpost.OAuthProvider;
import oauth.signpost.commonshttp.CommonsHttpOAuthConsumer;
import oauth.signpost.exception.OAuthCommunicationException;
import oauth.signpost.exception.OAuthExpectationFailedException;
import oauth.signpost.exception.OAuthMessageSignerException;
import oauth.signpost.exception.OAuthNotAuthorizedException;
import twitter4j.Twitter;
import twitter4j.TwitterFactory;
import twitter4j.http.AccessToken;
import android.app.Activity;
import android.content.Intent;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.AsyncTask;
import android.util.Log;
import android.widget.Toast;

public class CallbackReceiver extends Activity {

	@Override
	public void onResume() {
		super.onResume();
		if (this.getIntent() != null && this.getIntent().getData() != null) {
			Log.i(Commons.CALLBACK_URL, "got intent");
			Uri uri = getIntent().getData();
			if (uri != null && uri.toString().startsWith(Commons.CALLBACK_URL)) {
				final String verifier = uri.getQueryParameter(oauth.signpost.OAuth.OAUTH_VERIFIER);
				Log.i(Commons.CALLBACK_URL, "got intent: verifier = " + verifier);
				new GetAccessTokenTask().execute(verifier);
			}
		}
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}

	class GetAccessTokenTask extends AsyncTask<String, Void, Boolean> {

		@Override
		protected Boolean doInBackground(String... params) {
			if (params == null || params.length < 1) {
				return false;
			}
			OAuthProvider provider = Commons.getInstance().getProvider();
			CommonsHttpOAuthConsumer consumer = Commons.getInstance().getConsumer();
			try {
				provider.retrieveAccessToken(consumer, params[0]);
				AccessToken a = new AccessToken(consumer.getToken(), consumer.getTokenSecret());
				Commons.getInstance().storeAccessToken(a);
				Twitter twitter = (Twitter) (new TwitterFactory().getInstance());
				twitter.setOAuthConsumer(Commons.CONSUMER_KEY, Commons.CONSUMER_SECRET);
				twitter.setOAuthAccessToken(a);
				Commons.getInstance().setTwitter(twitter);
				return true;
			} catch (OAuthMessageSignerException e) {
				e.printStackTrace();
			} catch (OAuthNotAuthorizedException e) {
				e.printStackTrace();
			} catch (OAuthExpectationFailedException e) {
				e.printStackTrace();
			} catch (OAuthCommunicationException e) {
				e.printStackTrace();
			}
			return false;
		}

		@Override
		public void onPostExecute(Boolean result) {
			sendBroadcast(new Intent("finish()"));
			if (!result.booleanValue()) {
				Toast.makeText(CallbackReceiver.this, "Failed to get access token!",
						Toast.LENGTH_LONG).show();
			}
			finish();
		}
	}
}
