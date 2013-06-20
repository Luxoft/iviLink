package com.luxoft.ivilink.sdk.android.nonnative;

import android.util.Log;

import com.luxoft.ivilink.utils.Assertion;
import com.luxoft.ivilink.utils.log.Logging;

/**
 * Creates profile proxies. A profile proxy is the only way for the application
 * to communicate with the profile instance. Profile proxy implements a profile
 * API. When a method of the proxy is invoked, the proxy locks and obtains the
 * profile instance from the Service Manager, passes the method call to it, and
 * after that unlocks (releases) the profile instance. In case profile instance
 * could not be obtained nothing happens and null (or some other default value
 * depending on the method's signature) is returned. Note: any exceptions that
 * may occur during profile API method call are handled on the level of the
 * proxy and are not passed to the application.
 */
public class ProfileProxyFactory {
    private static final String TAG = ProfileProxyFactory.class.getName();

    /**
     * Creates an instance of the profile proxy (its functionality is to wrap
     * all profile API calls in locking/unlocking the profile instance with the
     * Service Manager) for the specified profile API. The returned object may
     * <b>only</b> be used for calling the methods defined in the
     * profileApiInterface (Note: all calls to standard <code>Object</code>'s
     * methods will return <code>null</code> or other default values without
     * doing anything)
     * 
     * @param serviceUid
     *            UID of the service on behalf of which the profile API is used
     * @param profileApiUID
     *            UID of the profile API
     * @param profileApiInterface
     *            <code>class object</code> of the interface representing the
     *            profile API (Note: if the provided object is not an interface,
     *            a <code>RuntimeException</code> will be thrown)
     * @return proxy implementation of the profileApiInterface
     * @throws RuntimeException
     *             in case the provided class object is not an interface
     */
    @SuppressWarnings("unchecked")
    public static <ProfileAPI> ProfileAPI createProfileProxy(String serviceUid,
            String profileApiUID, Class<ProfileAPI> profileApiInterface) {
        Log.v(TAG, Logging.getCurrentMethodName(serviceUid, profileApiUID, profileApiInterface));
        Assertion.check(profileApiInterface.isInterface());
        ProfileApiInvocationHandler invocationHandler = new ProfileApiInvocationHandler(
                profileApiUID, serviceUid, profileApiInterface.getMethods());
        return (ProfileAPI) java.lang.reflect.Proxy.newProxyInstance(profileApiInterface
                .getClassLoader(), new Class[] { profileApiInterface }, invocationHandler);
    }
}
