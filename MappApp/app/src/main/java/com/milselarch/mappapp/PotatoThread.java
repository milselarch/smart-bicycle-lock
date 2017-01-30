package com.milselarch.mappapp;

import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.Arrays;
import java.util.Queue;
import java.util.Vector;

/**
 * Created by lenovo on 15/8/2016.
 * simple length-based protocol for reading/writing bluetooth messages:
 * Message is formatted as follows
 * [length of message excluding this char][header length][header][body]
 */


class PotatoThread extends Thread {
    private final BluetoothSocket mmSocket;
    private final InputStream mmInStream;
    private final OutputStream mmOutStream;
    private Handler onRecieveHandler;

    private static final int TYPE_MESSAGE = 1;
    private Queue<Packet> packetQueue;

    public class Packet {
        public String header = "";
        public String body = "";

        Packet(byte[] packetBytes) {
            if (packetBytes == null) {
                header = null;
                body = null;

            } else {
                acqPacketHeader(packetBytes);
                acqPacketBody(packetBytes);
            }
        }

        byte acqPacketHeader(byte[] packetBytes) {
            for (int i = 0; i < packetBytes[1]; i++) {
                header += (char) packetBytes[2+i];
            }

            return packetBytes[1];
        }

        byte acqPacketBody(byte[] packetBytes) {
            final int length = packetBytes[0]-packetBytes[1]-1;
            final int bodyIndex = packetBytes[1]+2;

            for (int i = 0; i < length; i++) {
                body += (char) packetBytes[bodyIndex+i];
            }

            return (byte) length;
        }

    }

    public PotatoThread(BluetoothSocket socket, Handler RecieveHandler) {
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
        byte[] buffer = new byte[257];  // buffer store for the stream
        byte[] charBuffer = new byte[1];
        int recvIndex = 0;
        int bytes; // bytes returned from read()

        // Keep listening to the InputStream until an exception occurs
        while (true) {
            try {
                // Read from the InputStream
                Log.d("BYTES-READING","--");
                bytes = mmInStream.read(charBuffer);

                if (bytes == 1) {
                    buffer[recvIndex] = charBuffer[0];
                    charBuffer[0] = 0;
                    recvIndex++;

                    if (recvIndex > buffer[0]) {
                        Log.d("BYTES-RECIEVED", Arrays.toString(buffer));
                        onRecieveHandler.obtainMessage(
                                TYPE_MESSAGE , bytes, -1, new Packet(buffer)
                        ).sendToTarget();

                        for (int i=0; i<257; i++) {buffer[i] = 0;}
                        recvIndex = 0;
                    }

                } else if (bytes == -1) {
                    Log.e("BYTES RECIEVED -1","POTATO");
                    onRecieveHandler.obtainMessage(
                            TYPE_MESSAGE , bytes, -1, new Packet(null)
                    ).sendToTarget();
                    cancel();
                }

                // Send the obtained bytes to the UI activity

            } catch (IOException e) {
                break;
            }
        }
    }

    public void sendPacket(String header, String body) {
        byte[] headerBytes = header.getBytes();
        byte[] bodyBytes = body.getBytes();
        byte packet[] = new byte[headerBytes.length + bodyBytes.length + 2];
        packet[0] = (byte) (headerBytes.length + bodyBytes.length + 1);
        packet[1] = (byte) headerBytes.length;

        int index = 2;
        for (int i = 0; i < headerBytes.length; i++) {
            packet[index] = headerBytes[i];
            index++;
        }
        for (int i = 0; i < bodyBytes.length; i++) {
            packet[index] = bodyBytes[i];
            index++;
        }

        Log.d("PACKET-SENT", Arrays.toString(packet));
        write(packet);
    }


    /* Call this from the main activity to send data to the remote device */
    private void write(byte[] bytes) {
        try {
            mmOutStream.write(bytes);
        } catch (IOException e) {
            Log.e("IOE-EXCEPTION", "WRITE");
            onRecieveHandler.obtainMessage(
                    TYPE_MESSAGE , 0, -1, new Packet(null)
            ).sendToTarget();
            cancel();
        }
    }

    /* Call this from the main activity to shutdown the connection */
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) { }
    }
}