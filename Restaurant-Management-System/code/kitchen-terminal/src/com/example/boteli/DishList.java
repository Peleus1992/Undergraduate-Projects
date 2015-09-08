package com.example.boteli;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.example.boteli.MainActivity.MyAdapter;
import com.example.boteli.SlipButton.OnChangedListener;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.ListActivity;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.Cursor;
import android.os.Bundle;
import android.os.Message;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

public class DishList extends Activity{
	MyDB db = null;
	private List<Map<String, Object>> mData;
	private MyAdapter adapter;
	private String sFlag = "0";
	class BackBTlistener implements OnClickListener
	{
		
		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
			Intent intent = new Intent();
			intent.putExtra("dish_stuff_flag", sFlag);
			setResult(RESULT_OK, intent);
			BackMakeSure(0);
		}
	}
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.dish_list);
		Button backButton = (Button)findViewById(R.id.backbutton);
		backButton.setOnClickListener(new BackBTlistener());
		ListView listext = (ListView)findViewById(R.id.list_ext);
		mData = getData();
	    adapter = new MyAdapter(this);
		listext.setAdapter(adapter);
		adapter.notifyDataSetChanged();
	}
	
//	从数据库中获得数据，并更新列表
	private List<Map<String, Object>> getData() {
		List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();
		Map<String, Object> map = null;
		db = new MyDB(this);
		db.open();
		Cursor c = db.getAllTitles();
		
		if (c.moveToFirst())
		{
			do 
			{
				map = new HashMap<String, Object>();
				map.put("dish_id", c.getString(0));
				map.put("dish_name", c.getString(1));
				if(c.getString(2).equals("1"))
					map.put("dish_state", getResources().getString(R.string.dish_full));
				else if(c.getString(2).equals("0"))
					map.put("dish_state", getResources().getString(R.string.dish_empty));
				else
				{
					map.put("dish_state", "error");
				}
				list.add(map);
			} 
			while (c.moveToNext());
		}
		db.close();
		return list;
	}
	
// 显示菜的更新信息
	public void DisplayTitle(Cursor c)
    {
		String sTemp = null;
		if(c.getString(2).equals("0"))
			sTemp = "不足";
		else
			sTemp = "充足";
    	Toast.makeText(this,"菜名: " + 
    			c.getString(1) + "\n" +"素材: " + sTemp, Toast.LENGTH_SHORT).show();
    }

// 若菜的列表更新，同时更新数据库
	void ChangeDB(String sID, int iFlag)
	{
		db = new MyDB(this);
		db.open();
		Cursor c = db.getTitle(sID);
		if(iFlag == 1)
			db.updateTitle(c.getString(0), c.getString(1), "1");
		else
			db.updateTitle(c.getString(0), c.getString(1), "0");
		c = db.getTitle(sID);
		if (c.moveToFirst())
		{
			do 
			{
				DisplayTitle(c);
			} 
			while (c.moveToNext());
		}
		db.close();
	}

// 当点击列表项，触发更新事件
	class ItemClickEvent implements OnItemClickListener 
	 {

		@Override
		public void onItemClick(AdapterView<?> arg0, View v, int position,
				long arg3) {
			// TODO Auto-generated method stub
			String sID = (String) mData.get(position).get("dish_id");
			String sState = (String) mData.get(position).get("dish_state");
			int iFlag = 0;
			if(sState.equals(getResources().getString(R.string.dish_empty)))
				iFlag = 1;
			else
				iFlag = 0;
			ChangeDB(sID, iFlag);
			if(iFlag == 1)
				mData.get(position).put("dish_state", getResources().getString(R.string.dish_full));
			else
				mData.get(position).put("dish_state", getResources().getString(R.string.dish_empty));
			adapter.notifyDataSetChanged();
			sFlag = "1";
		}
	}
	
// 通过查数据库查询现在的不足的菜
	Cursor GetEmptyStuff()
    {
    	db = new MyDB(this);
		db.open();
		Cursor c = db.getEmptyStuffTitles();
		if(c.moveToFirst())
		{
			db.close();
			return c;
		}
		db.close();
		return null;
    }
	
	public void BackMakeSure(int flag){
		if(flag == 0)
		{
			Cursor c = GetEmptyStuff();
			String sTemp = "";
			if (c != null && c.moveToFirst())
			{
				do 
				{
					sTemp += c.getString(1);
					sTemp += "  不足\n";
				} 
				while (c.moveToNext());			
			}
			else
			{
				sTemp = "无";
			}
			new AlertDialog.Builder(this)
			.setTitle("素材不足情况")
			.setMessage(sTemp)
			.setPositiveButton("确认", new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
					finish();
				}
			}).setNegativeButton("返回", new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface dialog, int which) {
				}
			})
			.show();
		}
		else if (flag == 1)
		{
		}
	}
	
	public final class ViewHolder{
		public TextView textDishID;
		public TextView textDishName;
		public SlipButton sbutton;
	}
	
	public class MyAdapter extends BaseAdapter {
		private LayoutInflater mInflater;
		
		public MyAdapter(Context context){
			this.mInflater = LayoutInflater.from(context);
		}
		@Override
		public int getCount() {
			// TODO Auto-generated method stub
			return mData.size();
		}
		@Override
		public Object getItem(int arg0) {
			// TODO Auto-generated method stub
			return null;
		}
		@Override
		public long getItemId(int arg0) {
			// TODO Auto-generated method stub
			return 0;
		}  

		@Override
		public View getView(int position, View convertView, ViewGroup parent) {
			ViewHolder holder = null;
			
				holder=new ViewHolder();  
				convertView = mInflater.inflate(R.layout.list2, null);
				holder.textDishName = (TextView)convertView.findViewById(R.id.list_dish_name);
				holder.sbutton=(SlipButton)convertView.findViewById(R.id.slipButton);
				final int p = position;  
				holder.sbutton.SetOnChangedListener(new OnChangedListener() {			
					@Override
					public void OnChanged(boolean checkState) {
						// TODO Auto-generated method stub

							String sID = (String) mData.get(p).get("dish_id");
							String sState = (String) mData.get(p).get("dish_state");
							int iFlag = 0;
							if(sState.equals(getResources().getString(R.string.dish_empty)))
								iFlag = 1;
							else
								iFlag = 0;
							ChangeDB(sID, iFlag);
							if(iFlag == 1)
								mData.get(p).put("dish_state", getResources().getString(R.string.dish_full));
							else
								mData.get(p).put("dish_state", getResources().getString(R.string.dish_empty));
							adapter.notifyDataSetChanged();
							sFlag = "1";
					}
				
				}
				);
				 
			holder.textDishName.setText((String)mData.get(position).get("dish_name"));
			if(mData.get(position).get("dish_state").equals(getResources().getString(R.string.dish_full)))
			{
				holder.sbutton.IsOpen = true;
			}
			else
			{
				holder.sbutton.IsOpen = false;
			}
			holder.sbutton.invalidate();
				return convertView;
		}
	}
}
