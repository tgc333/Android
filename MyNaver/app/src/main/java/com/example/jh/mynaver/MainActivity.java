package com.example.jh.mynaver;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.annotation.Nullable;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class MainActivity extends Activity {
    protected JSONObject mResult = null;
    protected RequestQueue mQueue = null;

    private  static final String TAG = "MAIN";

    private Button db;
    private Button login, join;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mQueue = Volley.newRequestQueue(this);
        mQueue.start();

        db = (Button)findViewById(R.id.db_connect);
        login = (Button)findViewById(R.id.login);
        join = (Button)findViewById(R.id.join);

        db.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                requestinfo();
            }
        });

        join.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent =new Intent(getApplicationContext(),JoinActivity.class);
                startActivity(intent);
            }
        });

        login.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent =new Intent(getApplicationContext(),LoginActivity.class);
                startActivity(intent);
            }
        });
    }
    protected void requestinfo() {
        String url = "http://202.31.201.139/bike.php";
        JsonObjectRequest request = new JsonObjectRequest(Request.Method.GET, url, null,
                new Response.Listener<JSONObject>() {
                    @Override
                    public void onResponse(JSONObject response) {
                        Toast.makeText(MainActivity.this, "디비 연결 성공", Toast.LENGTH_SHORT).show();
                        mResult = response;
                        info();
                    }
                },
                new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Toast.makeText(MainActivity.this, "Error : " + error.toString(), Toast.LENGTH_SHORT).show();
                    }
                }
        );
        mQueue.add(request);
    }

    protected void info() {
        try {
            JSONArray list = mResult.getJSONArray("list");
            for (int i = 0; i < list.length(); i++) {
                JSONObject node = list.getJSONObject(i);
                int number = node.getInt("number");
                String name = node.getString("name");
                String address = node.getString("address");
                Toast.makeText(MainActivity.this, ""+number+","+name+","+address, Toast.LENGTH_SHORT).show();
            }
        } catch (JSONException | NullPointerException e) {
            e.printStackTrace();
            Toast.makeText(this, "Error: " + e.toString(),
                    Toast.LENGTH_LONG).show();
            Toast.makeText(MainActivity.this, "여긴가", Toast.LENGTH_SHORT).show();
            mResult = null;
        }

    }
}
