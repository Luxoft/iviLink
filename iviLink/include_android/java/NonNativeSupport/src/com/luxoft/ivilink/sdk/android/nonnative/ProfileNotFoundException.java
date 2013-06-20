package com.luxoft.ivilink.sdk.android.nonnative;

/**
 * Exception thrown by GenericProfileGetter when the profile instance couldn't
 * be obtained
 * 
 */
public class ProfileNotFoundException extends Exception {

	/**
	 * 
	 */
	private static final long serialVersionUID = -204846712718915187L;

	public ProfileNotFoundException(String message) {
		super(message);
	}
}
