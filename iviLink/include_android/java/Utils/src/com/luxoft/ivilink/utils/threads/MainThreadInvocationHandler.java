package com.luxoft.ivilink.utils.threads;

import java.lang.reflect.InvocationHandler;
import java.lang.reflect.Method;

import android.os.Handler;
import android.os.Looper;

/**
 * Invokes methods on main thread.
 */
public class MainThreadInvocationHandler implements InvocationHandler {
	private Object mTarget;
	private Handler mHandler = new Handler(Looper.getMainLooper());

	public MainThreadInvocationHandler(Object target) {
		mTarget = target;
	}

	@Override
	public Object invoke(Object proxy, final Method method, final Object[] args)
			throws Throwable {
		if (!Looper.getMainLooper().equals(Looper.myLooper())) {
			mHandler.post(new Runnable() {
				public void run() {
					callMethod(method, args);
				}
			});
		} else {
			callMethod(method, args);
		}
		return null;
	}

	private void callMethod(Method method, Object[] args) {
		try {
			method.invoke(mTarget, args);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
