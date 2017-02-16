package com.milselarch.mappapp;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.location.Location;
import android.net.Uri;
import android.support.v4.app.FragmentActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.OrientationEventListener;

import com.google.android.gms.appindexing.Action;
import com.google.android.gms.appindexing.AppIndex;
import com.google.android.gms.appindexing.Thing;
import com.google.android.gms.common.ConnectionResult;
import com.google.android.gms.common.GooglePlayServicesUtil;
import com.google.android.gms.common.api.GoogleApiClient;
import com.google.android.gms.location.LocationListener;
import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;

import static com.milselarch.mappapp.R.id.location;
import java.lang.Float;

public class RestaurantMap extends FragmentActivity implements OnMapReadyCallback {
    private GoogleMap mMap;

    private double lat;
    private double lon;
    private String restaurantName;
    private double myLat;
    private double myLon;
    private LatLng RESTAURANT;
    private LatLng ME;
    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    private GoogleApiClient client;

    private SensorManager sensorManager;
    private final int sensorType =  Sensor.TYPE_ROTATION_VECTOR;
    float[] rotMat = new float[9];
    float[] vals = new float[3];

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_restaurant_map);

        lat = getIntent().getDoubleExtra("LATITUDE", 0);
        lon = getIntent().getDoubleExtra("LONGITUDE", 0);
        restaurantName = getIntent().getStringExtra("NAME");
        myLat = getIntent().getDoubleExtra("MYLATITUDE", 0);
        myLon = getIntent().getDoubleExtra("MYLONGITUDE", 0);

        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client = new GoogleApiClient.Builder(this).addApi(AppIndex.API).build();
        //myOrientationEventListener = new OrientationEventListener(this, SensorManager.SENSOR_DELAY_NORMAL)

        sensorManager = (SensorManager) this.getSystemService(SENSOR_SERVICE);
        Sensor accel = sensorManager.getDefaultSensor(Sensor.TYPE_ROTATION_VECTOR);
        sensorManager.registerListener(listen, accel, SensorManager.SENSOR_DELAY_NORMAL);
    }

    public SensorEventListener listen = new SensorEventListener() {
        @Override
        public void onAccuracyChanged (Sensor sensor, int accuracy) {
            //super.onAccuracyChanged(sensor, accuracy);
        }

        @Override
        public void onSensorChanged(SensorEvent event) {
            //sensorHasChanged = false;
            SensorManager.getRotationMatrixFromVector(rotMat,
                    event.values);
            SensorManager
                    .remapCoordinateSystem(rotMat,
                            SensorManager.AXIS_X, SensorManager.AXIS_Y,
                            rotMat);

            SensorManager.getOrientation(rotMat, vals);
            float azimuth = (float) Math.toDegrees(vals[0]); // in degrees [-180, +180]
            float pitch = (float) Math.toDegrees(vals[1]);
            float roll = (float) Math.toDegrees(vals[2]);

            CameraPosition cameraPosition = new CameraPosition.Builder()
                    .target(RESTAURANT)             // Sets the center of the map to current location
                    .zoom(15)                       // Sets the zoom
                    .bearing(azimuth)               // Sets the orientation of the camera to east
                    .tilt(0)                        // Sets the tilt of the camera to 0 degrees
                    .build();                       // Creates a CameraPosition from the builder

            mMap.moveCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
            //Log.d("AZIMUTH", String.valueOf(azimuth));
            //sensorHasChanged = true;
        }
    };

    /*
    sensorHasChanged = false;
    if (event.sensor.getType() == sensorType){
        SensorManager.getRotationMatrixFromVector(rotMat,
                event.values);
        SensorManager
                .remapCoordinateSystem(rotMat,
                        SensorManager.AXIS_X, SensorManager.AXIS_Y,
                        rotMat);
        SensorManager.getOrientation(rotMat, vals);
        azimuth = deg(vals[0]); // in degrees [-180, +180]
        pitch = deg(vals[1]);
        roll = deg(vals[2]);
        sensorHasChanged = true;
    }
    */

    /**
     * Manipulates the map once available.
     * This callback is triggered when the map is ready to be used.
     * This is where we can add markers or lines, add listeners or move the camera. In this case,
     * we just add a marker near Sydney, Australia.
     * If Google Play services is not installed on the device, the user will be prompted to install
     * it inside the SupportMapFragment. This method will only be triggered once the user has
     * installed Google Play services and returned to the app.
     */
    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;

        // Add a marker in Sydney and move the camera
        RESTAURANT = new LatLng(lat, lon);
        ME = new LatLng(myLat, myLon);

        Marker restaurant = mMap.addMarker(new MarkerOptions().position(RESTAURANT).title(restaurantName));
        Marker me = mMap.addMarker(
                new MarkerOptions().position(ME).title("ME")
                        .snippet("My location")
                        .icon(BitmapDescriptorFactory.fromResource(
                                R.drawable.marker_me
                        ))
        );

        // Move the camera instantly to restaurant with a zoom of
        mMap.moveCamera(CameraUpdateFactory.newLatLngZoom(RESTAURANT, 15));
        mMap.getUiSettings().setMyLocationButtonEnabled(true);
        mMap.getUiSettings().setCompassEnabled(true);
    }

    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    public Action getIndexApiAction() {
        Thing object = new Thing.Builder()
                .setName("RestaurantMap Page") // TODO: Define a title for the content shown.
                // TODO: Make sure this auto-generated URL is correct.
                .setUrl(Uri.parse("http://[ENTER-YOUR-URL-HERE]"))
                .build();
        return new Action.Builder(Action.TYPE_VIEW)
                .setObject(object)
                .setActionStatus(Action.STATUS_TYPE_COMPLETED)
                .build();
    }

    /*
    @Override
    public void onLocationChanged(Location location) {
        CameraPosition cameraPosition = new CameraPosition.Builder()
            .target(RESTAURANT)             // Sets the center of the map to current location
            .zoom(15)                       // Sets the zoom
            .bearing(location.getBearing()) // Sets the orientation of the camera to east
            .tilt(0)                        // Sets the tilt of the camera to 0 degrees
            .build();                       // Creates a CameraPosition from the builder

        mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
    }
    */

    @Override
    public void onStart() {
        super.onStart();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client.connect();
        AppIndex.AppIndexApi.start(client, getIndexApiAction());
    }

    @Override
    public void onStop() {
        super.onStop();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        AppIndex.AppIndexApi.end(client, getIndexApiAction());
        client.disconnect();
    }
}
