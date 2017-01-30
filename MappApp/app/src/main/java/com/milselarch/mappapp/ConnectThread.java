package com.milselarch.mappapp;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.util.Log;

import java.io.IOException;
import java.util.UUID;

//reference material: https://developer.android.com/guide/topics/connectivity/bluetooth.html

class ConnectThread extends Thread {
    private final BluetoothSocket mmSocket;
    private final BluetoothDevice mmDevice;
    //BluetoothAdapter m_BluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
    //String m_bluetoothAdd = m_BluetoothAdapter.getAddress();
    //private final UUID MY_UUID = UUID.randomUUID();
    private final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private manageConnectedSocket connectCallback;

    public ConnectThread(BluetoothDevice device, manageConnectedSocket callback) {
        connectCallback = callback;
        // Use a temporary object that is later assigned to mmSocket,
        // because mmSocket is final
        BluetoothSocket tmp = null;
        mmDevice = device;

        // Get a BluetoothSocket to connect with the given BluetoothDevice
        try {
            // MY_UUID is the app's UUID string, also used by the server code
            tmp = device.createRfcommSocketToServiceRecord(MY_UUID);
        } catch (IOException e) { }
        mmSocket = tmp;
    }

    public void run() {
        // Cancel discovery because it will slow down the connection
        //mBluetoothAdapter.cancelDiscovery();

        try {
            // Connect the device through the socket. This will block
            // until it succeeds or throws an exception
            mmSocket.connect();
            Log.d("CONNECT-THREAD","SUCESSS");
        } catch (IOException connectException) {
            // Unable to connect; close the socket and get out
            try {
                mmSocket.close();
            } catch (IOException closeException) { }
            Log.d("CONNECT-THREAD","FAIL");
            connectCallback.onSocketConnected(false, mmSocket);
            return;
        }

        // Do work to manage the connection (in a separate thread)
        Log.d("CONNECT-THREAD", "CALL");
        connectCallback.onSocketConnected(true, mmSocket);
    }

    /** Will cancel an in-progress connection, and close the socket */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) { }
    }

    public interface manageConnectedSocket {
        public abstract void onSocketConnected(boolean sucess, BluetoothSocket socket);
    }
}