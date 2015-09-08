package com.example.boteli;
import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.SQLException;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.util.Log;
public class MyDB {
	public static final String KEY_DISHID = "dish_id";
	public static final String KEY_DISHNAME = "dish_name";
	public static final String KEY_DISHSTUFF = "dish_stuff";
	private static final String TAG = "DBAdapter";
	private static final String DATABASE_NAME = "CookerEnd";
	private static final String DATABASE_TABLE = "DishList";
	private static final int DATABASE_VERSION = 5;
	private static final String DATABASE_DROP =
			"drop table if exits DishList ;";
	private static final String DATABASE_CREATE = 
			"create table DishList (dish_id text primary key , " +
			"dish_name text not null, " +"dish_stuff text not null);";
	private final Context context;
	private DatabaseHelper DBHelper;
	private SQLiteDatabase db;
	public MyDB(Context ctx)
	{
		this.context = ctx;
		DBHelper = new DatabaseHelper(context);
	}
	private static class DatabaseHelper extends SQLiteOpenHelper
	{
		DatabaseHelper(Context context)
		{
			super(context, DATABASE_NAME, null, DATABASE_VERSION);
		}

		@Override
		public void onCreate(SQLiteDatabase db) {
			// TODO Auto-generated method stub
			System.out.println(DATABASE_CREATE);
			db.execSQL(DATABASE_CREATE);
		}

		@Override
		public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
			// TODO Auto-generated method stub
			Log.w(TAG, "Upgrading database from version " + oldVersion+ " to "+ newVersion + ", which will destroy all old data");
			db.execSQL("DROP TABLE IF EXISTS DishList");
			System.out.println("DROP TABLE IF EXISTS DishList");
			onCreate(db);
		}
	}
	public MyDB open() throws SQLException
	{
		db = DBHelper.getWritableDatabase();
		return this;
	}
	public void close()
	{
		DBHelper.close();
	}
	public long insertTitle(String dish_id, String dish_name, String dish_stuff)
	{
		ContentValues initialValues = new ContentValues();
		initialValues.put(KEY_DISHID, dish_id);
		initialValues.put(KEY_DISHNAME, dish_name);
		initialValues.put(KEY_DISHSTUFF, dish_stuff);
		return db.insert(DATABASE_TABLE, null, initialValues);
	}
	public boolean deleteTitle(String strDish)
	{
		return db.delete(DATABASE_TABLE, KEY_DISHID +"=" + strDish, null) > 0;
	}
	public Cursor getAllTitles()
	{
		return db.query(DATABASE_TABLE, new String[] 
			{KEY_DISHID,KEY_DISHNAME,KEY_DISHSTUFF},null,null,null,null,null);
	}
	public Cursor getEmptyStuffTitles()
	{
		return db.query(DATABASE_TABLE, new String[] 
			{KEY_DISHID,KEY_DISHNAME,KEY_DISHSTUFF},KEY_DISHSTUFF+"=?",new String[]{"0"},null,null,null);
	}
	public Cursor getTitle(String strDish) throws SQLException
	{
		Cursor mCursor = db.query(true, DATABASE_TABLE, new String[] {KEY_DISHID,KEY_DISHNAME, KEY_DISHSTUFF},
					KEY_DISHID + "=" + strDish,null,null,null,null,null);
		if (mCursor != null)
		{
			mCursor.moveToFirst();
		}
		return mCursor;
	}
	public boolean updateTitle(String strDish, String strName, String strStuff)
	{
		ContentValues args = new ContentValues();
		args.put(KEY_DISHID, strDish);args.put(KEY_DISHNAME, strName);args.put(KEY_DISHSTUFF, strStuff);
		return db.update(DATABASE_TABLE, args,KEY_DISHID + "=" + strDish, null) > 0;
	}
}
