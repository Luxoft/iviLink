package com.luxoft.ivilink.samples.basic;

import java.util.Arrays;
import java.util.List;

import com.luxoft.ivilink.applicationmanager.SingleEntryAppManRequestsReceiver;

public class AppmanReceiver extends SingleEntryAppManRequestsReceiver {

    @Override
    protected List<String> getSupportedServices() {
        return Arrays.asList(new String[] { "BasicSampleService" });
    }

}
