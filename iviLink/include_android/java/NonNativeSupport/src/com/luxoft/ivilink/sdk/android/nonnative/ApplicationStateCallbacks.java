package com.luxoft.ivilink.sdk.android.nonnative;

public interface ApplicationStateCallbacks {
    /**
     * Invoked after the application has been registered in Application Manager.
     * 
     * @param isStartedByUser
     *            true if the application has been started by user; false
     *            otherwise
     */
    public void onInitDone(boolean isStartedByUser);

    /**
     * Called before loading an incoming service.
     * 
     * @param serviceUID
     *            UID of an incoming service
     */
    public void onIncomingServiceBeforeLoading(String serviceUID);

    /**
     * Called after loading an incoming service.
     * 
     * @param serviceUID
     *            UID of a loaded incoming service
     */
    public void onIncomingServiceAfterLoading(String serviceUID);

    /**
     * Called if an error has occurred during service loading.
     * 
     * @param service
     *            UID of an incoming service that caused the error
     */
    public void onServiceLoadError(String serviceUID);

    /**
     * Called when a service session ends.
     * 
     * @param service
     *            UID of a service whose session has ended
     */
    public void onServiceDropped(String serviceUID);

    /**
     * Invoked when connection to the other side has been broken.
     */
    public void onPhysicalLinkDown();

    /**
     * Invoked when connection to the other side has been established.
     */
    public void onLinkEstablished();
}
