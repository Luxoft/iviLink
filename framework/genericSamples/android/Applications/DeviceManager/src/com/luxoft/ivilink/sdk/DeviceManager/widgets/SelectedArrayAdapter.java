package com.luxoft.ivilink.sdk.DeviceManager.widgets;

import android.content.Context;
import android.graphics.Color;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.TextView;
import com.luxoft.ivilink.sdk.DeviceManager.R;

import java.util.List;

public class SelectedArrayAdapter<T> extends ArrayAdapter<T> {
    // used to keep selected position in ListView
    private int mSelectedPos = -1; // init value for not-selected

    public SelectedArrayAdapter(Context context, int textViewResourceId) {
        super(context, textViewResourceId);
    }

    public SelectedArrayAdapter(Context context, int resource, int textViewResourceId) {
        super(context, resource, textViewResourceId);
    }

    public SelectedArrayAdapter(Context context, int textViewResourceId, T[] objects) {
        super(context, textViewResourceId, objects);
    }

    public SelectedArrayAdapter(Context context, int resource, int textViewResourceId, T[] objects) {
        super(context, resource, textViewResourceId, objects);
    }

    public SelectedArrayAdapter(Context context, int textViewResourceId, List<T> objects) {
        super(context, textViewResourceId, objects);
    }

    public SelectedArrayAdapter(Context context, int resource, int textViewResourceId, List<T> objects) {
        super(context, resource, textViewResourceId, objects);
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        Log.v(this.getClass().getName(), "SelectedArrayAdapter.getView: " + position + " selected: " + mSelectedPos);

        View v = convertView;

        if (v == null) {
            LayoutInflater vi = (LayoutInflater) this.getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            v = vi.inflate(R.layout.selected_row, null);
        }

        TextView rowCaption = (TextView) v.findViewById(R.id.selectedListRow);

        if (rowCaption != null) {
            if (mSelectedPos == position) {
                v.setBackgroundColor(Color.rgb(102, 178, 255));
                ((Button) v.findViewById(R.id.rmSelected)).setVisibility(View.VISIBLE);
            } else {
                v.setBackgroundColor(Color.TRANSPARENT);
                ((Button) v.findViewById(R.id.rmSelected)).setVisibility(View.GONE);
            }

            rowCaption.setText(this.getItem(position).toString());
        }

        return v;
    }

    public void setSelectedPosition(int pos) {
        Log.v(this.getClass().getName(), "setSelectedPosition pos = " + pos);
        mSelectedPos = pos;
        // inform the view of this change
        notifyDataSetChanged();
    }

    public void clearSelection() {
        mSelectedPos = -1;
    }

    public int getSelectedPosition() {
        return mSelectedPos;
    }
}
