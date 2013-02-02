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
package com.luxoft.ivilink.samples.media.customwidgets;

import java.util.List;

import android.content.Context;
import android.graphics.Color;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.TextView;

import com.luxoft.ivilink.samples.media.R;
import com.luxoft.ivilink.samples.media.structs.MediaType;
import com.luxoft.ivilink.samples.media.structs.PlayingState;
import com.luxoft.ivilink.samples.media.structs.TrackInformation;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class SelectedArrayAdapter extends ArrayAdapter<TrackInformation> {
	private int selectedPos = -1;

	class StateEncapsulation {
		private PlayingState mState = PlayingState.STOPPED;
		private final String TAG = StateEncapsulation.class.getName();

		PlayingState getState() {
			return mState;
		}

		void setState(PlayingState state) {
			Log.i(TAG, this.mState.toString() + " -> " + state.toString());
			this.mState = state;
		}
	}

	StateEncapsulation stateEnc = new StateEncapsulation();

	public SelectedArrayAdapter(Context context, int textViewResourceId) {
		super(context, textViewResourceId);
	}

	public SelectedArrayAdapter(Context context, int resource, int textViewResourceId) {
		super(context, resource, textViewResourceId);
	}

	public SelectedArrayAdapter(Context context, int textViewResourceId, TrackInformation[] objects) {
		super(context, textViewResourceId, objects);
	}

	public SelectedArrayAdapter(Context context, int resource, int textViewResourceId,
			TrackInformation[] objects) {
		super(context, resource, textViewResourceId, objects);
	}

	public SelectedArrayAdapter(Context context, int textViewResourceId,
			List<TrackInformation> objects) {
		super(context, textViewResourceId, objects);
	}

	public SelectedArrayAdapter(Context context, int resource, int textViewResourceId,
			List<TrackInformation> objects) {
		super(context, resource, textViewResourceId, objects);
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View v = convertView;
		if (v == null) {
			LayoutInflater vi = (LayoutInflater) this.getContext().getSystemService(
					Context.LAYOUT_INFLATER_SERVICE);
			v = vi.inflate(R.layout.selected_row, null);
		}

		TextView rowCaption = (TextView) v.findViewById(R.id.selectedListRow);
		ImageView rowIconPlayingState = (ImageView) v.findViewById(R.id.listIcon);
		ImageView rowIconLocationState = (ImageView) v.findViewById(R.id.remoteIcon);
		ImageView rowIconType = (ImageView) v.findViewById(R.id.typeIcon);

		if (selectedPos == position) {
			switch (stateEnc.getState()) {
			case PLAYING:
				rowIconPlayingState.setImageResource(R.drawable.play_selected_icon);
				rowCaption.setTextColor(Color.rgb(66, 140, 194));
				break;
			case PAUSED:
				rowIconPlayingState.setImageResource(R.drawable.pause_selected_icon);
				rowCaption.setTextColor(Color.rgb(66, 140, 194));
				break;
			case STOPPED:
				rowIconPlayingState.setImageResource(R.drawable.innactive_selected_icon);
				rowCaption.setTextColor(Color.BLACK);
				break;
			}

			rowIconPlayingState.setBackgroundColor(Color.rgb(192, 192, 192));
			rowIconLocationState.setBackgroundColor(Color.rgb(192, 192, 192));
			rowIconType.setBackgroundColor(Color.rgb(192, 192, 192));
			rowCaption.setBackgroundColor(Color.rgb(192, 192, 192));
		} else {
			rowIconPlayingState.setImageResource(R.drawable.innactive_icon);
			rowCaption.setTextColor(Color.WHITE);
			rowCaption.setBackgroundColor(Color.TRANSPARENT);
			rowIconPlayingState.setBackgroundColor(Color.TRANSPARENT);
			rowIconLocationState.setBackgroundColor(Color.TRANSPARENT);
			rowIconType.setBackgroundColor(Color.TRANSPARENT);
		}

		if (getItem(position).isLocal) {
			rowIconLocationState.setImageResource(R.drawable.innactive_icon);
		} else {
			rowIconLocationState.setImageResource(R.drawable.remote_icon);
		}
		if (getItem(position).mediaType == MediaType.AUDIO) {
			rowIconType.setImageResource(R.drawable.note);
		} else {
			rowIconType.setImageResource(R.drawable.video);
		}

		if (rowCaption != null) {
			rowCaption.setText(this.getItem(position).trackName);
		}

		return v;
	}

	public void setSelectedPosition(int pos) {
		Log.v(this.getClass().getSimpleName(), Logging.getCurrentMethodName(pos));
		selectedPos = pos;
		notifyDataSetChanged();
	}

	public int getSelectedPosition() {
		return selectedPos;
	}

	public void showPausedIcon() {
		Log.v(this.getClass().getSimpleName(), Logging.getCurrentMethodName());
		stateEnc.setState(PlayingState.PAUSED);
		notifyDataSetChanged();
	}

	public void showPlayingIcon() {
		Log.v(this.getClass().getSimpleName(), Logging.getCurrentMethodName());
		stateEnc.setState(PlayingState.PLAYING);
		notifyDataSetChanged();
	}

	public void hideIcon() {
		Log.v(this.getClass().getSimpleName(), Logging.getCurrentMethodName());
		stateEnc.setState(PlayingState.STOPPED);
		notifyDataSetChanged();
	}

	public void resetAllMarkers() {
		Log.v(this.getClass().getSimpleName(), Logging.getCurrentMethodName());
		selectedPos = -1;
		stateEnc.setState(PlayingState.STOPPED);
		notifyDataSetChanged();
	}
}
