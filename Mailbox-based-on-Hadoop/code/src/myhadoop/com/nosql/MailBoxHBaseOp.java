package myhadoop.com.nosql;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.ArrayList;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.hbase.HBaseConfiguration;
import org.apache.hadoop.hbase.HColumnDescriptor;
import org.apache.hadoop.hbase.HTableDescriptor;
import org.apache.hadoop.hbase.client.Delete;
import org.apache.hadoop.hbase.client.Get;
import org.apache.hadoop.hbase.client.HBaseAdmin;
import org.apache.hadoop.hbase.client.HTable;
import org.apache.hadoop.hbase.client.Put;
import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.client.ResultScanner;
import org.apache.hadoop.hbase.client.Scan;
import org.apache.hadoop.hbase.filter.CompareFilter.CompareOp;
import org.apache.hadoop.hbase.filter.FilterList;
import org.apache.hadoop.hbase.filter.SingleColumnValueFilter;
import org.apache.hadoop.hbase.util.Bytes;

public class MailBoxHBaseOp {
	private static Configuration cfg = null;
	//default max size of bytes in hbase is 10mb, we define 5 mb here.
	public final static long FILE_MAX_SIZE = 8 * 1024 * 1024;
	static {
		cfg = HBaseConfiguration.create();
//		System.out.println(cfg.get("hadoop.tmp.dir"));
	}
	//create a table
	public static boolean create(String tableName, String[] columnFamilies) {
		boolean flag = false;
		HBaseAdmin admin = null;
		try { 
			admin = new HBaseAdmin(cfg);
			if (!admin.tableExists(tableName)) {
				HTableDescriptor tableDesc = new HTableDescriptor(tableName);
				for(String columnFamily : columnFamilies) {
					tableDesc.addFamily(new HColumnDescriptor(columnFamily));
				}
				admin.createTable(tableDesc);
				flag = true;
			}
			
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				admin.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
	}
	//whether the table exists
	public static boolean exists(String tableName) {
		boolean flag = false;
		HBaseAdmin admin = null;
		try {
			admin = new HBaseAdmin(cfg);
			if (admin.tableExists(tableName)) {
				flag = true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				admin.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
	}
	//delete a table
	public static boolean drop(String tableName) {
		boolean flag = false;
		HBaseAdmin admin = null;
		try {
			admin = new HBaseAdmin(cfg);
			if (admin.tableExists(tableName)) {
				admin.disableTable(tableName);
				admin.deleteTable(tableName);
				flag = true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				admin.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
	}
	//put a cell in the table
	public static boolean put(String tablename, String row, String columnFamily,
			String column, String data) {
		boolean flag = false;
		HTable table = null;
		try {
			table = new HTable(cfg, tablename);
			Put put = new Put(Bytes.toBytes(row));
			put.add(Bytes.toBytes(columnFamily), Bytes.toBytes(column),
				Bytes.toBytes(data));
			table.put(put);
			flag = true;
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				table.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
	}
	//put a cell in the table
	public static boolean putFile(String tablename, String row, String columnFamily, 
			String column, InputStream in, long length) {
		if(length > FILE_MAX_SIZE) {
			return false;
		}
		boolean flag = false;
		HTable table = null;
		try {
			byte[] buffer = new byte[(int)length];
			in.read(buffer);
			table = new HTable(cfg, tablename);
			Put put = new Put(Bytes.toBytes(row));
			put.add(Bytes.toBytes(columnFamily), Bytes.toBytes(column),
				buffer);
			table.put(put);
			flag = true;
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				table.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
	}
	//get a cell from table
	public static byte[] get(String tablename, String row, String columnFamily,
			String column) {
		byte[] buffer = null;
		HTable table = null;
		try {
			table = new HTable(cfg, tablename);
			Get get = new Get(Bytes.toBytes(row));
			get.addColumn(Bytes.toBytes(columnFamily), Bytes.toBytes(column));
			Result r = table.get(get);
			buffer = r.getValue(Bytes.toBytes(columnFamily), Bytes.toBytes(column));
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				table.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return buffer;
	}
	public static Result get(String tablename, String row) {
		byte[] buffer = null;
		HTable table = null;
		Result r = null;
		try {
			table = new HTable(cfg, tablename);
			Get get = new Get(Bytes.toBytes(row));
//			get.addColumn(Bytes.toBytes(columnFamily), Bytes.toBytes(column));
			r = table.get(get);
//			buffer = r.getValue(Bytes.toBytes(columnFamily), Bytes.toBytes(column));
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				table.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return r;
	}
	//get a cell from the table
	public static InputStream getFile(String tablename, String row, String columnFamily,
			String column) {
		ByteArrayInputStream bais = null;
		HTable table = null;
		try {
			table = new HTable(cfg, tablename);
			Get get = new Get(Bytes.toBytes(row));
			get.addColumn(Bytes.toBytes(columnFamily), Bytes.toBytes(column));
			Result r = table.get(get);
			byte[] buffer = r.getValue(Bytes.toBytes(columnFamily), Bytes.toBytes(column));
			bais = new ByteArrayInputStream(buffer);
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				table.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return bais;
	}
	//delete
	public static boolean delete(String tablename, String row)  {  
		boolean flag = false;
		HTable table = null;
      try {  
			table = new HTable(cfg, tablename);  
			ArrayList list = new ArrayList();  
			Delete d1 = new Delete(row.getBytes());  
			list.add(d1);  
			table.delete(list);  
			flag = true;
		      
		} catch (Exception e) {  
		    e.printStackTrace();  
		}  finally {
			try {
				table.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
  
    }  
	//row exists
	public static boolean existsRow(String tablename, String row) {
		boolean flag = false;
		HTable table = null;
		try {
			table = new HTable(cfg, tablename);
			Get get = new Get(Bytes.toBytes(row));
			Result r = table.get(get);
			if(!r.isEmpty()) {
				flag = true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				table.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
	}
	//scan {select $(columns) from $(tableName) where $(columnQualifiers)}
	//REMEMBER TO CLOSE RESULTSCANNER.
	public static ResultScanner scan(String tableName, ArrayList<String[]> columnQualifiers
			,ArrayList<String[]> columns) {

		HTable table = null;
		ResultScanner rs = null;
		try {
			table = new HTable(cfg, tableName);
			Scan s = new Scan();
			if(columnQualifiers != null) {
				FilterList fl = new FilterList();
				for(String[] columnQualifier : columnQualifiers) {
					SingleColumnValueFilter f = new SingleColumnValueFilter(
						Bytes.toBytes(columnQualifier[0]),
						Bytes.toBytes(columnQualifier[1]),
						CompareOp.EQUAL,
						Bytes.toBytes(columnQualifier[2]) 
					);
					fl.addFilter(f);
				}
				s.setFilter(fl);
			}
			
			for(String[] column : columns) {
				s.addColumn(Bytes.toBytes(column[0]), Bytes.toBytes(column[1]));
			}
			rs = table.getScanner(s);
//			for (Result r : rs) {
//				for(Cell c : r.rawCells()) {
//					System.out.print("row: " + new String(c.getRow()));
//					System.out.print("family: " + new String(c.getFamily()));
//					System.out.print("key: " + new String(c.getQualifier()));
//					System.out.print("value: " + new String(c.getValue()));
//					System.out.println();
//				}
//			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				table.close();
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		return rs;
		
	}
}
