package com.milselarch.mappapp;

import android.media.MediaPlayer;
import android.support.v7.app.AppCompatActivity;

/**
 * Created by lenovo on 13/1/2017.
 */

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Window;
import android.view.WindowManager;

public class SplashActivity extends AppCompatActivity {
    // Splash screen timer
    private MediaPlayer mediaPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //Remove title bar
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        //Remove notification bar
        this.getWindow().setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN,
            WindowManager.LayoutParams.FLAG_FULLSCREEN
        );

        setContentView(R.layout.activity_splash_screen);
        //getSupportActionBar().setDisplayShowTitleEnabled(false);
        mediaPlayer = MediaPlayer.create(getApplicationContext(), R.raw.splash_music);
        mediaPlayer.start();

        mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                Intent i = new Intent(SplashActivity.this, MainActivity.class);
                finish();

                mediaPlayer.stop();
                mediaPlayer.release();

                // close this activity
                Log.d("SPLASH", "START ACTIVITY");
                startActivity(i);
            }
        });
    }
}