package com.example.jh.mywork;

import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

public class Menu2 extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.menu2);

        Button ok = (Button)findViewById(R.id.ok);

        ok.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Toast.makeText(Menu2.this, "신청 완료", Toast.LENGTH_SHORT).show();
                Intent i = new Intent(Menu2.this, Menu6.class);
                startActivity(i);
            }
        });

        Spinner box1 = (Spinner)findViewById(R.id.spinner);
        ArrayAdapter boxAdapter = ArrayAdapter.createFromResource(this,
                R.array.box, android.R.layout.simple_spinner_dropdown_item);

        box1.setAdapter(boxAdapter);
        box1.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, int i, long l) {
                ((TextView)adapterView.getChildAt(0)).setTextColor(Color.BLACK);
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {

            }
        });
        boxAdapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);



    }



}
