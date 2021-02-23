package com.example.tools;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.wifi.WifiManager;


public class NetworkChangedReceiver extends BroadcastReceiver {
    final static String TAG = "PlatformUtils";
    @Override
    public void onReceive(Context context, Intent intent) {
        if (WifiManager.RSSI_CHANGED_ACTION.equals(intent.getAction())){
            int level = PlatformUtils.getWifiSignalLevel();
            if (level!=PlatformUtils.mWifiStrengthLevel) {
                PlatformUtils.mWifiStrengthLevel = level;
                if (PlatformUtils.mNetworkType == "WIFI") {
                    PlatformUtils.onNetworkChanged();
                }
            }
        }else{
            PlatformUtils.updateNetworkInfo();
            PlatformUtils.onNetworkChanged();
        }
    }
}