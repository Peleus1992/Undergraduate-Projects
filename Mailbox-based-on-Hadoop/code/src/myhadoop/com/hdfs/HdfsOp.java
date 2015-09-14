package myhadoop.com.hdfs;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FSDataOutputStream;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;

public class HdfsOp {
	//创建新文件
	public final static int FILE_MAX_SIZE = 1 * 1024 * 1024 * 1024;
	public final static int BUFFER_SIZE = 1024;
	public static boolean createFile(String dst , InputStream in, long length ){
		if(length > FILE_MAX_SIZE) {
			return false;
		}
		boolean flag = false;
		FSDataOutputStream out = null;
		FileSystem fs = null;
		try {
			Configuration cfg = new Configuration();
			fs = FileSystem.get(cfg);
			Path dstPath = new Path(dst);
			out = fs.create(dstPath);
			int iFlag = -1;
			byte [] buffer = new byte[BUFFER_SIZE];
			while((iFlag = in.read(buffer)) != -1) {
				out.write(buffer, 0, iFlag);
			}
			flag = true;
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				in.close();
				out.close(); 
				fs.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} 
		return flag;
	}
	public static boolean appendFile(String dst , byte[] contents){
		if(contents.length > FILE_MAX_SIZE) {
			return false;
		}
		boolean flag = false;  
		FSDataOutputStream out = null;
		FileSystem fs = null;
		try {
			Configuration cfg = new Configuration();
			fs = FileSystem.get(cfg);
			Path dstPath = new Path(dst);
			out = fs.append(dstPath);
			out.write(contents); 
			flag = true;
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			try {
				out.close();
				fs.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} 
		return flag;
	}
    //上传本地文件
	public static boolean uploadFile(String src,String dst){
		boolean flag = false;
		FileSystem fs = null;
		try {
			Configuration cfg = new Configuration();
			fs = FileSystem.get(cfg);
			Path srcPath = new Path(src); //原路径
			Path dstPath = new Path(dst); //目标路径
			fs.copyFromLocalFile(false,srcPath, dstPath);
			flag = true;
		} catch(Exception e) {
			e.printStackTrace();
		} finally {
			try {
				fs.close();
			} catch(Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
        //打印文件路径
//        System.out.println("Upload to "+conf.get("fs.default.name"));
//        System.out.println("------------list files------------"+"\n");
//        FileStatus [] fileStatus = fs.listStatus(dstPath);
//        for (FileStatus file : fileStatus) 
//        {
//            System.out.println(file.getPath());
//        }
//        fs.close();
	}
    
//文件重命名
	public static boolean rename(String oldName,String newName) {
		boolean flag = false;
		FileSystem fs = null;
		try {
			Configuration cfg = new Configuration();
			fs = FileSystem.get(cfg);
			Path oldPath = new Path(oldName);
			Path newPath = new Path(newName);
			flag = fs.rename(oldPath, newPath);
		} catch(Exception e) {
			e.printStackTrace();
		} finally {
			try {
				fs.close();
			} catch(Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
	//        if(isok){
	//            System.out.println("rename ok!");
	//        }else{
	//            System.out.println("rename failure");
	//        }
	}
    //删除文件
    public static boolean delete(String filePath) {
    	boolean flag = false;
		FileSystem fs = null;
		try {
			Configuration cfg = new Configuration();
			fs = FileSystem.get(cfg);
        Path path = new Path(filePath);
        flag = fs.deleteOnExit(path);
		} catch(Exception e) {
			e.printStackTrace();
		} finally {
			try {
				fs.close();
			} catch(Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
//        if(isok){
//            System.out.println("delete ok!");
//        }else{
//            System.out.println("delete failure");
//        }
//        fs.close();
    }
    
    //创建目录
    public static boolean mkdir(String path) {
    	boolean flag = false;
		FileSystem fs = null;
		try {
			Configuration cfg = new Configuration();
			fs = FileSystem.get(cfg);
         Path srcPath = new Path(path);
         flag = fs.mkdirs(srcPath);
		} catch(Exception e) {
			e.printStackTrace();
		} finally {
			try {
				fs.close();
			} catch(Exception e) {
				e.printStackTrace();
			}
		}
		return flag;
//        if(isok){
//            System.out.println("create dir ok!");
//        }else{
//            System.out.println("create dir failure");
//        }
//        fs.close();
    }
    
    //读取文件的内容
    //REMEMBER TO CLOSE INPUTSTREAM
    public static boolean readFile(OutputStream out, String filePath) {
    	InputStream in = null;
		FileSystem fs = null;
		try {
			Configuration cfg = new Configuration();
			fs = FileSystem.get(cfg);
        Path srcPath = new Path(filePath);
            in = fs.open(srcPath);
            byte [] buffer = new byte[BUFFER_SIZE];
            int iFlag = -1;
            while((iFlag = in.read(buffer)) != -1) {
            	out.write(buffer);
            }
            out.close();
			in.close();
			fs.close();
			return true;
       } catch(Exception e) {
			e.printStackTrace();
		} finally {
			try {
				out.close();
				in.close();
				fs.close();
			} catch(Exception e) {
				e.printStackTrace();
			}
		}
		return false;
    }
    
    public static byte[] readFile(String filePath) {
    	InputStream in = null;
		FileSystem fs = null;
		try {
			Configuration cfg = new Configuration();
			fs = FileSystem.get(cfg);
        Path srcPath = new Path(filePath);
            in = fs.open(srcPath);
            byte [] buffer = new byte[(int)fs.getFileStatus(srcPath).getLen()];
            in.read(buffer);

			in.close();
			fs.close();
			return buffer;
       } catch(Exception e) {
			e.printStackTrace();
		} finally {
			try {
				in.close();
				fs.close();
			} catch(Exception e) {
				e.printStackTrace();
			}
		}
		return null;
    }
}
