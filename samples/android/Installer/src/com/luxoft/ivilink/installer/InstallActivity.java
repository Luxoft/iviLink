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
package com.luxoft.ivilink.installer;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

import android.app.Activity;
import android.app.ProgressDialog;
import android.content.res.Configuration;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.Gravity;
import android.widget.Toast;

import com.luxoft.ivilink.sdk.android.lib.utils.ForApp;
import com.luxoft.ivilink.sdk.android.lib.utils.Paths;
import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

public class InstallActivity extends Activity {

	private final String tag = InstallActivity.class.getName();

	private final static String EXTERNAL_PATH = Environment.getExternalStorageDirectory()
			.getAbsolutePath();
	private final static String ZIP_PATH = EXTERNAL_PATH + "/xmls.zip";

	private final static int BUFFER_SIZE = 1024;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		Log.v(tag, Logging.getCurrentMethodName(savedInstanceState));
		new InstallTask().execute(ZIP_PATH);
	}

	private boolean deleteDirectory(File path) {
		if (path.exists()) {
			BigLog.i(tag, "path " + path + " exists", '!');
			File[] files = path.listFiles();
			for (int i = 0; i < files.length; i++) {
				if (files[i].isDirectory()) {
					deleteDirectory(files[i]);
				} else {
					files[i].delete();
				}
			}
		} else {
			BigLog.i(tag, "path " + path + " does not exist", '!');
			return true;
		}
		return (path.delete());
	}

	class InstallTask extends AsyncTask<String, Integer, Boolean> {

		private ProgressDialog progress;

		@Override
		public void onPreExecute() {
			Log.v(tag, Logging.getCurrentMethodName());
			progress = new ProgressDialog(InstallActivity.this);
			progress.setTitle("");
			progress.setMessage("Installing data...");
			progress.setCanceledOnTouchOutside(false);
			progress.setCancelable(false);
			progress.show();
		}

		private void clearDirs() {
			String[] dirsToClear = { ForApp.getServicePath(), Paths.getAppDatabase(),
					Paths.getProfileRepository(), EXTERNAL_PATH + "/iviLinkMedia" };
			for (String path : dirsToClear) {
				File f = new File(path);
				if (f.exists()) {
					deleteDirectory(f);
				}
			}
		}

		private void makeDirs(String path) {
			Log.v(tag, Logging.getCurrentMethodName(path));
			String[] pathComponents = path.split(File.separator);
			for (String str : pathComponents) {
				Log.v(tag, "str= " + str);
			}
			StringBuffer sb = new StringBuffer("/");
			for (int i = 1; i < pathComponents.length; i++) {
				if (pathComponents[i].contains(".")) {
					Log.v(tag, pathComponents[i] + " is a filename, done!");
					break;
				} else {
					sb.append(pathComponents[i]).append("/");
					BigLog.i(tag, sb.toString(), Character.forDigit(i, 10));
				}

			}
			new File(sb.toString()).mkdirs();
		}

		@Override
		protected Boolean doInBackground(String... params) {
			BigLog.i(tag, Logging.getCurrentMethodName((Object[]) params), '!');
			if (!Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
				Log.e(tag, "external storage is inaccessible");
				return false;
			}
			byte[] buffer = new byte[BUFFER_SIZE];
			int readSize;
			// extracting zip archive from res/raw to sdcard
			InputStream ins = getResources().openRawResource(R.raw.xmls);
			try {
				FileOutputStream fos = new FileOutputStream(ZIP_PATH);
				while ((readSize = ins.read(buffer, 0, BUFFER_SIZE)) > -1) {
					fos.write(buffer, 0, readSize);
				}
				ins.close();
				fos.close();
			} catch (IOException e) {
				e.printStackTrace();
				return false;
			}
			// deleting known directories if they exist
			clearDirs();
			// exctracting files from zip
			try {
				BigLog.i(tag, "pathToZip = " + params[0], '!');
				ZipInputStream zipinputstream = new ZipInputStream(new FileInputStream(params[0]));
				ZipEntry zipentry;
				zipentry = zipinputstream.getNextEntry();
				while (zipentry != null) {
					String entryName = zipentry.getName();
					entryName = entryName.replace('/', File.separatorChar);
					entryName = entryName.replace('\\', File.separatorChar);
					BigLog.i(tag, "extracting file: " + entryName, '!');
					FileOutputStream fileoutputstream;
					File newFile = new File(EXTERNAL_PATH + "/" + entryName);
					if (zipentry.isDirectory()) {
						BigLog.i(tag, "new file: " + newFile.getAbsolutePath()
								+ " is a directory, skipping", '!');
						zipentry = zipinputstream.getNextEntry();
						continue;
					}
					makeDirs(newFile.getAbsolutePath());
					fileoutputstream = new FileOutputStream(newFile);
					while ((readSize = zipinputstream.read(buffer, 0, BUFFER_SIZE)) > -1) {
						fileoutputstream.write(buffer, 0, readSize);
					}
					fileoutputstream.close();
					zipinputstream.closeEntry();
					zipentry = zipinputstream.getNextEntry();
				}// while
				zipinputstream.close();
			} catch (Exception e) {
				e.printStackTrace();
				return false;
			}
			return true;
		}

		@Override
		public void onPostExecute(Boolean result) {
			progress.dismiss();
			new File(ZIP_PATH).delete();
			Toast toast = Toast.makeText(InstallActivity.this, (result ? "Install successful!"
					: "Sorry! Something went wrong during installation"), Toast.LENGTH_LONG);
			toast.setGravity(Gravity.CENTER_HORIZONTAL | Gravity.CENTER_VERTICAL, 0, 0);
			toast.show();
			InstallActivity.this.finish();
		}
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		Log.v(tag, Logging.getCurrentMethodName(newConfig));
	}

}
