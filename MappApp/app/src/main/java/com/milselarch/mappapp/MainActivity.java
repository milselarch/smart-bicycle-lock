package com.milselarch.mappapp;

import android.Manifest;
import android.app.Activity;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Build;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.Toast;

import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;

import static android.R.attr.action;

/*
* icons taken and/or adapted from www.materialui.co
* */

public class MainActivity extends AppCompatActivityApi6 {
    private ListView mListView;
    private DeviceListAdapter mAdapter;
    private ArrayList<BluetoothDevice> mDeviceList = new ArrayList<BluetoothDevice>();

    private ProgressDialog mProgressDlg;
    private BluetoothAdapter mBluetoothAdapter;
    private IntentFilter filter;
    private MenuItem refreshBttn;

    private ConnectThread btConnector;
    private boolean isDiscovering = false;
    private boolean isBuzzing = false;

    private final static int MY_PERMISSIONS_REQUEST_ACCESS_COARSE_LOCATION = 1;
    private final static int MY_PERMISSIONS_REQUEST_ACCESS_FINE_LOCATION = 2;

    public final static int BLUETOOTH_REQUEST_CODE = 1;

    /*
    private void checkBluetooth(int delay) {
        while (mBluetoothAdapter.getState() == BluetoothAdapter.STATE_TURNING_ON) {};
        if (mBluetoothAdapter.getState() != BluetoothAdapter.STATE_ON) {
            Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(intent, delay);
        }
    }
    */

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Log.d("VERS:","POTATO 0.01");

        mListView = (ListView) findViewById(R.id.lv_devices);

        mProgressDlg = new ProgressDialog(this);
        mProgressDlg.setMessage("Scanning...");
        mProgressDlg.setCancelable(false);

        mProgressDlg.setButton(DialogInterface.BUTTON_NEGATIVE, "Cancel", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                dialog.dismiss();
                mBluetoothAdapter.cancelDiscovery();
            }
        });

        //mAdapter = new ArrayAdapter<BluetoothDevice>(this, R.layout.list_item_device, mDeviceList);
        //mListView.setAdapter(mAdapter);
        startDiscovery();
    }

    private void startDiscovery() {
        mDeviceList.clear();

        filter = new IntentFilter();
        filter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        filter.addAction(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        filter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        registerReceiver(mReceiver, filter);

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        if (mBluetoothAdapter.getState() != mBluetoothAdapter.STATE_ON) {
            onBluetoothIntent();
        } else {
            mBluetoothAdapter.startDiscovery();
            isDiscovering = true;
            Log.d("BLUETOOTH_DISCOVERY", "ON");
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.mainmenu, menu);
        //displayToast("inflating...");
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onPrepareOptionsMenu(Menu menu) {
        //Log.d("HOST", Integer.toString(host.getCurrentTab()));
        refreshBttn = menu.findItem(R.id.action_refresh);
        return super.onPrepareOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case (R.id.action_refresh):
                startDiscovery();
                break;
        }
        return super.onOptionsItemSelected(item);
    }

    private void pairDevice(BluetoothDevice device) {
        try {
            Method method = device.getClass().getMethod("createBond", (Class[]) null);
            method.invoke(device, (Object[]) null);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void unpairDevice(BluetoothDevice device) {
        try {
            Method method = device.getClass().getMethod("removeBond", (Class[]) null);
            method.invoke(device, (Object[]) null);

        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    void initArrayAdaptor() {
        mAdapter = new DeviceListAdapter(this);
        mAdapter.setData(mDeviceList);
        mAdapter.setListener(new DeviceListAdapter.OnPairButtonClickListener() {
            @Override
            public void onPairButtonClick(int position) {
                final BluetoothDevice device = mDeviceList.get(position);

                if (device.getBondState() == BluetoothDevice.BOND_BONDED) {
                    Log.d("connecting to bonded:","lol");
                    displayToast("connecting...");
                    btConnector = new ConnectThread(device, new ConnectThread.manageConnectedSocket() {
                        @Override
                        public void onSocketConnected(boolean sucess, BluetoothSocket socket) {
                            if (sucess == true) {
                                Log.d("CONNECTED-SOCK",socket.toString());
                                Intent newIntent = new Intent(MainActivity.this, LockActivity.class);
                                //Intent intent = new Intent (this, BTActivity.class);
                                ((MyApplication) getApplication()).setBtSock(socket);
                                newIntent.putExtra("btdevice", device);
                                //newIntent.putExtra("btsocket", socket);
                                startActivity(newIntent);
                                //newIntent.putParcelableArrayListExtra("device.list", mDeviceList);
                                //startActivity(newIntent);


                            } else {
                                Log.d("FAILED-CSOCK",socket.toString());
                                unpairDevice(device);
                                btConnector.cancel();
                            }
                        }
                    });

                    btConnector.start();
                    //unpairDevice(device);
                } else {
                    displayToast("Pairing...");
                    pairDevice(device);
                }
            }
        });

        mListView.setAdapter(mAdapter);
        mAdapter.setData(mDeviceList);
        mAdapter.notifyDataSetChanged();
    }

    void onBluetoothIntent(int delay) {
        Intent newIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        setResult(Activity.RESULT_OK, newIntent);
        newIntent.setAction(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(newIntent, BLUETOOTH_REQUEST_CODE);
    }
    void onBluetoothIntent() {
        Intent newIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        setResult(Activity.RESULT_OK, newIntent);
        newIntent.setAction(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        startActivityForResult(newIntent, BLUETOOTH_REQUEST_CODE);
    }

    @Override
    public void onPause() {
        if (mBluetoothAdapter != null) {
            if (mBluetoothAdapter.isDiscovering()) {
                mBluetoothAdapter.cancelDiscovery();
            }
        }

        super.onPause();
    }

    @Override
    final protected void onDestroy() {
        // Unregister broadcast listeners
        unregisterReceiver(mReceiver);
        super.onDestroy();
    }

    private void displayToast(String data) {
        Toast.makeText(getApplicationContext(), data, Toast.LENGTH_SHORT).show();
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent intent) {
        //Log.d("INTENT_ACTION", intent.getAction());
        //Log.d("INTENT_BLUETOOTH", BluetoothAdapter.ACTION_REQUEST_ENABLE);

        if (requestCode == BLUETOOTH_REQUEST_CODE) { //(resultCode == Activity.RESULT_OK) {
            if (resultCode == Activity.RESULT_OK) {
                if (mBluetoothAdapter.getState() == mBluetoothAdapter.STATE_OFF) {
                    onBluetoothIntent(500);
                    System.out.println("one");
                } else {
                    System.out.println("two");
                }

                System.out.println("RESULT BLOB");

            } else if (resultCode == Activity.RESULT_CANCELED) {
                Log.d("WRONG_RESULT_CODE", "RESULT_CANCELLED");
                onBluetoothIntent(500);

            } else {
                Log.d("WRONG_RESULT_CODE", String.valueOf(resultCode));
            }
        }
    }

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d("INTENT-ACTION-B", action);

            if (BluetoothAdapter.ACTION_STATE_CHANGED.equals(action)) {
                int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);

                if (state == BluetoothAdapter.STATE_ON) {
                    Toast.makeText(getApplicationContext(), "Bluetooth enabled", Toast.LENGTH_SHORT).show();
                    //showBluetoothEnabled();

                } else if (state == BluetoothAdapter.STATE_OFF) {
                    //showBluetoothDisabled();
                    Toast.makeText(getApplicationContext(), "Bluetooth disabled", Toast.LENGTH_SHORT).show();
                    onBluetoothIntent(500);

                }

            } else if (BluetoothAdapter.ACTION_DISCOVERY_STARTED.equals(action)) {
                Toast.makeText(getApplicationContext(), "discovering...", Toast.LENGTH_SHORT).show();
            } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                Toast.makeText(getApplicationContext(), "discovery done", Toast.LENGTH_SHORT).show();
                isDiscovering = false;
                initArrayAdaptor();

            } else if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                BluetoothDevice device = (BluetoothDevice) intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                try {
                    Log.d("ADD_DEVICE:", device.getName());
                } catch (Exception e) {
                    Log.e("DEVICE_BT_NAME", "exception", e);
                }

                Toast.makeText(getApplicationContext(), "Found device " + device.getName(), Toast.LENGTH_SHORT).show();
                //mDeviceList.add(device);
                mDeviceList.add(device);
                //mListView.setAdapter(mAdapter);

            } else if (BluetoothDevice.ACTION_BOND_STATE_CHANGED.equals(action)) {
                int state = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, BluetoothDevice.ERROR);
                int prevState = intent.getIntExtra(BluetoothDevice.EXTRA_PREVIOUS_BOND_STATE, BluetoothDevice.ERROR);

                if (state == BluetoothDevice.BOND_BONDED && prevState == BluetoothDevice.BOND_BONDING) {
                    displayToast("Paired");
                } else if (state == BluetoothDevice.BOND_NONE && prevState == BluetoothDevice.BOND_BONDED){
                    displayToast("Unpaired");
                }

                mAdapter.notifyDataSetChanged();
            }
        }
    };
}

