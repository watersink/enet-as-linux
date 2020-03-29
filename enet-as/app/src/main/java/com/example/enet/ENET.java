package com.example.enet;

import android.graphics.Bitmap;

public class ENET {


    public native boolean Init(byte[] param, byte[] bin);
    public native float[] Process(Bitmap bitmap);
    // Used to load the 'native-lib' library on application startup.

    static {
        System.loadLibrary("ENET");
    }
}
