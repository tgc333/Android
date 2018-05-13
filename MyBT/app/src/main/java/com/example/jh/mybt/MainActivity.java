package com.example.jh.mybt;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.InputStream;
import java.io.OutputStream;
import java.text.SimpleDateFormat;
import java.util.Date;


public class MainActivity extends AppCompatActivity {

    private BluetoothAdapter btAdapter;
    private BluetoothService btService = null;
    private BluetoothDevice btDevice = null;
    private ImageView img = null;
    BluetoothSocket mmSocket = null;
    OutputStream mOutputStream = null;
    InputStream mInputStream = null;

    private  static final String TAG = "MAIN";
    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;

    private Button btn_Connect;
    private Button btn_red;
    private Button btn_blue;
    private Button btn_green;
    private Button btn_white;
    private Button btn_yellow;
    private Button btn_off;
    private Button btn_on;
    private Button btn_reverse;
    private TextView xtext;
    private TextView ytext;
    private TextView ztext;
    private TextView pitchtext;
    private TextView rolltext;
    private TextView yawtext;
    private TextView txt_Result;
    private TextView time;

    private SensorManager mSensorManager = null;
    private SensorEventListener sel;
    private Sensor Gyro = null;

    private double xpos, ypos, zpos, timestamp, dt;
    private double pitch, roll, yaw;
    private double RAD2DGR = 180 / Math.PI;
    private static final float NS2S = 1.0f/1000000000.0f;

    private long curTime = System.currentTimeMillis();


    Thread t = new Thread() {

        @Override
        public void run() {
            try {
                while (!isInterrupted()) {
                    Thread.sleep(10);
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            updateYOURthing();
                        }
                    });
                }
            } catch (InterruptedException e) {
            }
        }
    };
    private void updateYOURthing() {
        long now = System.currentTimeMillis();
        now = System.currentTimeMillis();
        Date date = new Date(now);
        SimpleDateFormat sdfNow = new SimpleDateFormat("yyyy/MM/dd HH:mm:ss.SSS");
        String strNow = sdfNow.format(date);
        time.setText(strNow.toString());
    }

    private final Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
        }
    };

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mSensorManager = (SensorManager)getSystemService(Context.SENSOR_SERVICE);
        Gyro = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);

        class GyroscopeListener implements SensorEventListener{
            @Override
            public void onSensorChanged(SensorEvent sensorEvent) {
                double gyroX = sensorEvent.values[0];
                double gyroY = sensorEvent.values[1];
                double gyroZ = sensorEvent.values[2];

                dt = (sensorEvent.timestamp - timestamp) * NS2S;
                timestamp = sensorEvent.timestamp;

                if(dt - timestamp*NS2S != 0){

                    roll = roll + gyroX*dt;
                    pitch = pitch + gyroY*dt;
                    yaw = yaw + gyroZ*dt;

                    xtext.setText("   X : "+ String.format("%.4f", sensorEvent.values[0]*100));
                    ytext.setText("   Y : "+ String.format("%.4f", sensorEvent.values[1]*100));
                    ztext.setText("   Z : "+ String.format("%.4f", sensorEvent.values[2]*100));

                    rolltext.setText("   Roll : " + String.format("%.1f", roll*RAD2DGR));
                    pitchtext.setText("   Pitch : " + String.format("%.1f", pitch*RAD2DGR));
                    yawtext.setText("   Yaw : " + String.format("%.1f", yaw*RAD2DGR));

                    String str = time.getText().toString() + "\n" +
                    xtext.getText().toString() + ytext.getText().toString()
                            + ztext.getText().toString()+"\n";

                    byte[] buffer2 = new byte[60];
                    buffer2 = str.getBytes();
                    System.out.println(time.getText().toString());
                    btService.write(buffer2);
                    str = rolltext.getText().toString() + pitchtext.getText().toString()
                            + yawtext.getText().toString() + "\n";
                    buffer2 = str.getBytes();
                    btService.write(buffer2);

                    if(yaw*RAD2DGR > 100) {
                        byte[] buffer1 = new byte[2];
                        buffer1[0] = 'Q';
                        img.setImageResource(R.drawable.red);
                        btService.write(buffer1);
                    }else if(yaw*RAD2DGR < -100){
                        byte[] buffer1 = new byte[2];
                        buffer1[0] = 'J';
                        img.setImageResource(R.drawable.red);
                        btService.write(buffer1);
                    }else{
                        byte[] buffer1 = new byte[2];
                        buffer1[0] = 'U';
                        btService.write(buffer1);
                    }
                }
            }

            @Override
            public void onAccuracyChanged(Sensor sensor, int i) {

            }
        }

        sel = new GyroscopeListener();
        mSensorManager.registerListener(sel, Gyro, SensorManager.SENSOR_DELAY_FASTEST);

        getSupportActionBar().setDisplayShowHomeEnabled(true);

        //getSupportActionBar().setIcon(R.mipmap.ic_launcher_aaa);

        btn_Connect = (Button)findViewById(R.id.btn_connect);
        btn_red = (Button)findViewById(R.id.btn_red);
        btn_blue = (Button)findViewById(R.id.btn_bule);
        btn_green = (Button)findViewById(R.id.btn_green);
        btn_yellow = (Button)findViewById(R.id.btn_yellow);
        btn_white = (Button)findViewById(R.id.btn_white);
        btn_off = (Button)findViewById(R.id.btn_off);
        btn_on = (Button)findViewById(R.id.btn_on);
        btn_reverse = (Button)findViewById(R.id.btn_reverse);
        img = (ImageView)findViewById(R.id.img);

        xtext = (TextView)findViewById(R.id.text1);
        ytext = (TextView)findViewById(R.id.text2);
        ztext = (TextView)findViewById(R.id.text3);
        rolltext = (TextView)findViewById(R.id.text4);
        pitchtext = (TextView)findViewById(R.id.text5);
        yawtext = (TextView)findViewById(R.id.text6);
        time = (TextView)findViewById(R.id.time);

        btn_Connect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(btService.getDeviceState())
                    btService.enableBluetooth();
                else
                    finish();
            }
        });

        if(btService == null){
            btService = new BluetoothService(this, mHandler);
        }

        t.start();

    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d(TAG, "onActivityResult" + resultCode);

        switch (requestCode){
            case REQUEST_CONNECT_DEVICE:
                if(resultCode == Activity.RESULT_OK) {
                    btService.getDeviceInfo(data);
                }
                break;

            case REQUEST_ENABLE_BT:
                if(resultCode == Activity.RESULT_OK){
                    btService.scanDevice();
                }else {
                    Log.d(TAG, "Bluetooth is not enabled");
                }
                break;
        }
    }
}
