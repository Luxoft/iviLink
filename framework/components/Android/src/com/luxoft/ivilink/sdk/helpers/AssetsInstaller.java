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
package com.luxoft.ivilink.sdk.helpers;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.LinkedList;
import java.util.List;

import android.content.Context;
import android.content.res.AssetManager;
import android.content.res.Resources;
import android.os.Environment;
import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.BigLog;
import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

/**
 * Installs files from the "assets" directory to the external storage device.
 * Files to install are listed in predefined files that are created during
 * compilation (it is much much faster than listing assets during runtime)
 */
public class AssetsInstaller {
    private static final String TAG = AssetsInstaller.class.getName();
    private static final int BUF_SIZE = 1024;
    private static final String STORAGE_PATH = Environment.getExternalStorageDirectory()
            .getAbsolutePath();

    private List<String> mAssetsFiles = new LinkedList<String>();
    private List<String> mAssetsFolders = new LinkedList<String>();
    private byte[] mCopyBuffer = new byte[BUF_SIZE];

    /**
     * Reads assets directory, and copies files listed in the assetsfiles file
     * (created during compilation) from it to the external storage.
     * 
     * @param context
     */
    public synchronized void moveAssetsToExternalStorage(Context context) {
        Log.v(TAG, Logging.getCurrentMethodName(context));
        mAssetsFiles.clear();
        mAssetsFolders.clear();
        Log.v(TAG, Logging.getCurrentMethodName());
        Resources res = context.getResources();
        AssetManager am = res.getAssets();
        fillLists(am);
        BigLog.i(TAG, "Assets folders: ", '~');
        for (String str : mAssetsFolders) {
            Log.i(TAG, str);
            mkdirsOnExternalStorage(str);
        }
        BigLog.i(TAG, "Copying assets files: ", '~');
        for (String str : mAssetsFiles) {
            Log.v(TAG, str);
            copyFileToExternalStorage(am, str);
        }
    }

    /**
     * Creates the folder described by the "path" parameter (the folder may have
     * several parents: e.g. f/ol/d/er) on the external storage.
     * 
     * @param path
     *            folder name
     * @return <code>true</code> in case the folder structure has been
     *         successfully created
     */
    private boolean mkdirsOnExternalStorage(String path) {
        Log.v(TAG, Logging.getCurrentMethodName(path));
        File file = new File(STORAGE_PATH + File.separator + path);
        return (file.mkdirs() || file.isDirectory());
    }

    /**
     * Creates a file on the external storage with the path
     * STORAGE_PATH/assetFilePath and contents from the assets/assetFilePath
     * file. Does nothing if a file already exists in the path.
     * 
     * @param am
     * @param assetFilePath
     */
    private void copyFileToExternalStorage(AssetManager am, String assetFilePath) {
        Log.v(TAG, Logging.getCurrentMethodName(am, assetFilePath));
        File newFile = new File(STORAGE_PATH + File.separator + assetFilePath);
        if (newFile.exists()) {
            return;
        }
        try {
            InputStream fin = am.open(assetFilePath);
            FileOutputStream fout = new FileOutputStream(newFile);
            int readSize = 0;
            while ((readSize = fin.read(mCopyBuffer, 0, BUF_SIZE)) > -1) {
                fout.write(mCopyBuffer, 0, readSize);
            }
            fout.close();
            fin.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private void fillLists(AssetManager am) {
        Log.v(TAG, Logging.getCurrentMethodName(am));
        readAsset(am, "assetsfiles", mAssetsFiles);
        readAsset(am, "assetsdirs", mAssetsFolders);
    }

    /**
     * Parses a file from the assets folder into a List of Strings.
     * 
     * @param context
     * @param assetName
     *            file to read
     * @param target
     *            list to fill
     */
    private void readAsset(AssetManager am, String assetName, List<String> target) {
        Log.v(TAG, Logging.getCurrentMethodName(am, assetName, target));
        String line;
        BufferedReader reader;
        try {
            reader = new BufferedReader(new InputStreamReader(am.open(assetName)));
            while ((line = reader.readLine()) != null) {
                target.add(line);
            }
            reader.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
