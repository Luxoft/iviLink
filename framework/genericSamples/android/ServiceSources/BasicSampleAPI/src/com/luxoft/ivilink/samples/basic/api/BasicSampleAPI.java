package com.luxoft.ivilink.samples.basic.api;

public class BasicSampleAPI {
    public static final String API_NAME = "BasicSampleProfile_PAPI_UID";
    public static final String SERVICE_NAME = "BasicSampleService";

    public interface ProfileAPI {
        public void sendOperands(byte a, byte b);

        public void sendResult(byte res);
    }

    public interface ApplicationAPI {
        public void receiveOperands(byte a, byte b);

        public void receiveResult(byte res);
    }
}
