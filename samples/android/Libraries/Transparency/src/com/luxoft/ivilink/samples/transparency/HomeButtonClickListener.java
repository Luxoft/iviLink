package com.luxoft.ivilink.samples.transparency;

import android.view.View;
import android.view.View.OnClickListener;

class HomeButtonClickListener implements OnClickListener {

	@Override
	public void onClick(View v) {
		android.os.Process.killProcess(android.os.Process.myPid());
	}

}
