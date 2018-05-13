package com.example.jh.mynaver;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.ListView;
import android.widget.TextView;

public class MoreInfo extends Activity {

    private BluetoothAdapter btAdapter;
    private BluetoothService btService = null;
    private BluetoothDevice btDevice = null;
    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;
    private final Handler mHandler = new Handler(){
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.moreinfo);

        Intent intent = getIntent();
        String attack = intent.getStringExtra("attack");
        String defense = intent.getStringExtra("defense");
        String level = intent.getStringExtra("level");
        ;
        String skill = intent.getStringExtra("skill");
        ;

        TextView attackT = (TextView) findViewById(R.id.attack);
        TextView defenseT = (TextView) findViewById(R.id.defense);
        TextView levelT = (TextView) findViewById(R.id.level);
        TextView skillT = (TextView) findViewById(R.id.skill);

        levelT.setText("번호 : " + level);
        attackT.setText("가격 : " + attack);
        defenseT.setText("대여정보 : " + defense);
        skillT.setText("파워 : " + skill);

        Button btn_Connect = (Button) findViewById(R.id.btnshare);
        btn_Connect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (btService.getDeviceState())
                    btService.enableBluetooth();
                else
                    finish();
            }
        });

        if (btService == null) {
            btService = new BluetoothService(this, mHandler);
        }
    }
    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        Log.d("Bluetooth", "onActivityResult" + resultCode);

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
                    Log.d("Bluetooth", "Bluetooth is not enabled");
                }
                break;
        }
    }
}
