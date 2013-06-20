package com.luxoft.ivilink.sdk.DeviceManager;

import java.util.ArrayList;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.os.Bundle;
import android.os.Environment;
import android.os.FileObserver;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Button;
import android.widget.ListView;
import android.widget.RelativeLayout;

import com.luxoft.ivilink.sdk.DeviceManager.widgets.SelectedArrayAdapter;

public abstract class TrustListActivity extends Activity implements OnItemClickListener {
    final static String TAG = TrustListActivity.class.getName();

    static {
        System.loadLibrary("devicemanager");
    }

    private Button mRemoveAllButton, mDisconnectButton;
    private ListView mListView;
    private FileObserver mTrustDbObserver;
    private Handler mHandler;
    private Runnable mTimerTask;
    private Runnable mUpdateDbTask;

    private SelectedArrayAdapter<String> adapter;
    private int selectedItem = -1;

    public native void start(String trustListPath);

    public native void removeDeviceFromList(int index);

    public native void disconnectDevice(int index);

    public native boolean isConnected();

    public native void trustDbReload();

    public native void trustListClear();

    protected abstract void setContentView();

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView();

        mListView = (ListView) findViewById(R.id.listView);
        mListView.setOnItemClickListener(this);

        mRemoveAllButton = (Button) findViewById(R.id.rmAll);
        mRemoveAllButton.setOnClickListener(mButtonClickListener);
        mDisconnectButton = (Button) findViewById(R.id.disconnect);
        mDisconnectButton.setOnClickListener(mButtonClickListener);

        mHandler = new Handler();
        mTimerTask = new Runnable() {
            @Override
            public void run() {
                mDisconnectButton.setEnabled(isConnected());
                mHandler.postDelayed(this, 500);
            }
        };
        mUpdateDbTask = new Runnable() {
            @Override
            public void run() {
                adapter.clearSelection();
                adapter.clear();
                trustDbReload();
            }
        };
        mHandler.postDelayed(mTimerTask, 500);

        ArrayList<String> values = new ArrayList<String>();

        adapter = new SelectedArrayAdapter(this, android.R.layout.simple_list_item_1, values);
        adapter.setNotifyOnChange(true);

        mListView.setAdapter(adapter);

        start(Environment.getExternalStorageDirectory().getAbsolutePath());
        mTrustDbObserver = new FileObserver(Environment.getExternalStorageDirectory().getAbsolutePath(),
                FileObserver.CLOSE_WRITE | FileObserver.CREATE | FileObserver.DELETE | FileObserver.MODIFY
                        | FileObserver.MOVED_FROM | FileObserver.MOVED_TO) {
            @Override
            public void onEvent(int event, String path) {
                Log.i(TAG, "onEvent( " + event + "," + path + ")");
                if (path.equals("trust.db")) {
                    mHandler.post(mUpdateDbTask);
                }
            }
        };
        mTrustDbObserver.startWatching();
    }

    private View.OnClickListener mButtonClickListener = new View.OnClickListener() {
        public void onClick(View view) {
            if (R.id.rmAll == view.getId()) {
                // Remove all devices
                onRemoveAll();
            } else if (R.id.disconnect == view.getId()) {
                // Disconnect button
                onDisconnect();
            }
        }
    };

    private void onRemoveAll() {
        AlertDialog.Builder builder = new AlertDialog.Builder(this);
        builder.setTitle("Clearing Trust-list").setMessage("Are you sure?").setIcon(android.R.drawable.ic_dialog_alert)
                .setPositiveButton("Yes", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        clearTrustList();

                    }
                }).setNegativeButton("No", null).show();
    }

    public void onRemoveDeviceHandler(View v) {
        int index = mListView.indexOfChild((RelativeLayout) (v.getParent()).getParent());
        Log.v(TAG, "OnRemoveDeviceHandler: " + index);
        if (mListView.getChildCount() == 1) {
            mRemoveAllButton.setEnabled(false);
        }
        deleteDevice(index);
    }

    private void onDisconnect() {
        disconnectDevice(0);
    }

    private void clearTrustList() {
        adapter.clear();
        mRemoveAllButton.setEnabled(false);
        trustListClear();
    }

    private void deleteDevice(int index) {
        Log.v(TAG, "delete device: " + index + " out of " + adapter.getCount());
        removeDeviceFromList(index);
        adapter.remove(adapter.getItem(index));
        selectedItem = -1;
        adapter.clearSelection();
    }

    public void updateListContent(final String item) {
        Log.i(TAG, "updateListContent(" + item + ")");

        adapter.add(item);
        mRemoveAllButton.setEnabled(true);
    }

    @Override
    public void onItemClick(AdapterView<?> arg0, View arg1, int position, long arg3) {
        Log.i(TAG, "onListItemClick(position = " + position + ")");
        if (selectedItem == position) {
            adapter.clearSelection();
            selectedItem = -1;
        } else {
            adapter.setSelectedPosition(position);
            selectedItem = position;
        }
    }
}
