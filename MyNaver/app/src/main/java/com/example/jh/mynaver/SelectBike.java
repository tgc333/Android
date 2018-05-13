package com.example.jh.mynaver;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.support.annotation.LayoutRes;
import android.support.annotation.NonNull;
import android.support.annotation.Nullable;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;

import java.util.ArrayList;

public class SelectBike extends Activity {

    protected ListView mList = null;
    protected ArrayList<character> mArray = new ArrayList<character>();
    protected characterAdapter mAdapter = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.list);

        mArray.add(new character("오토바이 1", "100CC", 10, 20, 1));
        mArray.add(new character("오토바이 2", "50CC", 20, 10, 1));
        mArray.add(new character("오토바이 3", "50CC", 15, 15, 1));
        mAdapter = new characterAdapter(this, R.layout.myitem);
        mList = (ListView)findViewById(R.id.list);
        mList.setAdapter(mAdapter);

        mList.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> adapterView, View view, int i, long l) {
                Intent intent = new Intent(SelectBike.this, MoreInfo.class);
                intent.putExtra("attack", Integer.toString(mArray.get(i).getatk()));
                intent.putExtra("defense", Integer.toString(mArray.get(i).getdef()));
                intent.putExtra("level", Integer.toString(mArray.get(i).getlevel()));
                intent.putExtra("skill", mArray.get(i).getSkill());
                intent.putExtra("name", mArray.get(i).getName());

                startActivity(intent);
            }
        });

    }

    public class character {
        String name;
        String skill;
        int atk;
        int def;
        int level;

        public character(String name, String skill, int atk, int def, int level) {
            this.name = name;
            this.skill = skill;
            this.atk = atk;
            this.def = def;
            this.level = level;
        }

        public String getTitle() {
            return name;
        }
        public String getSinger() {
            return skill;
        }
        public String getName(){return name;}
        public String getSkill(){return skill;}
        public int getatk(){return atk;}
        public int getdef(){return def;}
        public int getlevel(){return level;}
    }

    static class characterViewHolder{
        TextView name;
        TextView skill;
        TextView atk;
        TextView def;
        TextView level;
        ImageView image;
    }

    public class characterAdapter extends ArrayAdapter<character>{
        protected LayoutInflater mInflater = null;
        public characterAdapter(@NonNull Context context, @LayoutRes int resource) {
            super(context, resource);
            mInflater = LayoutInflater.from(context);
        }

        @Override
        public int getCount() {
            return mArray.size();
        }

        @NonNull
        @Override
        public View getView(int position, @Nullable View convertView, @NonNull ViewGroup parent) {
            characterViewHolder vh = null;
            if(convertView==null) {
                convertView = mInflater.inflate(R.layout.myitem, parent, false);
                vh = new characterViewHolder();
                vh.name = (TextView)convertView.findViewById(R.id.name);
                vh.skill = (TextView)convertView.findViewById(R.id.skill);
                vh.atk = (TextView)convertView.findViewById(R.id.atk);
                vh.def = (TextView)convertView.findViewById(R.id.def);
                vh.level = (TextView)convertView.findViewById(R.id.level);
                convertView.setTag(vh);
            }
            else{
                vh = (characterViewHolder) convertView.getTag();
            }
            vh.name.setText(mArray.get(position).getTitle());
            vh.level.setText("번호 : "+mArray.get(position).getlevel());
            vh.skill.setText("파워 : "+mArray.get(position).getSinger());
            vh.atk.setText("가격 : "+ mArray.get(position).getatk());
            vh.def.setText("대여정보 : "+mArray.get(position).getdef());

            return convertView;
        }
    }
}
