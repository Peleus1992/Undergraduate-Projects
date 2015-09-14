package myhadoop.com.nosql.vo;

public class File {
	private String user_id;
	private String file_name;
	private String file_id;
	private long file_size;
	private String time;
	public void setUserId(String user_id) {
		this.user_id = user_id;
	}
	public void setFileName(String file_name) {
		this.file_name = file_name;
	}
	public void setFileId(String file_id) {
		this.file_id = file_id;
	}
	public void setFileSize(long file_size) {
		this.file_size = file_size;
	}
	public void setTime(String time) {
		this.time = time;
	}
	
	public String getUserId() {
		return user_id;
	}
	public String getFileName() {
		return file_name;
	}
	public String getFileId() {
		return file_id;
	}
	public long getFileSize() {
		return file_size;
	}
	public String getTime() {
		return time;
	}
	public String toString() {
		return user_id
				+ ":" + file_name
				+ ":" + file_id
				+ ":" + file_size
				+ ":" + time;
	}
}
