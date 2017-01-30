package com.milselarch.mappapp;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

/**
 * Created by lenovo on 30/12/2016.
 * AppCompatActivity that asks for runtime permissions for
 * ACCESS_FINE_LOCATION and ACCESS_COARSE_LOCATION
 * (this is required for both permissions to work in API 23+)
 */

public class AppCompatActivityApi6 extends AppCompatActivity {
    private static final String[] INITIAL_PERMS = {
        android.Manifest.permission.ACCESS_FINE_LOCATION,
        android.Manifest.permission.ACCESS_COARSE_LOCATION
    };

    private static final int INITIAL_REQUEST = 1337;
    private Activity contextActivity;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        if (this.hasPermission() == false){
            this.contextActivity = contextActivity;
            Log.d("GPS", "NEED SET GPS POSITION");
            setRuntimePermission();
        }

        super.onCreate(savedInstanceState);
    }

    @TargetApi(23)
    private boolean setRuntimePermission() {
        requestPermissions(INITIAL_PERMS, INITIAL_REQUEST);
        return(canAccessLocation());
    }

    @TargetApi(23)
    private boolean canAccessLocation() {
        return(
            _hasPermission(android.Manifest.permission.ACCESS_FINE_LOCATION)
            && _hasPermission(android.Manifest.permission.ACCESS_COARSE_LOCATION)
        );
    }

    public boolean hasPermission() {
        int currentapiVersion = android.os.Build.VERSION.SDK_INT;
        // run time location permission request needed to access location
        // in mums phone: Android level >= 7 (API 23)

        if (currentapiVersion >= Build.VERSION_CODES.M) {
            this.contextActivity = contextActivity;
            return this.canAccessLocation();
        } else {
            return true;
        }
    }

    @TargetApi(23)
    private boolean _hasPermission(String perm) {
        return (
            PackageManager.PERMISSION_GRANTED ==
            checkSelfPermission(perm)
        );
    }
}
