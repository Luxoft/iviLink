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
package com.luxoft.ivilink.samples.notifications.server.twitter;

import java.util.List;

import twitter4j.Status;
import twitter4j.TwitterException;
import android.app.ListActivity;
import android.app.ProgressDialog;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.res.Configuration;
import android.graphics.Typeface;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.util.TypedValue;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ListView;
import android.widget.Toast;

import com.luxoft.ivilink.samples.notifications.server.Commons;
import com.luxoft.ivilink.samples.notifications.server.R;
import com.luxoft.ivilink.samples.notifications.server.TwitterServerRequestManager;
import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;

public class TweetsActivity extends ListActivity implements OnClickListener {
	private final String tag = "iviLink.samples.Notifications.TweetsActivity";
	private TwitterListAdapter adapter;
	BroadcastReceiver updater;
	private final static int FIRST_PAGE = 1; // not 0!
	// static for persistence between different instances of this Activity
	static int currentPage = FIRST_PAGE;

	private final String REFRESH_UTF = "\u27F2";

	ProgressDialog progress;

	Button backRefresh, forward;

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Log.v(tag, "onCreate()");
		setContentView(R.layout.main);
		backRefresh = (Button) findViewById(R.id.refresh);
		Typeface font = Typeface.createFromAsset(getAssets(), "DejaVuSans-Bold.ttf");
		backRefresh.setTypeface(font);
		forward = (Button) findViewById(R.id.forward);
		forward.setTypeface(font);

		forward.setOnClickListener(this);
		backRefresh.setOnClickListener(this);

		forward.setTextSize(20);
		backRefresh.setTextSize(20);
		setButtonsText();
		ForApp.setWindowFlagsToKeepDeviceOn(this);
		Toast noti = Toast.makeText(this, "Click any tweet to view it on the other device!",
				Toast.LENGTH_LONG);
		noti.setGravity(Gravity.CENTER_HORIZONTAL | Gravity.CENTER_VERTICAL, 0, 0);
		noti.show();
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
	}

	@Override
	public void onResume() {
		super.onResume();
		Log.v(tag, "onResume()");
		updater = new UpdatesReceiver();
		registerReceiver(updater, new IntentFilter(TwitterServerRequestManager.INTENT_ACTION));
		new GetTwitterTimeline().execute(currentPage);
	}

	@Override
	public void onBackPressed() {
		finish();
	}

	@Override
	public void onPause() {
		super.onPause();
		finish();
	}

	@Override
	public void onStop() {
		super.onStop();
		if (progress != null && progress.isShowing()) {
			progress.dismiss();
		}
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		unregisterReceiver(updater);
	}

	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {
		Log.v(tag, "item " + position + " was clicked!");
		Status status = (Status) getListView().getItemAtPosition(position);
		TwitterServerRequestManager.getInstance().sendStatus(status);
	}

	private void setButtonsText() {
		if (currentPage == FIRST_PAGE) {
			backRefresh.setText(REFRESH_UTF);
		} else {
			backRefresh.setText("<< \n(" + (currentPage - 1) + ")");
		}
		forward.setText(">> \n(" + (currentPage + 1) + ")");
	}

	class UpdatesReceiver extends BroadcastReceiver {

		@Override
		public void onReceive(Context context, Intent intent) {
			if (currentPage == FIRST_PAGE) {
				Toast.makeText(TweetsActivity.this, "Timeline updated automatically!",
						Toast.LENGTH_LONG).show();
				synchronized (Commons.class) {
					adapter = new TwitterListAdapter(TweetsActivity.this, Commons.getInstance()
							.getTwitterTimeline());
					setListAdapter(adapter);
				}
			}
		}
	}

	class GetTwitterTimeline extends AsyncTask<Integer, Void, Boolean> {
		List<twitter4j.Status> updatedStatuses;
		int pageToRetrieve;

		@Override
		public void onPreExecute() {
			progress = new ProgressDialog(TweetsActivity.this);
			progress.setTitle("");
			progress.setMessage("Downloading your timeline...");
			progress.setCanceledOnTouchOutside(false);
			progress.setCancelable(false);
			progress.show();
		}

		@Override
		protected Boolean doInBackground(Integer... params) {
			if (params == null || params.length < 1) {
				return false;
			}
			/*
			 * just to avoid crashes from twitter4j - reduce frequency of
			 * requests a bit
			 */
			try {
				Thread.sleep(2000);
			} catch (InterruptedException e) {
				e.printStackTrace();
				if (isCancelled()) {
					return false;
				}
			}
			if (isCancelled()) {
				return false;
			}
			pageToRetrieve = params[0];
			try {
				updatedStatuses = TweetReader.retrieveSpecificUsersTweets(Commons.getInstance()
						.getTwitter(), params[0]);
			} catch (TwitterException e) {
				e.printStackTrace();
			}
			if (isCancelled()) {
				return false;
			}
			return updatedStatuses == null;
		}

		@Override
		public void onCancelled() {
			Log.v(tag + ".Task", "Cancelled!");
			if (progress != null && progress.isShowing()) {
				progress.dismiss();
			}
		}

		@Override
		public void onPostExecute(Boolean result) {
			if (progress != null && progress.isShowing()) {
				progress.dismiss();
			}
			if (updatedStatuses == null) {
				Toast.makeText(TweetsActivity.this, "Sorry! Could not retrieve your timeline!",
						Toast.LENGTH_LONG).show();
				synchronized (Commons.class) {
					if (Commons.getInstance().getTwitterTimeline() == null) {
						Log.e(tag,
								"Could not retrieve the timeline, for some reason have none cached.");
					} else {
						Log.e(tag,
								"Could not retrieve the timeline, but have one cached and will apply it.");
						adapter = new TwitterListAdapter(TweetsActivity.this, Commons.getInstance()
								.getTwitterTimeline());
						setListAdapter(adapter);
					}
				}
				currentPage = FIRST_PAGE;
				setButtonsText();
			} else {
				if (pageToRetrieve == FIRST_PAGE) {
					synchronized (Commons.class) {
						Commons.getInstance().setTwitterTimeline(updatedStatuses);
					}
				}
				adapter = new TwitterListAdapter(TweetsActivity.this, updatedStatuses);
				setListAdapter(adapter);
			}
		}
	}

	public void onClick(View v) {
		if (v.getId() == backRefresh.getId()) {
			Log.v(tag, "backRefresh clicked; currentPage = " + currentPage);
			if (currentPage > FIRST_PAGE) {
				currentPage--;
			}
		} else if (v.getId() == forward.getId()) {
			Log.v(tag, "forward clicked; currentPage = " + currentPage);
			currentPage++;
		} else {
			return;
		}
		setButtonsText();
		new GetTwitterTimeline().execute(currentPage);
	}
}
