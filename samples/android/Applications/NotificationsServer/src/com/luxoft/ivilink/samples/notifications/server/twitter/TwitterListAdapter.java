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
import android.app.Activity;
import android.text.Html;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

import com.luxoft.ivilink.samples.notifications.server.R;

public class TwitterListAdapter extends ArrayAdapter<Status> {

	public TwitterListAdapter(Activity activity, List<Status> imageAndTexts) {
		super(activity, 0, imageAndTexts);
		Log.i(this.getClass().getName(), "got list: "
				+ (imageAndTexts == null ? "null!" : "not null, size = "
						+ imageAndTexts.size()));
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		Activity activity = (Activity) getContext();
		LayoutInflater inflater = activity.getLayoutInflater();
		// Inflate the views from XML
		View rowView = inflater.inflate(R.layout.image_text_layout, null);
		Status status = getItem(position);
		TextView textView = (TextView) rowView.findViewById(R.id.job_text);
		textView.setBackgroundResource(R.drawable.speech_bubble_right);

		String message = status.getText();
		String auth = status.getUser().getName();
		String date = Utility.getDateDifference(status.getCreatedAt());
		if (date.length() > 0) {
			String latest = message + "<br><br><i>" + auth + " - " + date
					+ "</i>";
			textView.setText(Html.fromHtml(latest));
		} else {
			textView.setText(Html.fromHtml(message) + "\n"
					+ Html.fromHtml(auth));
		}
		return rowView;

	}

}
