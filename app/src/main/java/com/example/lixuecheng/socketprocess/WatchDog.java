package com.example.lixuecheng.socketprocess;

public class WatchDog {

    static {
        System.loadLibrary("native-lib");
    }
    public native void creatSocket(String uid);
    public native void connectMonitor();
}
