package com.example.jh.mycap;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

public class BluetoothService extends AppCompatActivity {

    byte[] RcvBuffer = new byte[1024];
    int pRcvBuffer;

    private static final String TAG = "BluetoothService";
    private static final int REQUEST_CONNECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;

    private static final UUID MY_UUID =
            UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private BluetoothAdapter btAdapter;

    private ConnectThread mConnectThread;
    private ConnectedThread mConnectedThread;

    private Activity mActivity;
    private Handler mHandler;

    private int mState;

    private static final int STATE_NONE = 0; // we're doing nothing
    private static final int STATE_LISTEN = 1; // now listening for incoming connections
    private static final int STATE_CONNECTING = 2; // now initiating an outgoing connection
    private static final int STATE_CONNECTED = 3; // now connected to a remote device

    private  synchronized void setState(int state){
        Log.d(TAG, "setState() " + mState + " -> " + state);
        mState = state;
    }

    public synchronized int getState() {
        return mState;
    }

    public synchronized void start() {
        Log.d(TAG, "start");
        if (mConnectThread == null) {

        } else {
            mConnectThread.cancel();
            mConnectThread = null;
        } // Cancel any thread currently running a connection
        if (mConnectedThread == null) {

        } else {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }
    }

    public synchronized void connect(BluetoothDevice device) {
        Log.d(TAG, "connect to: " + device);
        if (mState == STATE_CONNECTING) {
            if (mConnectThread == null) {

            } else {
                mConnectThread.cancel();
                mConnectThread = null;
            }
        }
        if (mConnectedThread == null) {

        } else {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }

        mConnectThread = new ConnectThread(device);
        mConnectThread.start();
        setState(STATE_CONNECTING);
    }


    public synchronized void connected(BluetoothSocket socket, BluetoothDevice device) {
        Log.d(TAG, "connected");
        // Cancel the thread that completed the connection
        if (mConnectThread == null) {

        } else {
            mConnectThread.cancel();
            mConnectThread = null;
        } // Cancel any thread currently running a connection
        if (mConnectedThread == null) {

        } else {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }

        mConnectedThread = new ConnectedThread(socket);
        mConnectedThread.start();
        setState(STATE_CONNECTED);
    }

    public synchronized void stop() {
        Log.d(TAG, "stop");
        if (mConnectThread != null) {
            mConnectThread.cancel();
            mConnectThread = null;
        }
        if (mConnectedThread != null) {
            mConnectedThread.cancel();
            mConnectedThread = null;
        }
        setState(STATE_NONE);
    }

    public void write(byte[] out){
        ConnectedThread r;
        synchronized (this){
            if(mState != STATE_CONNECTED) {
               // Toast.makeText(mActivity, "write failed", Toast.LENGTH_SHORT).show();
                return;
            }
            r = mConnectedThread;
            r.write(out);
        }
    }

    private void connectionFailed(){
        setState(STATE_LISTEN);
    }

    private void connectionLost(){
        setState(STATE_LISTEN);
    }


    private class ConnectThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final BluetoothDevice mmDevice;

        public ConnectThread(BluetoothDevice device){
            mmDevice = device;
            BluetoothSocket tmp = null;

            try{
                tmp = device.createRfcommSocketToServiceRecord(MY_UUID);
            }catch(IOException e){
                Log.e(TAG, "create() failed", e);
            }
            mmSocket = tmp;
        }

        public void run(){
            Log.i(TAG, "BEGIN mConnectThread");
            setName("ConnectThread");
            Log.i(TAG, "1");
            btAdapter.cancelDiscovery();
            Log.i(TAG, "2");

            try{
                mmSocket.connect();
                Log.d(TAG, "Connect Succes");
            }catch(IOException e){
                connectionFailed();
                Log.d(TAG,"Connect fail" + e);

                try{
                    mmSocket.close();
                }catch (IOException e2){
                    Log.e(TAG, "unable to close() socket during connection failure", e2);
                }
                BluetoothService.this.start();
                return;
            }
            synchronized (BluetoothService.this){
                mConnectThread = null;
            }
            connected(mmSocket, mmDevice);
        }
        public void cancel(){
            try{
                mmSocket.close();
            }catch (IOException e){
                Log.e(TAG, "close() of connect socket failed",e);
            }
        }
    }

    private class ConnectedThread extends Thread {
        private final BluetoothSocket mmSocket;
        private final InputStream mmInStream;
        private final OutputStream mmOutStream;

        public ConnectedThread(BluetoothSocket socket){
            Log.d(TAG, "create ConnectedThread");
            mmSocket = socket;
            InputStream tmpIn = null;
            OutputStream tmpOut = null;

            try{
                tmpIn = socket.getInputStream();
                tmpOut = socket.getOutputStream();
            }catch (IOException e){
                Log.e(TAG, "temp sockets not created", e);
            }
            mmInStream = tmpIn;
            mmOutStream = tmpOut;
        }

        public void run(){
            Log.i(TAG, "BEGIN mConnectedThread");
            byte[] buffer = new byte[1024];
            int i;
            byte chr;
            int bytes;

            while(true){
                try{
                    bytes = mmInStream.read(buffer);
                    i = 0;
                    while(i < bytes) {
                        chr = buffer[i++];
                        if (chr == '*') pRcvBuffer = 0;
                        RcvBuffer[pRcvBuffer++] = chr;
                        if (chr == 0x0a) {
                            pRcvBuffer = 0;
                        }
                    }
                }catch (IOException e){
                    Log.e(TAG, "disconnected",e);
                    connectionLost();
                    break;
                }
            }
        }

        public void write(byte[] buffer){
            try{
                mmOutStream.write(buffer);
            }catch (IOException e){
                Log.e(TAG, "Exception during write",e);
            }
        }

        public void cancel(){
            try{
                mmSocket.close();
            }catch (IOException e){
                Log.e(TAG, "close() of connect socket failed",e);
            }
        }

    }

    public BluetoothService(Activity ac, Handler h){
        mActivity = ac;
        mHandler = h;

        btAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    public boolean getDeviceState(){
        Log.i(TAG, "Check the Bluetooth support");

        if(btAdapter == null){
            Log.d(TAG, "Bluetooth is not available");
            return false;
        }else{
            Log.d(TAG, "Bluetooth is available");
            return true;
        }
    }

    public void enableBluetooth(){
        Log.i(TAG, "Check the enable Bluetooth");

        if(btAdapter.isEnabled())
            Log.d(TAG, "BlueTooth Enable Now");
        else{
            Log.d(TAG, "BlueTooth Enable Request");

            Intent i = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            mActivity.startActivityForResult(i, REQUEST_ENABLE_BT);
        }
    }

    public void scanDevice(){
        Log.d(TAG, "Scan Device");

        Intent serverIntent = new Intent(mActivity, DeviceListActivity.class);
        mActivity.startActivityForResult(serverIntent, REQUEST_CONNECT_DEVICE);
    }

    public void getDeviceInfo(Intent data){
        String address = data.getExtras().getString(DeviceListActivity.EXTRA_DEVICE_ADDRESS);
        BluetoothDevice device = btAdapter.getRemoteDevice(address);

        Log.d(TAG, "Get Device Info\n" + "address : " + address);

        connect(device);
    }
}
