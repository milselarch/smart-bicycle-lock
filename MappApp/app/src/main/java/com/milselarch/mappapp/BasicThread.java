package com.milselarch.mappapp;

import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;

/**
 * Created by lenovo on 15/8/2016.
 */

class BasicThread extends Thread {
    private final BluetoothSocket mmSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    private Handler onRecieveHandler;

    private static final int TYPE_MESSAGE = 1;

    public BasicThread(BluetoothSocket socket, Handler RecieveHandler) {
        onRecieveHandler = RecieveHandler;

        mmSocket = socket;
        InputStream tmpIn = null;
        OutputStream tmpOut = null;

        // Get the input and output streams, using temp objects because
        // member streams are final
        try {
            tmpIn = socket.getInputStream();
            tmpOut = socket.getOutputStream();
        } catch (IOException e) { }

        mmInStream = tmpIn;
        mmOutStream = tmpOut;
    }

    public void run() {
        byte[] buffer = new byte[1024];  // buffer store for the stream
        int bytes; // bytes returned from read()

        // Keep listening to the InputStream until an exception occurs
        while (true) {
            try {
                // Read from the InputStream
                Log.d("BYTES-READING","--");
                bytes = mmInStream.read(buffer);
                Log.d("BYTES-RECIEVED", Arrays.toString(buffer));
                // Send the obtained bytes to the UI activity
                onRecieveHandler.obtainMessage(TYPE_MESSAGE , bytes, -1, buffer)
                        .sendToTarget();

            } catch (IOException e) {
                break;
            }
        }
    }

    /* Call this from the main activity to send data to the remote device */
    public void write(byte[] bytes) {
        try {
            mmOutStream.write(bytes);
        } catch (IOException e) { }
    }

    /* Call this from the main activity to shutdown the connection */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) { }
    }
}