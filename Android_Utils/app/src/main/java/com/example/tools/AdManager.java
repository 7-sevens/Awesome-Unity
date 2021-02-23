package com.example.tools;

//import android.content.Context;
import android.os.Looper;
import android.view.Gravity;
import android.view.View;
import android.widget.FrameLayout;
import android.widget.LinearLayout;
import android.widget.Toast;
import android.util.Log;

import com.google.android.gms.ads.AdListener;
import com.google.android.gms.ads.AdRequest;
import com.google.android.gms.ads.AdSize;
import com.google.android.gms.ads.AdView;
import com.google.android.gms.ads.LoadAdError;
import com.google.android.gms.ads.MobileAds;
import com.google.android.gms.ads.reward.RewardItem;
import com.google.android.gms.ads.reward.RewardedVideoAd;
import com.google.android.gms.ads.reward.RewardedVideoAdListener;

public class ADManager implements RewardedVideoAdListener {
    final static String TAG = "ADManager";
    // 运营广告ID(Google Play)
    private static final String APP_ID = "ca-app-pub-5233446190113804~9866779404"; //应用ID
    private static final String AD_BANNER_UNIT_ID = "ca-app-pub-5233446190113804/2705986985"; //横幅广告ID
    private static final String AD_VIDEO_ID = "ca-app-pub-5233446190113804/6608887215"; //激励视频广告ID

    // 运营广告ID(国内Android商店)
    //private static final String APP_ID = "ca-app-pub-5233446190113804~8454466674"; //应用ID
    //private static final String AD_BANNER_UNIT_ID = "ca-app-pub-5233446190113804/5800383170"; //横幅广告ID
    //private static final String AD_VIDEO_ID = "ca-app-pub-5233446190113804/9844952859"; //激励视频广告ID

    // 测试广告ID
    //private static final String AD_BANNER_UNIT_ID = "ca-app-pub-3940256099942544/6300978111"; //横幅广告ID
    //private static final String AD_VIDEO_ID = "ca-app-pub-3940256099942544/5224354917"; //激励视频广告ID

    private static ADManager mInstace = null;
    private static AppActivity mainActive = null;

    private AdView mAdView;
    private LinearLayout bannerLayout;
    private RewardedVideoAd rewardedVideoAd;

    private static boolean bInit = false;
    private static boolean isVideoRewarded = false;
    private static boolean isVideoClose = false;

    public static ADManager getInstance() {
        if (null == mInstace) {
            mInstace = new ADManager();
        }
        return mInstace;
    }

    public void init(AppActivity activity) {
        this.mainActive = activity;

        //初始化广告 SDK.
        MobileAds.initialize(activity, APP_ID);

        //预加载激励视频广告
        rewardedVideoAd = MobileAds.getRewardedVideoAdInstance(activity);
        rewardedVideoAd.setRewardedVideoAdListener(this);
        loadRewardedVideoAd();

        //预加载banner广告
        loadBannerAd();

        bInit = true;
    }

    public static void callJSMethod(final String scriptString) {
        Log.d(TAG,scriptString);
        if (!bInit) {
            Log.d(TAG,"JS not ready");
            return;
        }

        mainActive.runOnGLThread(new Runnable() {
            @Override
            public void run() {
                Cocos2dxJavascriptJavaBridge.evalString(scriptString);
            }
        });
    }

    /**
     * 加载google banner广告
     */
    public void loadBannerAd() {
        bannerLayout = new LinearLayout(this.mainActive);
        bannerLayout.setOrientation(LinearLayout.VERTICAL);

        // Create a banner ad. The ad size and ad unit ID must be set before calling loadAd.
        mAdView = new AdView(this.mainActive);
        mAdView.setAdSize(AdSize.SMART_BANNER);
        mAdView.setAdUnitId(AD_BANNER_UNIT_ID);

        // Create an ad request.
        AdRequest.Builder adRequestBuilder = new AdRequest.Builder();

        // Optionally populate the ad request builder.
        adRequestBuilder.addTestDevice(AdRequest.DEVICE_ID_EMULATOR);

        // Add the AdView to the view hierarchy.
        bannerLayout.addView(mAdView);

        // Start loading the ad.
        mAdView.loadAd(adRequestBuilder.build());

        AppActivity activity = (AppActivity)this.mainActive;
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(FrameLayout.LayoutParams.MATCH_PARENT, FrameLayout.LayoutParams.WRAP_CONTENT);
        params.gravity = Gravity.BOTTOM;
        activity.addContentView(bannerLayout,params);
        bannerLayout.setVisibility(View.INVISIBLE);

        mAdView.setAdListener(new AdListener() {
            @Override
            public void onAdLoaded() {
                // Code to be executed when an ad finishes loading.
                Log.d(TAG,"onAdLoaded");
            }

            @Override
            public void onAdFailedToLoad(LoadAdError adError) {
                // Code to be executed when an ad request fails.
                Log.d(TAG,"onAdFailedToLoad");
            }

            @Override
            public void onAdOpened() {
                // Code to be executed when an ad opens an overlay that
                // covers the screen.
                Log.d(TAG,"onAdOpened");
            }

            @Override
            public void onAdClicked() {
                // Code to be executed when the user clicks on an ad.
                Log.d(TAG,"onAdClicked");
            }

            @Override
            public void onAdLeftApplication() {
                // Code to be executed when the user has left the app.
                Log.d(TAG,"onAdLeftApplication");
            }

            @Override
            public void onAdClosed() {
                // Code to be executed when the user is about to return
                // to the app after tapping on an ad.
                Log.d(TAG,"onAdClosed");
            }
        });
    }

    /**
     * 显示google banner广告
     */
    public static void showBannerAd() {
        Log.d(TAG,"showBannerAd");
        AppActivity mActivity = (AppActivity) ADManager.getInstance().mainActive;
        //一定要确保在UI线程操作
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ADManager.getInstance().bannerLayout.setVisibility(View.VISIBLE);
            }
        });
    }

    /**
     * 隐藏google banner广告
     */
    public static void hideBannerAd() {
        Log.d(TAG,"hideBannerAd");
        AppActivity mActivity = (AppActivity) ADManager.getInstance().mainActive;
        //一定要确保在UI线程操作
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                ADManager.getInstance().bannerLayout.setVisibility(View.INVISIBLE);
            }
        });
    }

    /**
     * 预加载google视频广告
     */
    public void loadRewardedVideoAd() {
        if (!rewardedVideoAd.isLoaded()) {
            rewardedVideoAd.loadAd(AD_VIDEO_ID, new AdRequest.Builder().build());
        }
    }

    /**
     * 显示视频广告
     */
    public static void showRewardedVideo() {
        Log.d(TAG,"showRewardedVideo");
        ADManager.getInstance().isVideoRewarded = false;
        ADManager.getInstance().isVideoClose = false;

        AppActivity mActivity = (AppActivity) ADManager.getInstance().mainActive;
        //一定要确保在UI线程操作
        mActivity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (ADManager.getInstance().rewardedVideoAd.isLoaded()) {
                    ADManager.getInstance().rewardedVideoAd.show();
                }
            }
        });
    }

    @Override
    public void onRewardedVideoAdLeftApplication() {
        Log.d(TAG,"onRewardedVideoAdLeftApplication");
    }

    @Override
    public void onRewardedVideoAdClosed() {
        Log.d(TAG,"onRewardedVideoAdClosed");
        ADManager.getInstance().isVideoClose = true;

        // reward call back
        if (ADManager.getInstance().isVideoRewarded) {
            final String scriptString = String.format("window.ADTools.videoADRewardedCB();");
            callJSMethod(scriptString);
        }

        //预加载下一个视频广告
        loadRewardedVideoAd();
    }

    @Override
    public void onRewardedVideoAdFailedToLoad(int errorCode) {
        String s = "onRewardedVideoAdFailedToLoad." + Integer.toString(errorCode);
        Log.d(TAG, s);
    }

    @Override
    public void onRewardedVideoAdLoaded() {
        Log.d(TAG,"onRewardedVideoAdLoaded");
    }

    @Override
    public void onRewardedVideoAdOpened() {
        Log.d(TAG,"onRewardedVideoAdOpened");
    }

    @Override
    public void onRewarded(RewardItem reward) {
        Log.d(TAG,"onRewarded");
        ADManager.getInstance().isVideoRewarded = true;
    }

    @Override
    public void onRewardedVideoStarted() {
        Log.d(TAG,"onRewardedVideoStarted");
    }

    @Override
    public void onRewardedVideoCompleted() {
        Log.d(TAG,"onRewardedVideoCompleted");
    }

    // 用于监听视频广告播放完成
    public static boolean videoRewardedListener() {
        return ADManager.getInstance().isVideoRewarded;
    }

    // 用于cocos监听视频广告播放关闭
    public static boolean videoCloseListener() {
        return ADManager.getInstance().isVideoClose;
    }

    public void onResume() {
        mAdView.resume();
        rewardedVideoAd.resume(this.mainActive);
    }

    public void onPause() {
        mAdView.pause();
        rewardedVideoAd.pause(this.mainActive);
    }

    public void onDestroy() {
        mAdView.destroy();
        rewardedVideoAd.destroy(this.mainActive);
    }
}
