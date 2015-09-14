package myhadoop.com.hdfs;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.OutputStream;

import myhadoop.com.util.RandomName;


public class HdfsImpl {
	private final static String ROOT_DIR = "/mailbox";
	private final static String MAIL_DIR = ROOT_DIR + "/mail/";
	private final static String FILE_DIR = ROOT_DIR + "/file/";
	public static String saveMail(String content, String ip) {
		String mail_id = RandomName.getRandomName(ip).toString();
		ByteArrayInputStream bais = new ByteArrayInputStream(content.getBytes());
		if(HdfsOp.createFile(MAIL_DIR + mail_id, bais, content.length())) {
			return mail_id;
		}
		return null;
	}
	
	public static String saveFile(InputStream in, int length, String ip) {
		String file_id = RandomName.getRandomName(ip).toString();
		if(HdfsOp.createFile(FILE_DIR + file_id, in, length)) {
			return file_id;
		} 
		return null;
	}
	public static String readMail(String mail_id) {
		byte [] buffer = HdfsOp.readFile(MAIL_DIR + mail_id);
		if(buffer == null) {
			return null;
		} else {
			return new String(buffer);
		}
	}
	
	public static boolean readFile(OutputStream out, String file_id) {
		if(HdfsOp.readFile(out, FILE_DIR + file_id)) {
			return true;
		}
		return false;
	}
	public static boolean deleteFile(String file_id) {
		if(HdfsOp.delete(FILE_DIR + file_id)) {
			return true;
		}
		return false;
	}
	public static boolean deleteMail(String mail_id) {
		if(HdfsOp.delete(MAIL_DIR + mail_id)) {
			return true;
		}
		return false;
	}
}
