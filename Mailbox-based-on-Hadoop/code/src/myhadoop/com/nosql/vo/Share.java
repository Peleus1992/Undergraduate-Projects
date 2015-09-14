package myhadoop.com.nosql.vo;

public class Share {
	private String user_id;
	private String file_id;
	private String time;
	public void setUserId(String user_id) {
		this.user_id = user_id;
	}
	public void setFileId(String file_id) {
		this.file_id = file_id;
	}
	public void setTime(String time) {
		this.time = time;
	}
	
	public String getUserId() {
		return user_id;
	}
	public String getFileId() {
		return file_id;
	}
	public String getTime() {
		return time;
	}
}
