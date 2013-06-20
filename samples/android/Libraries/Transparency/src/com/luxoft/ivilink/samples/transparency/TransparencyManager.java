package com.luxoft.ivilink.samples.transparency;

import android.app.Activity;
import android.view.ViewStub;
import android.widget.ImageButton;

public class TransparencyManager {
	/**
	 * Set content view as layoutResID with transparent areas on top and bottom
	 * of the view. See base_layout for sizes of the areas.
	 * 
	 * @param activity
	 *            should have its theme set to
	 *            "@android:style/Theme.Translucent.NoTitleBar.Fullscreen" in
	 *            the AndroidManifest.xml
	 * @param layoutResID
	 */
	public static void setContentView(Activity activity, int layoutResID) {
		activity.setContentView(R.layout.base_layout);
		((ImageButton) activity.findViewById(R.id.top_bar_home_button))
				.setOnClickListener(new HomeButtonClickListener());
		ViewStub baseView = (ViewStub) activity
				.findViewById(R.id.baseViewHolder);
		baseView.setLayoutResource(layoutResID);
		baseView.inflate();
	}

}
