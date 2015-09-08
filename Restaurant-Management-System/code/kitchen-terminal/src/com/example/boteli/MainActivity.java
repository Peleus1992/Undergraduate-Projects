package com.example.boteli;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.example.boteli.SlipButton.OnChangedListener;
import com.example.boteli.SlipButton2.OnChangedListener2;
import android.R.bool;
import android.R.color;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.database.Cursor;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.os.SystemClock;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.Chronometer;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends Activity {
	MyDB db = null;
	private Handler mMainHandler = null;
	private List<Map<String, Object>> mData;
	private List<Map<String, Object>> mData2;
	private MyAdapter adapter;
	private MyAdapter2 adapter2;
	private String COOKER_CODE = "boteli";
	private long TIMEOUT = 40;
	RcvThread rThread = null;
    SndThread sThread = null;
    int port = 4567, caport = 11911;
    String ip = "", caip = "192.168.1.111";
    private int GET_CODE = 23;
// �����̣߳����ڽ��շ���������
    class RcvThread extends Thread {
    	InputStream iStream = null;
    	ServerSocket svSocket = null;
    	Socket rcvSocket = null;
    	byte buffer [] = new byte[1024*4];
    	String iString;
    	Message toMain;
    	int strLen;
    	int iFlag;
    	
    	public void run(){
    		try {
    			
    			System.out.println("rcvbegin");
				svSocket = new ServerSocket(port);
				//System.out.println("listen thread open");
				while(true)
				{
					rcvSocket = svSocket.accept();
					//System.out.println("_____________accept block_____________________");				
					iStream = rcvSocket.getInputStream();
					strLen = iStream.read(buffer);
					//System.out.println("-----------"+temp);
					iString = new String(buffer,0,strLen);
					if(iString.getBytes()[0]=='0')
						iFlag = 0;
					else if(iString.getBytes()[0]=='4')
						iFlag = 4;
					else 
						iFlag = -1;
					toMain = new Message();
					toMain.obj = iString;
					toMain.arg1 = iFlag;
					mMainHandler.sendMessage(toMain);
					//System.out.println(re);
				}
				//AddButton(re);
				//deletebutton.setText("fuck");
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			finally{
				try {
					if(svSocket != null)
						svSocket.close();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
    	}
    	
    }
//�����̣߳����ڷ��͸�����������
    class SndThread extends Thread {
    	private Handler childHander = null;
        private static final String CHILD_TAG = "ChildThread";
        public void run() {
            this.setName("ChildThread");
            // ��ʼ����Ϣѭ�����У���Ҫ��Handler����֮ǰ
            Looper.prepare();
            childHander = new Handler() {
                @Override
                public void handleMessage(Message msg) {
	                	String iString = (String) msg.obj;
	                	try {
	        				//System.out.println("begin out throw to server");
	        				Socket socket = new Socket(caip,caport);
	        				//System.out.println("begin out throw2");
	        				OutputStream outputStream = socket.getOutputStream();
	        				byte buffer[] = new byte[1024 * 4];
	        				buffer = iString.getBytes();
	        				int len = buffer.length;
	        				outputStream.write(buffer, 0, len);
	        				//DataOutputStream outputStream = new DataOutputStream(socket.getOutputStream());
	        				//outputStream.writeUTF(iString);
	        				outputStream.flush();
	        				socket.close();
	        			} catch (Exception e) {
	        				// TODO Auto-generated catch block
	        				e.printStackTrace();
        			}
                }
            };
            // �������߳���Ϣѭ������
            Looper.loop();
        }
    }
//���һ���µ�list 
    private List<Map<String, Object>> getData() {
		List<Map<String, Object>> list = new ArrayList<Map<String, Object>>();
		return list;
	}
//�����Ŀ�������¼�
    class ItemClickEvent implements OnItemClickListener 
    {

		@Override
		public void onItemClick(AdapterView<?> l, View arg1, int position,
				long arg3) {
			// TODO Auto-generated method stub
			Message toMain = new Message();
			toMain.arg2 = position;
			toMain.arg1 = 1;
			toMain.obj = "";
			mMainHandler.sendMessage(toMain);
		}
    	
    }

    class ItemClickEvent2 implements OnItemClickListener 
    {

		@Override
		public void onItemClick(AdapterView<?> l, View arg1, int position,
				long arg3) {
			// TODO Auto-generated method stub
		}
    	
    }
	
	public final class ViewHolder{
		public TextView textDishName;
		public TextView textDishNum;
		public TextView textDishState;
		public TextView textTableId;
		public TextView textShowTimer;
		public LinearLayout lLayout;
		public ProgressBar psb;
		public CheckBox cb;
	}
	
	public class MyAdapter extends BaseAdapter{
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
			
			if (convertView == null) {
				holder=new ViewHolder();  
				convertView = mInflater.inflate(R.layout.list1, null);
				holder.textDishName = (TextView)convertView.findViewById(R.id.dish_name);
				holder.textDishNum = (TextView)convertView.findViewById(R.id.dish_num);
				holder.textDishState = (TextView)convertView.findViewById(R.id.dish_state_show);
				holder.textTableId = (TextView)convertView.findViewById(R.id.table_id);	
				holder.textShowTimer = (TextView)convertView.findViewById(R.id.showtimer);
				holder.lLayout = (LinearLayout)convertView.findViewById(R.id.listbackgroud);
				holder.psb = (ProgressBar)convertView.findViewById(R.id.psb);
				holder.psb.setVisibility(8);
//				System.out.println(holder.timer.getText()+" id:" + String.valueOf(position));
				convertView.setTag(holder);
			}
			else{
				holder = (ViewHolder)convertView.getTag();
//				System.out.println("else" + holder.textDishName.getText());
			}
			long ltime = (Long)mData.get(position).get("timer");
			long lhour = ltime/60;
			long lmin = ltime%60;
			String stime = String.valueOf(lhour), stime2 = String.valueOf(lmin);
			if(lhour < 10)
				stime = "0"+stime;
			if(lmin < 10)
				stime2 = "0" + stime2;
//			System.out.println("id->"+mData.get(position).get("dish_num"));
			if(ltime >= TIMEOUT && TIMEOUT != 0)
				holder.lLayout.setBackgroundColor(Color.parseColor("#55ff0000"));
			else
				holder.lLayout.setBackgroundColor(Color.parseColor("#33ffffff"));
			if(TIMEOUT==0)
			{
				holder.textShowTimer.setTextColor(color.white);
			}
			else
			{
				holder.textShowTimer.setTextColor(0xff00ee00);
			}
			if(mData.get(position).get("dish_state").equals(getResources().getString(R.string.dish_begin)))
			{
				holder.textShowTimer.setVisibility(8);
				holder.psb.setVisibility(0);
			}
			else
			{
				holder.textShowTimer.setVisibility(0);
				holder.psb.setVisibility(8);
			}
			holder.textDishName.setText((String)mData.get(position).get("dish_name"));
			holder.textDishNum.setText((String)mData.get(position).get("dish_num"));	
			holder.textTableId.setText((String)mData.get(position).get("table_id"));
			holder.textDishState.setText((String)mData.get(position).get("dish_state"));
			holder.textShowTimer.setText(stime+":"+stime2);			
			return convertView;
		}
	}
	public class MyAdapter2 extends BaseAdapter{
		private LayoutInflater mInflater;
		
		public MyAdapter2(Context context){
			this.mInflater = LayoutInflater.from(context);
		}
		@Override
		public int getCount() {
			// TODO Auto-generated method stub
			return mData2.size();
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
			
			if (convertView == null) {
				holder=new ViewHolder();
				convertView = mInflater.inflate(R.layout.list1, null);
				holder.textDishName = (TextView)convertView.findViewById(R.id.dish_name);
				holder.textDishNum = (TextView)convertView.findViewById(R.id.dish_num);
				holder.textDishState = (TextView)convertView.findViewById(R.id.dish_state_show);
				holder.textTableId = (TextView)convertView.findViewById(R.id.table_id);	
				holder.cb = (CheckBox)convertView.findViewById(R.id.selected);
				holder.cb.setVisibility(0);
				final int p = position;
				holder.cb.setOnClickListener(new View.OnClickListener() {  
                     @Override 
                    public void onClick(View v) {  
                         CheckBox cb = (CheckBox)v; 
                         if(cb.isChecked())
                        	 mData2.get(p).put("ext1", true);
                         else
                        	 mData2.get(p).put("ext1", false);
                         adapter2.notifyDataSetChanged();
                     }  
                });  
				holder.textShowTimer = (TextView)convertView.findViewById(R.id.showtimer);
				holder.textShowTimer.setVisibility(8);
				holder.psb = (ProgressBar)convertView.findViewById(R.id.psb);
				holder.psb.setVisibility(8);
				
				convertView.setTag(holder);
			}
			else{
				holder = (ViewHolder)convertView.getTag();
//				System.out.println("else" + holder.textDishName.getText());
			}
			
//			System.out.println("id->"+mData.get(position).get("dish_num"));
			holder.textDishName.setText((String)mData2.get(position).get("dish_name"));
			holder.textDishNum.setText((String)mData2.get(position).get("dish_num"));	
			holder.textTableId.setText((String)mData2.get(position).get("table_id"));
			holder.textDishState.setText((String)mData2.get(position).get("dish_state"));
			holder.cb.setChecked(Boolean.valueOf(mData2.get(position).get("ext1").toString()));	
			return convertView;
		}
	}
// ��ʼ�����ݿ�
	public void InitMyDB()
	{
		db = new MyDB(this);
		db.open();
		if(db.insertTitle("1", "�峴�ײ�", "1") == -1)
			db.updateTitle("1", "�峴�ײ�", "1");
		if(db.insertTitle("2", "�Ļƹ�", "1") == -1)
			db.updateTitle("2", "�Ļƹ�", "1");
		if(db.insertTitle("3", "Ģ�����ײ�", "1") == -1)
			db.updateTitle("3", "Ģ�����ײ�", "1");
		if(db.insertTitle("4", "ը����", "1") == -1)
			db.updateTitle("4", "ը����", "1");
		if(db.insertTitle("5", "��������", "1") == -1)
			db.updateTitle("5", "��������", "1");
		if(db.insertTitle("6", "���軨��", "1") == -1)
			db.updateTitle("6", "���軨��", "1");
		if(db.insertTitle("7", "�ཷ����", "1") == -1)
			db.updateTitle("7", "�ཷ����", "1");
		if(db.insertTitle("8", "���Ŷ���", "1") == -1)
			db.updateTitle("8", "���Ŷ���", "1");
		if(db.insertTitle("9", "������", "1") == -1)
			db.updateTitle("9", "������", "1");
		if(db.insertTitle("10", "��������", "1") == -1)
			db.updateTitle("10", "��������", "1");
		if(db.insertTitle("11", "��������", "1") == -1)
			db.updateTitle("11", "��������", "1");
		if(db.insertTitle("12", "��բз", "1") == -1)
			db.updateTitle("12", "��բз", "1");
		if(db.insertTitle("13", "������", "1") == -1)
			db.updateTitle("13", "������", "1");
		if(db.insertTitle("14", "��Ƥ����", "1") == -1)
			db.updateTitle("14", "��Ƥ����", "1");
		if(db.insertTitle("15", "���Ӽ�", "1") == -1)
			db.updateTitle("15", "���Ӽ�", "1");
		if(db.insertTitle("16", "ը����", "1") == -1)
			db.updateTitle("16", "ը����", "1");
		if(db.insertTitle("17", "���͵���", "1") == -1)
			db.updateTitle("17", "���͵���", "1");
		if(db.insertTitle("18", "���µ���", "1") == -1)
			db.updateTitle("18", "���µ���", "1");
		if(db.insertTitle("19", "ˮ����", "1") == -1)
			db.updateTitle("19", "ˮ����", "1");
		if(db.insertTitle("20", "����Ͱ", "1") == -1)
			db.updateTitle("20", "����Ͱ", "1");
		if(db.insertTitle("21", "��ɫ����", "1") == -1)
			db.updateTitle("21", "��ɫ����", "1");
		if(db.insertTitle("22", "ɽ髵���", "1") == -1)
			db.updateTitle("22", "ɽ髵���", "1");
		if(db.insertTitle("23", "�ɿ�������", "1") == -1)
			db.updateTitle("23", "�ɿ�������", "1");
		if(db.insertTitle("24", "��ݮ����", "1") == -1)
			db.updateTitle("24", "��ݮ����", "1");
		if(db.insertTitle("25", "��֭", "1") == -1)
			db.updateTitle("25", "��֭", "1");
		if(db.insertTitle("26", "���ʹ�֭", "1") == -1)
			db.updateTitle("26", "���ʹ�֭", "1");
		if(db.insertTitle("27", "ˮ������", "1") == -1)
			db.updateTitle("27", "ˮ������", "1");
		if(db.insertTitle("28", "�����Ѿ�", "1") == -1)
			db.updateTitle("28", "�����Ѿ�", "1");
		if(db.insertTitle("29", "����֭", "1") == -1)
			db.updateTitle("29", "����֭", "1");
		if(db.insertTitle("30", "��ݮ֭", "1") == -1)
			db.updateTitle("30", "��ݮ֭", "1");	
		Cursor c = db.getAllTitles();
		if (c.moveToFirst())
		{
			do 
			{
				//DisplayTitle(c);
			} 
			while (c.moveToNext());
		}
		db.close();
	}
// չʾĳ������	
	public void DisplayTitle(Cursor c)
    {
    	Toast.makeText(this,"dish_id: " + c.getString(0) + "\n" +"dish_name: " + 
    			c.getString(1) + "\n" +"dish_stuff: " + c.getString(2),Toast.LENGTH_SHORT).show();
    }  
//չʾһ���ַ���
	public void DisplayString(String str)
	{
		Toast.makeText(this,str,Toast.LENGTH_LONG).show();
	}
//�ͷ�������������	
    void NotifyServer()
    {
    	String sTemp = "8";
    	Message toChild = new Message();
    	toChild.obj = sTemp;
    	sThread.childHander.sendMessage(toChild);
    }
//��õ��ص�ip��ַ
    public String getLocalIpAddress() {    
        try {    
            for (Enumeration<NetworkInterface> en = NetworkInterface    
                    .getNetworkInterfaces(); en.hasMoreElements();) {    
                NetworkInterface intf = en.nextElement();    
                for (Enumeration<InetAddress> enumIpAddr = intf    
                        .getInetAddresses(); enumIpAddr.hasMoreElements();) {    
                    InetAddress inetAddress = enumIpAddr.nextElement();    
                    if (!inetAddress.isLoopbackAddress()&& !inetAddress.isLinkLocalAddress()) {    
                        return inetAddress.getHostAddress().toString();    
                    }    
                }    
            }    
        } catch (SocketException ex) {    
            Log.e("WifiPreference IpAddress", ex.toString());    
        }    
        return null;    
    }    
//�˵���ť����
    class ShiftBTlistener implements OnClickListener
    {

		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
			Intent intent = new Intent();
			intent.setClass(MainActivity.this, DishList.class);
			startActivityForResult(intent, GET_CODE);  
		}
    	
    }
//���ð�ť����
    class IpBTlistener implements OnClickListener
    {

		@Override
		public void onClick(View v) {				//
				LayoutInflater factory = LayoutInflater.from(MainActivity.this);
				final View passView = factory.inflate(R.layout.dialog_password,
						null);
				new AlertDialog.Builder(MainActivity.this)
						.setTitle("����������")
						.setIcon(android.R.drawable.ic_dialog_info)
						.setView(passView)
						.setPositiveButton("ȷ��",
						new DialogInterface.OnClickListener() 
						{
							public void onClick(DialogInterface dialog, int which) 
							{
								// TODO Auto-generated method stub
								AlertDialog ad = (AlertDialog) dialog;
								EditText content = (EditText) ad.findViewById(R.id.dialog_password);
								String string = content.getText().toString();
								if (string.equals(COOKER_CODE)) 
								{
									LayoutInflater factory = LayoutInflater.from(MainActivity.this);
											// �õ��Զ���Ի���
									final View DialogView = factory.inflate(R.layout.dialog_loin,null);
											// �����Ի���
									EditText temp = (EditText) DialogView.findViewById(R.id.text_ip);
									temp.setText(ip);
									temp.setFocusable(false);
									temp = (EditText) DialogView.findViewById(R.id.text_port);
									String iString = String.valueOf(port);
									temp.setText(iString);
									temp.setFocusable(false);
									temp = (EditText) DialogView.findViewById(R.id.text_caip);
									temp.setText(caip);
									temp.setFocusable(true);
									temp = (EditText) DialogView.findViewById(R.id.text_caport);
									String iString2 = String.valueOf(caport);
									temp.setText(iString2);
									temp.setFocusable(true);
									temp = (EditText) DialogView.findViewById(R.id.text_sec);
									String iString3 = String.valueOf(TIMEOUT);
									temp.setText(iString3);
									temp.setFocusable(true);
									SlipButton2 temp2 = (SlipButton2) DialogView.findViewById(R.id.timeslipButton);
									if(TIMEOUT == 0)
									{
										temp2.IsOpen = false;
										temp2.invalidate();
										temp.setVisibility(8);
										DialogView.findViewById(R.id.sectitle).setVisibility(8);
									}
									else
									{
										temp2.IsOpen = true;
										temp2.invalidate();
										temp.setVisibility(0);
										DialogView.findViewById(R.id.sectitle).setVisibility(0);
									}	
									temp2.SetOnChangedListener(new OnChangedListener2() {
										
										@Override
										public void OnChanged(boolean CheckState) {
											// TODO Auto-generated method stub
											if(CheckState)
											{
												EditText temp = (EditText)DialogView.findViewById(R.id.text_sec);
												temp.setVisibility(0);
												DialogView.findViewById(R.id.sectitle).setVisibility(0);
												temp.setText("0");
											}
											else
											{
												EditText temp = (EditText)DialogView.findViewById(R.id.text_sec);
												temp.setVisibility(8);
												DialogView.findViewById(R.id.sectitle).setVisibility(8);
												temp.setText("0");
											}
										}
									});
									new AlertDialog.Builder(MainActivity.this)
										.setTitle("����")
										.setView(DialogView)
										// �����Զ���Ի�����ʽ
										.setPositiveButton("ȷ��",
										new DialogInterface.OnClickListener()
										{
											@Override
											public void onClick(DialogInterface dialog,int which)
											{
												AlertDialog ad = (AlertDialog) dialog;
												EditText temp = (EditText) ad.findViewById(R.id.text_caip);
												String iString = temp.getText().toString();
												if (iString.length() != 0) 
												{
													caip = iString;
												}
												temp = (EditText) ad.findViewById(R.id.text_caport);
												iString = temp.getText().toString();
												if (iString.length() != 0) 
												{
													caport = Integer.valueOf(iString).intValue();
												}
												temp = (EditText) ad.findViewById(R.id.text_sec);
												iString = temp.getText().toString();
												if (iString.length() != 0) 
												{
													if(TIMEOUT != Integer.valueOf(iString).intValue())
													{
														if(!iString.equals(""))
															TIMEOUT = Integer.valueOf(iString).intValue();
														else
															TIMEOUT = 0;
														if(TIMEOUT!=0)
															DisplayString("�˿͵ȴ���ʱʱ��\n����Ϊ: "+iString+"��");
														else
															DisplayString("��ʱ���ѹر�");
														Message toMain = new Message();
														toMain.arg1 = 7;
														mMainHandler.sendMessage(toMain);
														adapter.notifyDataSetChanged();
													}
													else 
													{
														
													}
												}
											}
										}).setNegativeButton("ȡ��", null).show();	
									}
								}
							}).setNegativeButton("ȡ��", null).show();
				
			}

    	
    }
//ɾ����ť����
    class DeleteBTlistener implements OnClickListener
    {

		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
				////////////////////////////////////////////notify server
				if(!caip.equals(""))
				{	
					Message toMain = new Message();
					toMain.arg1 = 6;
					toMain.obj = "";//����Ϊ����
					mMainHandler.sendMessage(toMain);
				}
					
				////////////////////////////////////////////notify end
		}
    	
    }
//ȫѡ��ť����
    class FullGetBTlistener implements OnClickListener
    {

		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
				////////////////////////////////////////////notify server
				if(!caip.equals(""))
				{	
					Message toMain = new Message();
					toMain.arg1 = 8;
					toMain.obj = "";//����Ϊ����
					mMainHandler.sendMessage(toMain);
				}
					
				////////////////////////////////////////////notify end
		}
    	
    }
//���Ӱ�ť����
    class LinkBTlistener implements OnClickListener
    {

		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
				////////////////////////////////////////////notify server
				if(!caip.equals(""))
				{	
					NotifyServer();
					DisplayString("���ӷ�����\n����IP:"+ip+"\n������IP:"+caip);
				}
					
				////////////////////////////////////////////notify end
		}
    	
    }
//����ť����
    class SortBTlistener implements OnClickListener
    {

		@Override
		public void onClick(View arg0) {
			// TODO Auto-generated method stub
			Message toMain = new Message();
			toMain.arg1 = 2;
			toMain.obj = "";
			mMainHandler.sendMessage(toMain);
		}
    	
    }
//����һ��activity���صĲ�����ú���   
    @Override  
    protected void onActivityResult(int requestCode, int resultCode, Intent data) 
    {  
	    if (requestCode == GET_CODE)
	    {  
	        if (resultCode == RESULT_CANCELED) 
	        {} 
	        else 
	        {  
				System.out.println("restart");
				String sFlag = data.getStringExtra("dish_stuff_flag");
				if(sFlag.equals("1"))
				{
					Message toMain = new Message();
					toMain.obj = "";
					toMain.arg1 = 3;
					mMainHandler.sendMessage(toMain);
				}
						    		//System.out.println("threadflag=1");
	        }  
	    }  
    }  
    @Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		ListView list1 = (ListView)findViewById(R.id.listup);
		ListView list2 = (ListView)findViewById(R.id.listdown);
		Button shifButton = (Button)findViewById(R.id.shifbutton);
		Button ipButton = (Button)findViewById(R.id.ipbutton);
		Button linkButton = (Button)findViewById(R.id.linkbutton);
		Button sortButton = (Button)findViewById(R.id.sortbutton);
		Button deleteButton = (Button)findViewById(R.id.deletebutton);
		Button fullgetButton = (Button)findViewById(R.id.fullgetbutton);
		Chronometer timer = (Chronometer)findViewById(R.id.chronometer);
		shifButton.setOnClickListener(new ShiftBTlistener());
		ipButton.setOnClickListener(new IpBTlistener());
		linkButton.setOnClickListener(new LinkBTlistener());
		sortButton.setOnClickListener(new SortBTlistener());
		deleteButton.setOnClickListener(new DeleteBTlistener());
		fullgetButton.setOnClickListener(new FullGetBTlistener());
		/////////////////////////////////DB
		InitMyDB();
		///////////////////////////////////////////DBend
		///////////////////////////////////////////Listview
		mData = getData();
		mData2 = getData();
		adapter = new MyAdapter(this);
		adapter2 = new MyAdapter2(this);
		System.out.println("cao2");
	    list1.setOnItemClickListener(new ItemClickEvent());
	    list1.setAdapter(adapter);
	    System.out.println("cao3");
	    list2.setAdapter(adapter2);
	    System.out.println("cao4");
		///////////////////////////////////////////Listviewend
		mMainHandler = new Handler(){
	        @Override
	        public void handleMessage(Message msg) {
	        	String iString = (String) msg.obj;
	            switch(msg.arg1)
	            {
		            case 0:
		            {
		            	String subString[] = iString.split(","), strTemp[] = new String[3];
		            	String sTableId[] = new String[2];
		            	sTableId[0]=subString[0];
		            	sTableId[1]=subString[1];
		            	int strLen = subString.length;
		            	for(int i = 2; i < strLen; i++)
		            	{
		            		strTemp[0] = subString[i++];
		            		strTemp[1] = subString[i++];
		            		strTemp[2] = subString[i];
		            		AddDishList(sTableId[1], strTemp[0], strTemp[1], GetNamebyId(strTemp[1]), 
		            				strTemp[2], getResources().getString(R.string.dish_wait));
		            	}
		            	adapter.notifyDataSetChanged();
		            	break;
		            }
		            case 1:
		            {
		            	//update list ("begin")
		            	if(mData.get(msg.arg2).get("dish_state").equals(getResources().getString(R.string.dish_wait)))
		            	{
		            		ChangeDishList(msg.arg2);
		            	}
		            	else if(mData.get(msg.arg2).get("dish_state").equals(getResources().getString(R.string.dish_begin)))
		            	{
		            		ChangeDishList(msg.arg2);
		            		DeleteDishList(msg.arg2);
		            	}
		            		
		            	break;
		            }
		            case 2:
		            {
		            	SortDishList();
		            	break;
		            }
		            case 3:
		            {
		            	//deleteList
		            	DeleteDishListArr();
		            	break;
		            }
		            case 4:
		            {
		            	String subString[] = iString.split(",");
		            	//updata list
		            	UpdateDishList(subString[1], subString[2], subString[3]);
		            	break;
		            }
		            case 5:
		            {
		            	SetDishListFirst(msg.arg2);
		            	break;
		            }
		            case 6:
		            {
		            	System.out.println("cao6");
		            	RemoveDishList();
		            	break;
		            }
		            case 7:
		            {
		            	Chronometer timer = (Chronometer)findViewById(R.id.chronometer);
		            	if(TIMEOUT != 0)
		            	{
							timer.start();
		            	}
		            	else
		            	{
		            		timer.stop();
		            	}
		            	break;
		            }
		            case 8:
		            {
		            	SeleteDishList();
		            	break;
		            }
		            default:
		            	break;
	            }
	        }
	    };
	    ip = getLocalIpAddress();
	    rThread = new RcvThread();
	    rThread.start();
	    sThread = new SndThread();
	    sThread.start();
		timer.setVisibility(8);
	    timer.setOnChronometerTickListener
		(new Chronometer.OnChronometerTickListener() 
			{        
				@Override     
				public void onChronometerTick(Chronometer timer) 
				{        // �����ʼ��ʱ�����ڳ�����startime��   
					int size = mData.size();
					long lTemp = 0;
					System.out.println();
					for(int i = 0; i < size; i++)
					{
						if(mData.get(i).get("dish_state").equals
								(getResources().getString(R.string.dish_wait)))
						{
							lTemp = (Long)mData.get(i).get("timer");
							lTemp += 1;
							mData.get(i).put("timer", lTemp);
							if(lTemp == TIMEOUT)
							{
								Message toMain = new Message();
								toMain.arg1 = 5;
								toMain.arg2 = i;
								toMain.obj = "";
								mMainHandler.sendMessage(toMain);
							}
						}
						
					}
					adapter.notifyDataSetChanged();
				}
			}
		);
		timer.setBase(SystemClock.elapsedRealtime());
		timer.start();
	}
//��ò��ϲ���Ĳ�
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
//ͨ����id��ò���
    String GetNamebyId(String strDishId)
    {
    	String sTemp;
    	db = new MyDB(this);
		db.open();
		Cursor c = db.getTitle(strDishId);
		if(c.moveToFirst())
		{
			sTemp = c.getString(1);
			db.close();
			return sTemp;
		}
		db.close();
		return null;
    }
//���û���������ӵ��б�
    void AddDishList(String sTableId, String sDishSeq, String sDishId, 
    		String sDishName, String sDishNum, String sDishState)
    {
    	Map<String, Object> map = new HashMap<String, Object>();
		map.put("table_id", sTableId);
		map.put("dish_seq", sDishSeq);
		map.put("dish_id", sDishId);
		map.put("dish_name", sDishName);
		map.put("dish_num", sDishNum);
		map.put("dish_state", sDishState);
		map.put("timer", (long)0);
		map.put("ext1", false);
		map.put("ext2", false);
		mData.add(map);
    }
//ɾ��һ���û�����
    void DeleteDishList(int position)
    {
    	//mData.get(position).put("dish_state", getResources().getString(R.string.dish_finish));
    	mData2.add(mData.get(position));
    	adapter2.notifyDataSetChanged();
    	mData.remove(position);
    	adapter.notifyDataSetChanged();
    }
//��ʦ�ı�����״̬
    void ChangeDishList(int position)
    {
    	String sTemp = (String)mData.get(position).get("table_id") + "," + 
    			(String)mData.get(position).get("dish_seq");
    	if(mData.get(position).get("dish_state").equals(getResources().getString(R.string.dish_wait)))
    	{
    		mData.get(position).put("dish_state", getResources().getString(R.string.dish_begin));
    		sTemp = "2," + sTemp;
    	}
    	else
    	{
    		mData.get(position).put("dish_state", getResources().getString(R.string.dish_finish));
    		sTemp = "3," + sTemp;
    	}
    	adapter.notifyDataSetChanged();
    	if(!caip.equals(""))
    	{
    		Message toChild = new Message();
    		toChild.obj = sTemp;
    		sThread.childHander.sendMessage(toChild);
    	}
    	else
    	{
    		DisplayString("please set network");
    	}
    }
//	�����û�������ı��б�
	void UpdateDishList(String sTableId, String sDishSeq, String sDishNum)
	{
		int size = mData.size();
		int iFlag = 0;
		for(int i = 0; i < size; i++)
		{
			if(mData.get(i).get("table_id").equals(sTableId) && 
					mData.get(i).get("dish_seq").equals(sDishSeq))
			{
				if(!sDishNum.equals("0"))
				{
					mData.get(i).put("dish_num", sDishNum);
				}
				else
				{
					mData.remove(i);
				}
				iFlag = 1;
				break;
			}
		}
		if(iFlag == 1)
		{
			adapter.notifyDataSetChanged();
		}
	}
// 	���ݲ˵Ĳ��ϲ��㣬���²˵�����
	void DeleteDishListArr()
	{
		Cursor c = GetEmptyStuff();
		int listSize, iFlag = 0;
		String sTemp = null;
		if (c != null && c.moveToFirst())
		{
			do 
			{
				//DisplayTitle(c);
				listSize = mData.size();
				sTemp = "7," + c.getString(0);
//				System.out.println("size->"+listSize);
				for(int i = listSize-1; i >= 0; i--)
				{
					if(mData.get(i).get("dish_id").equals(c.getString(0)) && 
						mData.get(i).get("dish_state").equals(getResources().getString(R.string.dish_wait)))
					{
						
//						sTemp += ("\n" + (String)mData.get(i).get("table_id") + "," + 
//						(String)mData.get(i).get("dish_seq"));
						mData.remove(i);  
						iFlag = 1;
					}
				}
				adapter.notifyDataSetChanged();
				Message toChild = new Message();
				toChild.obj = sTemp;
				sThread.childHander.sendMessage(toChild);
			} 
			while (c.moveToNext());
//			if(iFlag == 1)
//			{
//				Message toChild = new Message();
//				toChild.obj = sTemp;
//				sThread.childHander.sendMessage(msg);
//			}			
		}
	}
//	ѡ���б���Ŀ
	void SeleteDishList()
	{
		int size = mData2.size();
		for(int i = size-1; i >=0; i--)
		{
			mData2.get(i).put("ext1",true);
		}
		adapter2.notifyDataSetChanged();
	}
// ����ɾ����Ŀ
	void RemoveDishList()
	{
		int size = mData2.size();
		for(int i = size-1; i >=0; i--)
		{
			if(Boolean.valueOf(mData2.get(i).get("ext1").toString()))
			{
				mData2.remove(i);
				
			}
		}
		adapter2.notifyDataSetChanged();
	}
// �����û�����
	void SortDishList()
	{
		int size = mData.size();
		for(int i = 0; i < size; i++)
		{
			for(int j = i+1; j < size; j++)
			{
				if(mData.get(j).get("dish_id").equals(mData.get(i).get("dish_id")) && (i+1 < j))
				{
					mData.add(++i, mData.get(j));
					mData.remove(j+1);
				}
			}
		}
		adapter.notifyDataSetChanged();
	}
//	����ʱ��������Ϊ������ȼ�
	void SetDishListFirst(int postion)
	{
		System.out.println("delete");
		mData.add(0, mData.get(postion));
		mData.remove(postion+1);
		adapter.notifyDataSetChanged();
	}
   
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

}
