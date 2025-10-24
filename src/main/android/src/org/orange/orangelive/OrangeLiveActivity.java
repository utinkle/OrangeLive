package org.orange.orangelive;

import org.qtproject.qt.android.QtNative;
import org.qtproject.qt.android.bindings.QtActivity;

import android.os.*;
import android.content.*;
import android.app.*;

import android.content.res.Resources;
import android.content.res.Configuration;
import android.util.DisplayMetrics;
import android.util.Log;

import android.view.Display;

import android.hardware.display.DisplayManager;
import android.view.Surface;
import android.view.View;
import android.view.DisplayCutout;
import android.view.Window;
import android.view.WindowManager;
import android.view.WindowInsets;
import android.view.WindowInsetsController;
import android.graphics.Color;

import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsCompat.Type.InsetsType;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsControllerCompat;

public class OrangeLiveActivity extends QtActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        enable(this, true);
    }

    public static void enable(Activity activity, boolean isLight) {
        Log.d("EdgeToEdge", "Enable edge-to-edge called");
        Window window = activity.getWindow();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS | WindowManager.LayoutParams.FLAG_TRANSLUCENT_NAVIGATION);
            window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS);
            window.getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            );

            window.setNavigationBarContrastEnforced(false);
            window.setStatusBarContrastEnforced(false);
            window.setStatusBarColor(android.graphics.Color.TRANSPARENT);
            window.setNavigationBarColor(android.graphics.Color.TRANSPARENT);
            Log.d("EdgeToEdge", "Edge-to-edge enabled");
        } else {
            Log.d("EdgeToEdge", "Edge-to-edge not supported on this SDK version");
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            WindowInsetsControllerCompat windowInsetsController = ViewCompat.getWindowInsetsController(window.getDecorView());
            windowInsetsController.setAppearanceLightNavigationBars(isLight);
            windowInsetsController.setAppearanceLightStatusBars(isLight);
        } else if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            if (isLight) {
                window.getDecorView().setSystemUiVisibility(window.getDecorView().getSystemUiVisibility() | View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR);
                window.getDecorView().setSystemUiVisibility(window.getDecorView().getSystemUiVisibility() | View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
            } else {
                window.getDecorView().setSystemUiVisibility(window.getDecorView().getSystemUiVisibility() & ~View.SYSTEM_UI_FLAG_LIGHT_NAVIGATION_BAR);
                window.getDecorView().setSystemUiVisibility(window.getDecorView().getSystemUiVisibility() & ~View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR);
            }
        }
    }
}
