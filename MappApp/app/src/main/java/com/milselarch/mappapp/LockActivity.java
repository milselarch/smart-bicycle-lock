package com.milselarch.mappapp;

import android.app.Activity;
import android.app.Notification;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.content.SharedPreferences;

import java.util.Arrays;

/**
 * Created by lenovo on 15/8/2016.
 */

public class LockActivity extends Activity {
    public BluetoothDevice activeDevice;
    public BluetoothSocket socket;
    //Handler onPacketRecieve;
    PotatoThread btThread;

    private int STATUS_OFF = 0;
    private int STATUS_TURNING_OFF = 1;
    private int STATUS_TURNING_ON = 2;
    private int STATUS_ON = 3;

    private int isEditing = STATUS_OFF;
    private int isLocked = STATUS_OFF;
    private int isBuzzing = STATUS_OFF;
    private int isAuthenticate = STATUS_OFF;

    TextView LockNameText;
    TextView LockMACText;
    EditText editUnlock;
    EditText editReenter;
    EditText editPassword;
    Button modPasswordBttn;

    TextView commentsText;

    Button lockBttn;
    Button soundBttn;
    Button editBttn;

    private String defaultPassword = "";
    private String password = "";

    public static final String MY_PREFS_NAME = "MyPrefs";
    SharedPreferences sharedPref;

    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_lock);
        Log.d("MEH", "start-lock");

        sharedPref = getSharedPreferences(MY_PREFS_NAME, MODE_PRIVATE);

        activeDevice = getIntent().getExtras().getParcelable("btdevice");
        LockNameText = (TextView) findViewById(R.id.device_name);
        LockNameText.setText(activeDevice.getName());
        LockMACText = (TextView) findViewById(R.id.device_MAC);
        LockMACText.setText(activeDevice.getAddress());

        editUnlock = (EditText) findViewById(R.id.edit_password);
        editReenter = (EditText) findViewById(R.id.edit_password2);
        editReenter.setVisibility(View.GONE);
        editPassword = (EditText) findViewById(R.id.edit_new_password);
        editPassword.setVisibility(View.GONE);
        modPasswordBttn = (Button) findViewById(R.id.change_password);
        modPasswordBttn.setVisibility(View.GONE);

        commentsText = (TextView) findViewById(R.id.text_comment);
        commentsText.setVisibility(View.GONE);

        lockBttn = (Button) findViewById(R.id.btn_lock);
        soundBttn = (Button) findViewById(R.id.btn_sound);
        editBttn = (Button) findViewById(R.id.btn_edit);

        Log.d("DEF-PASSSTART", "START[");
        this.loadPreferences();
        Log.d("DEF-INIT-PASSREAD", defaultPassword);
        editUnlock.setText(defaultPassword);

        socket = ((MyApplication) getApplication()).getBtSock();

        btThread = new PotatoThread(socket, this.onPacketRecieve);
        btThread.start();
        btThread.sendPacket("STATUS", "");

        lockBttn.setOnClickListener(this.lockBttnClick);
        soundBttn.setOnClickListener(soundBttnClick);
        modPasswordBttn.setOnClickListener(modPasswordBttnClick);
        editBttn.setOnClickListener(editBttnClick);

        editPassword.setOnKeyListener(editPasswordPress);
    }

    private View.OnClickListener editBttnClick = new View.OnClickListener() {
        public void onClick(View v) {
            displayToast("editing...");

            if (isEditing == STATUS_OFF) {
                isEditing = STATUS_ON;
                editReenter.setVisibility(View.VISIBLE);
                editPassword.setVisibility(View.VISIBLE);
                modPasswordBttn.setVisibility(View.VISIBLE);

                editUnlock.setText("");
                editReenter.setText("");

            } else {
                isEditing = STATUS_OFF;
                editReenter.setVisibility(View.GONE);
                editPassword.setVisibility(View.GONE);
                modPasswordBttn.setVisibility(View.GONE);

                editUnlock.setText(defaultPassword);
            }
        }
    };

    private View.OnClickListener modPasswordBttnClick = new View.OnClickListener() {
        public void onClick(View v) {
            displayToast("editing...");

            String password1 = editUnlock.getText().toString();
            String password2 = editReenter.getText().toString();

            if (password1.equals(password2)) {
                if (isAuthenticate == STATUS_OFF) {
                    isAuthenticate = STATUS_TURNING_ON;
                    modPasswordBttn.setBackgroundResource(R.drawable.input_blue);
                    changePassword(password1,editPassword.getText().toString());

                } else if (isAuthenticate == STATUS_TURNING_ON) {
                    displayToast("authenticating...");
                } else {
                    displayToast("WROMG PASSWORD STATUS: "+Integer.toString(isAuthenticate));
                }

            } else {
                displayToast("passwords do not match!");
            }

        }
    };

    private View.OnKeyListener editPasswordPress = new View.OnKeyListener() {
        public boolean onKey(View v, int keyCode, KeyEvent event) {
            // If the event is a key-down event on the "enter" button
            if ((event.getAction() == KeyEvent.ACTION_DOWN) &&
                    (keyCode == KeyEvent.KEYCODE_ENTER)) {
                // Perform action on key press
                displayToast("Entered data... [EDITEXT]");
                return true;
            }
            return false;
        }
    };

    private View.OnClickListener soundBttnClick = new View.OnClickListener() {
        public void onClick(View v) {
            if (isBuzzing == STATUS_ON) {
                isBuzzing = STATUS_TURNING_OFF;
                btThread.sendPacket("UNBUZZ","1");
                soundBttn.setBackgroundResource(R.drawable.bluetooth_searching_blue);
                displayToast("unbuzzing");

            } else if (isBuzzing == STATUS_TURNING_ON) {
                displayToast("turning on buzzer...");

            } else if (isBuzzing == STATUS_TURNING_OFF) {
                displayToast("turning off buzzer...");

            } else if (isBuzzing == STATUS_OFF) {
                isBuzzing = STATUS_TURNING_ON;
                btThread.sendPacket("BUZZ","1");
                soundBttn.setBackgroundResource(R.drawable.bluetooth_disabled_blue);
                displayToast("buzzing!");

            } else {
                displayToast("WROMG BUZZ STATUS: "+Integer.toString(isBuzzing));
            }
        }
    };

    private Handler onPacketRecieve = new Handler() {
        public void handleMessage(Message mssg) {
            String header = ((PotatoThread.Packet) mssg.obj).header;
            String body = ((PotatoThread.Packet) mssg.obj).body;

            if (header == null) {
                //onDestroy();
                displayToast("connection disconneted");
                //finish();
                (new Handler()).postDelayed(new Runnable() {
                    @Override
                    public void run() {finish(); /*Do something after 100ms*/ }
                }, 1000);

            } else if (header.equals("STATUS_RETURN")) {
                switch ((int) body.charAt(0)) {
                    case 1:
                        unlockLockBttn();
                        loadPreferences();
                        break;
                    case 2:
                        lockLockBttn();
                        loadPreferences();
                        break;
                    default:
                        Log.d("ERRRRRRORRRRRRR STATUS", Integer.toString((int) body.charAt(0)));
                }

            } else if (header.equals("unlocked")) {
                setDefaultPassword();
                unlockLockBttn();
            } else if (header.equals("locked")) {
                setDefaultPassword();
                lockLockBttn();

            } else if (header.equals("UNLOCK_FAIL")) {
                revertLockStatus();
                displayToast("Wrong password - cannot unlock");

            } else if (header.equals("LOCK_FAIL")) {
                revertLockStatus();
                displayToast("Wrong password - cannot lock");

            } else if (header.equals("BUZZED")) {
                isBuzzing = STATUS_ON;
                soundBttn.setBackgroundResource(R.drawable.bluetooth_searching_black);
            } else if (header.equals("UNBUZZED")) {
                isBuzzing = STATUS_OFF;
                soundBttn.setBackgroundResource(R.drawable.bluetooth_disabled_black);

            } else if (header.equals("CHANGE_PASSWORD_STATUS")) {
                if (body.equals("1")) {
                    isAuthenticate = STATUS_OFF;
                    modPasswordBttn.setBackgroundResource(R.drawable.input_black);
                    setDefaultPassword();
                    displayToast("password changed!");
                } else {
                    isAuthenticate = STATUS_OFF;
                    modPasswordBttn.setBackgroundResource(R.drawable.input_black);
                    displayToast("Wrong password entered");
                }

            } else {
                Log.d("WRONG_HEADER",header);
                displayToast("WRONG_HEADER: ["+header+"]");
            }

            //Log.d("THREAD MESSAGE HEAD", header);
            //Log.d("THREAD MESSAGE BODY", Integer.toString((int) body.charAt(0)));
        }
    };

    private View.OnClickListener lockBttnClick = new View.OnClickListener() {
        public void onClick(View v) {
            //displayToast("CLICKING");
            password = editUnlock.getText().toString();

            if (isLocked == STATUS_ON) {
                isLocked = STATUS_TURNING_OFF;
                Log.d("PASSWORDU", password);
                lockBttn.setBackgroundResource(R.drawable.lock_closed_blue);
                btThread.sendPacket("UNLOCK", password);

            } else if (isLocked == STATUS_TURNING_ON) {
                displayToast("turning on...");

            } else if (isLocked == STATUS_TURNING_OFF) {
                displayToast("turning off...");

            } else if (isLocked == STATUS_OFF) {
                isLocked = STATUS_TURNING_ON;
                String password = editUnlock.getText().toString();
                Log.d("PASSWORDL",password);
                lockBttn.setBackgroundResource(R.drawable.lock_open_blue);
                btThread.sendPacket("LOCK",password);

            } else {
                displayToast("WROMG LOCKED STATUS: "+Integer.toString(isLocked));
            }
        }
    };

    private void loadPreferences() {
        Log.d("DEF-KEY-R", activeDevice.getAddress());
        String defpass = sharedPref.getString(activeDevice.getAddress(), "");

        if (!defpass.equals("")) {
            defaultPassword = defpass;
            Log.d("DEF-PASSREAD", defpass);
        } else {
            Log.d("DEF-PASSREAD", "<NOPE>");
        }
    }

    private void setDefaultPassword() {
        Log.d("SET DEFAULT PASSWORD", password);
        SharedPreferences.Editor editor = sharedPref.edit();
        editor.clear();
        Log.d("DEF-KEY-S", activeDevice.getAddress());
        editor.putString(activeDevice.getAddress(), password);
        editor.apply();

        loadPreferences();
    }

    private void displayToast(String data) {
        Toast.makeText(getApplicationContext(), data, Toast.LENGTH_SHORT).show();
    }

    void unlockLockBttn() {
        isLocked = STATUS_OFF;
        lockBttn.setBackgroundResource(R.drawable.lock_open_black);
    }
    
    void lockLockBttn() {
        isLocked = STATUS_ON;
        lockBttn.setBackgroundResource(R.drawable.lock_closed_black);
    }

    void revertLockStatus() {
        if (isLocked == STATUS_TURNING_OFF) {
            isLocked = STATUS_ON;
            lockBttn.setBackgroundResource(R.drawable.lock_open_black);

        } else if (isLocked == STATUS_TURNING_ON) {
            isLocked = STATUS_OFF;
            lockBttn.setBackgroundResource(R.drawable.lock_closed_black);

        } else {
            Log.e("REVERT STATUS FAIL", Integer.toString(isLocked));
        }
    }

    void changePassword(String nowPassword, String newPassword) {
        while (nowPassword.length() < 8) {
            nowPassword += "\0";
        }
        while (newPassword.length() < 8) {
            newPassword += "\0";
        }
        btThread.sendPacket("CHANGE_PASSWORD",nowPassword+newPassword);
    }

    @Override
    protected void onPause() {
        btThread.cancel();
        Log.d("THREAD_SOCK_CANCEL","YES");
        super.onPause();
        super.onDestroy();
    }
}
