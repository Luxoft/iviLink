package com.luxoft.ivilink.sdk.android.nonnative;

/**
 * Stores information about an iviLink's channel.
 */
public final class DataChannel {
    /**
     * Priority of the DataChannel
     */
    public enum Priority {
        // higher priority
        REAL_TIME,
        // lower priority
        PLAIN_DATA;
        /**
         * Should be kept up-to-date with c++ code. (see utils/misc/Types.hpp)
         * 
         * @return values corresponding to c++ enum TChannelPriority
         */
        int getIntegerValue() {
            switch (this) {
            case REAL_TIME:
                return 1;
            case PLAIN_DATA:
                return 2;
            }
            return -1;
        }
    }

    private int mID;
    private final String mTag;
    private boolean mIsValid;
    private final Priority mPriority;

    /**
     * Creates a DataChannel instance with the tag and priority specified
     * 
     * @param tag
     * @param channelPrio
     */
    public DataChannel(String tag, Priority channelPrio) {
        mID = 0;
        mTag = tag;
        mPriority = channelPrio;
    }

    /**
     * Creates a DataChannel with the tag specified and priority PLAIN_DATA
     * 
     * @param tag
     */
    public DataChannel(String tag) {
        this(tag, Priority.PLAIN_DATA);
    }

    int getID() {
        return mID;
    }

    /**
     * @return tag associated with this DataChannel
     */
    public String getTag() {
        return mTag;
    }

    void setID(int ID) {
        mID = ID;
    }

    void setValid(boolean valid) {
        mIsValid = valid;
    }

    void invalidate() {
        setID(0);
        setValid(false);
    }

    /**
     * @return true if this DataChannel can be used in sending data
     */
    public boolean isValid() {
        return mIsValid;
    }

    public Priority getPriority() {
        return mPriority;
    }

    @Override
    public String toString() {
        return this.getClass().getSimpleName() + " " + mTag + ":" + mID + ": is valid? " + mIsValid;
    }
}
