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
package com.luxoft.ivilink.sdk.android.nonnative;

import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Modifier;
import java.util.Enumeration;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

import android.util.Log;

import com.luxoft.ivilink.sdk.android.lib.utils.log.Logging;

import dalvik.system.DexClassLoader;

/**
 * Class used for profile creation. Used from the native code
 * (PMAL/PIM/profileFactory)
 */
public class ProfileCreator {
    final static String tag = ProfileCreator.class.getName();

    /**
     * Creates profile instance. Is invoked from PMAL's profileFactory.
     * 
     * @param libPath
     *            Path to .jar with profile implementation
     * @param profileIUID
     * @param serviceUID
     * @param nativeCallbacksPointer
     *            Serialized pointer to NonnativeCallbacksWrapper (where java
     *            callbacks can be obtained)
     * @return null, in case of error, serialized pointer to native profile
     *         instance otherwise
     */
    @SuppressWarnings({ "rawtypes", "unchecked", "unused" })
    private static long createProfile(String libPath, String profileIUID, String serviceUID,
            long nativeCallbacksPointer) {
        Log.v(tag, Logging.getCurrentMethodName(libPath, profileIUID, serviceUID,
                                                nativeCallbacksPointer));
        try {
            ClassLoader loader = new DexClassLoader("file://" + libPath,
                    IviLinkApplication.internalPath, null, ProfileCreator.class.getClassLoader());
            JarFile jarFile = new JarFile(libPath);
            Enumeration<JarEntry> allEntries = jarFile.entries();
            while (allEntries.hasMoreElements()) {
                JarEntry entry = (JarEntry) allEntries.nextElement();
                String name = entry.getName().replaceAll("/", "\\.");
                if (name.endsWith("class")) {
                    try {
                        Class claz = loader.loadClass(name.replace(".class", ""));
                        Log.i(tag, "Loaded class: " + claz.getName());
                        if (AbstractProfile.class.isAssignableFrom(claz)) {
                            Log.i(tag, claz.getName() + " is " + AbstractProfile.class.getName());
                            if (!Modifier.isAbstract(claz.getModifiers())) {
                                Log.i(tag, claz.getName()
                                        + " is not abstract, trying to create an instance!");
                                try {
                                    AbstractProfile profileInstance = (AbstractProfile) (claz
                                            .getConstructor(String.class, String.class,
                                                            long.class)
                                            .newInstance(profileIUID, serviceUID,
                                                         nativeCallbacksPointer));

                                    Log.i(tag, claz.getName() + " created!");
                                    return profileInstance.getNativeProfileHolder();
                                } catch (IllegalArgumentException e) {
                                    e.printStackTrace();
                                } catch (InstantiationException e) {
                                    e.printStackTrace();
                                } catch (IllegalAccessException e) {
                                    e.printStackTrace();
                                } catch (InvocationTargetException e) {
                                    e.printStackTrace();
                                } catch (NoSuchMethodException e) {
                                    e.printStackTrace();
                                }
                            } else {
                                Log.e(tag, claz.getName() + " is abstract, skipping!");
                            }
                        } else {
                            Log.i(tag, claz.getName() + "is not " + AbstractProfile.class.getName());
                        }
                    } catch (ClassNotFoundException e) {
                        e.printStackTrace();
                    }
                }
            }
        } catch (IOException e1) {
            e1.printStackTrace();
        }
        return 0;
    }
}
