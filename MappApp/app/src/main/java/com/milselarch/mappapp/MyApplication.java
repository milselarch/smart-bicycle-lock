package com.milselarch.mappapp;

import android.app.Application;
import android.bluetooth.BluetoothSocket;

/**
 * Created by lenovo on 15/8/2016.
 */
public class MyApplication extends Application {
    private BluetoothSocket btSock;
    private String someVariable;

    public BluetoothSocket getBtSock() {
        return btSock;
    }
    public void setBtSock(BluetoothSocket socket) {
        btSock = socket;
    }

    public String getSomeVariable() {
        return someVariable;
    }

    public void setSomeVariable(String someVariable) {
        this.someVariable = someVariable;
    }
}