package com.example.tools;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.provider.Settings;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.OrientationEventListener;
import android.widget.Toast;

import androidx.core.app.ActivityCompat;

import java.util.UUID;

public class PlatformUtils {
    final static String TAG = "PlatformUtils";

    private static Activity mActivity;

    private static int orientType = 0;                          // 0-横屏,1-反向横屏
    private static OrientationEventListener orientListener;     // 方向监听

    public static String mNetworkType = "NONE";
    public static int mWifiStrengthLevel = 4;                   // wifi信号强度0-4
    public static int mSignalStrengthLevel = 4;                 // 移动网络信号强度0-4
    private static NetworkChangedReceiver mNetworkChangedReceiver = null;

    private static boolean bIsRequestingPermission = false;
    private static final int REQUEST_CODE_WRITE_EXTERNAL = 101; // WRITE_EXTERNAL权限请求码
    private static final int REQUEST_CODE_READ_PHONE = 102;     // READ_PHONE_STATE权限请求码


    // 初始化
    public static void onCreate(Activity activity) {
        mActivity = activity;
        // 初始化网络监听
        mNetworkChangedReceiver = new NetworkChangedReceiver();

        // 屏幕旋转方向
        orientListener = new OrientationEventListener(mActivity) {
            @Override
            public void onOrientationChanged(int orientation) {
                if (orientation == OrientationEventListener.ORIENTATION_UNKNOWN) {
                    return;  //手机平放时，检测不到有效的角度
                }
                //只检测是否有四个角度的改变
                if (orientation > 80 && orientation < 100 && orientType == 0) { //90度
                    orientType = 1;
                } else if (orientation > 260 && orientation < 280 && orientType == 1) { //270度
                    orientType = 0;
                } else {
                    return;
                }
                PlatformUtils.onRotate(orientType);
            }
        };
        if (orientListener.canDetectOrientation()) {
            Log.d(TAG, "Can detect orientation");
            orientListener.enable();
        } else {
            Log.d(TAG, "Cannot detect orientation");
            orientListener.disable();
        }
    }

    public static void onDestroy() {
        orientListener.disable();
    }

    public static void onResume() {

    }

    // 获取网络类型
    public static String getNetworkType() {
        updateNetworkInfo();
        return mNetworkType + "#" + mSignalStrengthLevel;
    }

    public static void updateNetworkInfo() {
        ConnectivityManager connectivityManager = (ConnectivityManager) mActivity.getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo info = connectivityManager.getActiveNetworkInfo();

        if (info != null && info.isAvailable()) {
            switch (info.getType()) {
                case ConnectivityManager.TYPE_WIFI:
                    //wifi
                    mNetworkType = "WIFI";
                    mWifiStrengthLevel = getWifiSignalLevel();
                    break;
                case ConnectivityManager.TYPE_MOBILE:
                    //移动网络,可以通过TelephonyManager来获取具体细化的网络类型
                    TelephonyManager teleManager = (TelephonyManager) mActivity.getSystemService(Context.TELEPHONY_SERVICE);

                    if (ActivityCompat.checkSelfPermission(mActivity, Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED) {
                        // TODO: Consider calling
                        //    ActivityCompat#requestPermissions
                        // here to request the missing permissions, and then overriding
                        //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                        //                                          int[] grantResults)
                        // to handle the case where the user grants the permission. See the documentation
                        // for ActivityCompat#requestPermissions for more details.
                        return;
                    }
                    mNetworkType = _getTeleNetworkClass(teleManager.getNetworkType());
                    mSignalStrengthLevel = 4;
                    break;
            }
        } else {
            mNetworkType = "NONE";
            mSignalStrengthLevel = 0;
        }
    }

    public static int getWifiSignalLevel(){
        WifiManager manager = (WifiManager) mActivity.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        WifiInfo connectionInfo = manager.getConnectionInfo();
        return netDbmToLevel(connectionInfo.getRssi());
    }

    // 获取设备唯一标识
    public static String getUniq() {
        String serial;
        // 请求权限
        if (Build.VERSION.SDK_INT >= 28) {
            //验证是否许可权限
            if (mActivity.checkSelfPermission(Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED) {
                //申请权限
                mActivity.requestPermissions(new String[]{Manifest.permission.READ_PHONE_STATE}, REQUEST_CODE_READ_PHONE);
                return "";
            }

            // serial = android.os.Build.getSerial();
            serial = Settings.System.getString(mActivity.getApplicationContext().getContentResolver(), Settings.Secure.ANDROID_ID);
            Log.d(TAG,"API>=28,android.os.Build.SERIAL="+serial);
        }else{
            try {
                serial = android.os.Build.class.getField("SERIAL").get(null).toString();
                //API>=9 使用serial号
                Log.d(TAG,"api>=9,android.os.Build.SERIAL="+serial);
            } catch (Exception exception) {
                // serial需要一个初始化
                serial = "serial"; // 随便一个初始化
                Log.d(TAG,"api<9,serial="+serial);
            }
        }

        String szDevIDShort = "35" +
                Build.BOARD.length()%10+ Build.BRAND.length()%10 +

                Build.CPU_ABI.length()%10 + Build.DEVICE.length()%10 +

                Build.DISPLAY.length()%10 + Build.HOST.length()%10 +

                Build.ID.length()%10 + Build.MANUFACTURER.length()%10 +

                Build.MODEL.length()%10 + Build.PRODUCT.length()%10 +

                Build.TAGS.length()%10 + Build.TYPE.length()%10 +

                Build.USER.length()%10 ; //13 位
        // 使用硬件信息拼凑出来的15位号码
        String uuid = new UUID(szDevIDShort.hashCode(), serial.hashCode()).toString();
        Log.d(TAG,"uuid="+uuid);
        return uuid;
    }

    public static boolean isReadPhoneStateGranted(){
        //api>=28才需要这个权限
        if (Build.VERSION.SDK_INT >= 28) {
            //验证是否许可权限
            if (mActivity.checkSelfPermission(Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }

        return true;
    }

    public static void requestReadPhoneState(){
        if (bIsRequestingPermission)
            return;

        bIsRequestingPermission = true;
        //api>=28才需要这个权限
        if (Build.VERSION.SDK_INT >= 28) {
            //验证是否许可权限
            if (mActivity.checkSelfPermission(Manifest.permission.READ_PHONE_STATE) != PackageManager.PERMISSION_GRANTED) {
//                if (mActivity.shouldShowRequestPermissionRationale(Manifest.permission.READ_PHONE_STATE))
                //申请权限
                mActivity.requestPermissions(new String[]{Manifest.permission.READ_PHONE_STATE}, REQUEST_CODE_READ_PHONE);
//                else
//                    mActivity.showDialog("提示","为了您的账户安全，我们需要获取手机状态的权限来获取设备唯一标识，请在应用设置中打开该权限，如有问题请联系客服");
            }
        }
    }

    // 开始监听网络状态
    public static void startListenNetwork() {
        IntentFilter filter = new IntentFilter();
        filter.addAction(ConnectivityManager.CONNECTIVITY_ACTION);
        filter.addAction(WifiManager.RSSI_CHANGED_ACTION);
        mActivity.registerReceiver(mNetworkChangedReceiver,filter);
    }

    // 停止监听网络状态
    public static void stopListenNetwork() {
        mActivity.unregisterReceiver(mNetworkChangedReceiver);
    }

    public static void onNetworkChanged(){
        int signalStrength = mNetworkType.equals("WIFI")?mWifiStrengthLevel:mSignalStrengthLevel;
        final String scriptString = String.format("window.PlatformUtils.onNetworkChanged(\"%s\",%d);",mNetworkType,Math.max(0, Math.min(4, signalStrength)));
        Toast.makeText(mActivity,scriptString,Toast.LENGTH_LONG).show();
        Log.d(TAG,"当前为" + mNetworkType + ",信号强度为" + signalStrength);
    }

    private static String _getTeleNetworkClass(int teleNetworkType) {
        switch (teleNetworkType) {
            case TelephonyManager.NETWORK_TYPE_GPRS:
            case TelephonyManager.NETWORK_TYPE_EDGE:
            case TelephonyManager.NETWORK_TYPE_CDMA:
            case TelephonyManager.NETWORK_TYPE_1xRTT:
            case TelephonyManager.NETWORK_TYPE_IDEN:
                return "2G";
            case TelephonyManager.NETWORK_TYPE_UMTS:
            case TelephonyManager.NETWORK_TYPE_EVDO_0:
            case TelephonyManager.NETWORK_TYPE_EVDO_A:
            case TelephonyManager.NETWORK_TYPE_HSDPA:
            case TelephonyManager.NETWORK_TYPE_HSUPA:
            case TelephonyManager.NETWORK_TYPE_HSPA:
            case TelephonyManager.NETWORK_TYPE_EVDO_B:
            case TelephonyManager.NETWORK_TYPE_EHRPD:
            case TelephonyManager.NETWORK_TYPE_HSPAP:
                return "3G";
            case TelephonyManager.NETWORK_TYPE_LTE:
                return "4G";
            default:
                return "UNKNOWN";
        }
    }

    public static int netDbmToLevel(int netStrength) {
        if (netStrength<-85) {
            return 1;
        } else if (netStrength<-70) {
            return 2;
        } else if (netStrength<-55) {
            return 3;
        }
        return 4;
    }

    /**
     * 生成一个唯一的会话标识符
     * @param type 类型
     */
    private static String buildTransaction(final String type) {
        return (type == null) ? String.valueOf(System.currentTimeMillis()) : type + System.currentTimeMillis();
    }

    // 是否刘海屏
    public static boolean hasNotch() {
        return DeviceUtils.hasNotchInScreen(mActivity);
    }

    // 获取屏幕旋转方向 0横屏 1反向横屏
    public static int getOrientation() {
        return orientType;
    }

    // 屏幕旋转通知 0横屏 1反向横屏
    public static void onRotate(int rotate) {
        final String scriptString = String.format("window.PlatformUtils.onRotate(%d);",rotate);

    }

    public static void setOrientation(String dir){
        if(dir.equals("V"))
            mActivity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
        else
            mActivity.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
    }

    // 打开系统设置让用户开启权限
    private static void openSetting(final int requestCode,final String msg){
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                new AlertDialog.Builder(mActivity)
                        .setTitle("提示")
                        .setMessage(msg)
                        .setPositiveButton("确定"
                                , new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialog, int which) {
                                        Intent intent = new Intent(Settings.ACTION_APPLICATION_DETAILS_SETTINGS);
                                        Uri uri = Uri.fromParts("package", mActivity.getPackageName(), null);
                                        intent.setData(uri);
                                        mActivity.startActivityForResult(intent, requestCode);
                                        dialog.dismiss();
                                    }
                                })
                        .create()
                        .show();
            }
        });
    }
}
