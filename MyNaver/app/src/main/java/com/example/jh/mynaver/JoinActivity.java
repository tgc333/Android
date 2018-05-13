package com.example.jh.mynaver;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
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

import java.net.MalformedURLException;

public class JoinActivity extends Activity {
    private Button join, overlap;
    private EditText e_id, e_password1, e_password2;
    private String id, password1, password2;
    protected JSONObject mResult = null;
    protected RequestQueue mQueue = null;
    boolean flag = false;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.join);
        mQueue = Volley.newRequestQueue(this);
        NetworkUtil.setNetworkPolicy();
        join = (Button)findViewById(R.id.join);
        overlap = (Button)findViewById(R.id.overlap);
        e_id = (EditText)findViewById(R.id.id);
        e_password1=(EditText)findViewById(R.id.password1);
        e_password2=(EditText)findViewById(R.id.password2);

        overlap.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                id = e_id.getText().toString();
                password1 = e_password1.getText().toString();
                requestinfo();
                if(flag) {
                    Toast.makeText(getApplication(), "아이디 사용 가능", Toast.LENGTH_SHORT).show();
                }
                else {
                    Toast.makeText(getApplication(), "중복된 아이디가 존재합니다.", Toast.LENGTH_SHORT).show();
                }
            }
        });

        join.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(flag) {
                    flag=false;
                    try {
                        PHPRequest request = new PHPRequest("http://202.31.201.139/join.php");
                        id = e_id.getText().toString();
                        password1 = e_password1.getText().toString();
                        password2 = e_password2.getText().toString();
                        String result = request.PhPtest(id, password1);

                        if (password1.equals(password2)) {
                            Intent intent = new Intent(getApplicationContext(), MainActivity.class); //메인화면으로 이동
                            startActivity(intent);
                            Toast.makeText(getApplication(), "회원가입 완료", Toast.LENGTH_SHORT).show();
                        } else {
                            Toast.makeText(getApplication(), "비밀번호가 일치하지 않습니다.", Toast.LENGTH_SHORT).show();
                        }


//                    if(result.equals("1")){
//                        Toast.makeText(getApplication(),"들어감",Toast.LENGTH_SHORT).show();
//                    }
//                    else{
//                        Toast.makeText(getApplication(),"안 들어감", Toast.LENGTH_SHORT).show();
//                    }
                    } catch (MalformedURLException e) {
                        e.printStackTrace();
                    }
                }
                else {
                    Toast.makeText(getApplication(), "아이디 중복 확인 ㄱㄱ", Toast.LENGTH_SHORT).show();
                }
            }
        });

    }



    protected void info() {
        try {
            JSONArray list = mResult.getJSONArray("list");
            for (int i = 0; i < list.length(); i++) {
                JSONObject node = list.getJSONObject(i);

                String d_email = node.getString("email");

                if(d_email.equals(id)) {
                    flag = false;
                    return;
                }

                if(i== list.length() - 1) {
                    flag = true;
                }
            }
        } catch (JSONException | NullPointerException e) {
            e.printStackTrace();
            Toast.makeText(this, "Error: " + e.toString(),
                    Toast.LENGTH_LONG).show();
            Toast.makeText(JoinActivity.this, "여긴가", Toast.LENGTH_SHORT).show();
            mResult = null;
        }
    }

    protected void requestinfo() {
        String url = "http://202.31.201.139/user.php";
        JsonObjectRequest request = new JsonObjectRequest(Request.Method.GET, url, null,
                new Response.Listener<JSONObject>() {
                    @Override
                    public void onResponse(JSONObject response) {
                        //Toast.makeText(LoginActivity.this, "디비 연결 성공", Toast.LENGTH_SHORT).show();
                        mResult = response;

                        info();
                    }
                },
                new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        Toast.makeText(JoinActivity.this, "Error : " + error.toString(), Toast.LENGTH_SHORT).show();
                    }
                }
        );
        mQueue.add(request);
    }


}
