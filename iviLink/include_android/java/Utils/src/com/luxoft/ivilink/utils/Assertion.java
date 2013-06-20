package com.luxoft.ivilink.utils;

public final class Assertion {
	public static void check(boolean expression) {
		if (!expression) {
			throw new RuntimeException();
		}
	}

	public static void check(boolean expression, String message) {
		if (!expression) {
			throw new RuntimeException(message);
		}
	}
}
