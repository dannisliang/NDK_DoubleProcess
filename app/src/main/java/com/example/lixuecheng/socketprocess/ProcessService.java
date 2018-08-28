package com.example.lixuecheng.socketprocess;

import android.app.Service;
import android.content.Intent;
import android.os.IBinder;
import android.os.Process;
import android.support.annotation.Nullable;
import android.util.Log;

import java.util.Timer;
import java.util.TimerTask;

public class ProcessService extends Service {

    int i = 0;

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        WatchDog watchDog = new WatchDog();
        watchDog.creatSocket(String.valueOf(Process.myUid()));
        watchDog.connectMonitor();
        Timer timer = new Timer();
        timer.scheduleAtFixedRate(
                new TimerTask() {
                    @Override
                    public void run() {
                        Log.e("lxc", "服务开启中" + i);
                        i++;
                    }
                }
                , 0, 1000 * 3);
    }
}
