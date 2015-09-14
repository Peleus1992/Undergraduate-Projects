package myhadoop.com.nosql.vo;

import java.io.InputStream;
import java.util.Date;

public class Mail {
	private String from_id;
	private String to_id;
	
	private String subject;
	private String mail_id;
	private String to_position;
	private String from_position;
	private String time;
	private String to_star;
	private String from_star;	
	private String to_read;
	private String from_read;
	private String appendix;

	private String file_id;
	
	public static final String INBOX = "inbox";
	public static final String OUTBOX = "outbox";
	public static final String DRAFTS = "drafts";
	public static final String TRASH = "trash";
	public static final String VOID = "void";
	
	public void setFromId(String from_id) {
		this.from_id = from_id;
	}
	public void setToId(String to_id) {
		this.to_id = to_id;
	}
	
	public void setSubject(String subject) {
		this.subject = subject;
	}
	public void setMailId(String mail_id) {
		this.mail_id = mail_id;
	}
	public void setToPosition(String to_position) {
		this.to_position = to_position;
	}
	public void setFromPosition(String from_position) {
		this.from_position = from_position;
	}
	public void setTime(String time) {
		this.time = time;
	}
	public void setToStar(String to_star) {
		this.to_star = to_star;
	}
	public void setFromStar(String from_star) {
		this.from_star = from_star;
	}
	public void setToRead(String to_read) {
		this.to_read = to_read;
	}
	public void setFromRead(String from_read) {
		this.from_read = from_read;
	}
	public void setAppendix(String appendix) {
		this.appendix = appendix;
	}
	public void setFileId(String file_id) {
		this.file_id = file_id;
	}

	
	public String getFromId() {
		return from_id;
	}
	public String getToId() {
		return to_id;
	}
	
	public String getSubject() {
		return subject;
	}
	public String getMailId() {
		return mail_id;
	}
	public String getToPosition() {
		return to_position;
	}
	public String getFromPosition() {
		return from_position;
	}
	public String getTime() {
		return time;
	}
	public String getToStar() {
		return to_star;
	}
	public String getFromStar() {
		return from_star;
	}
	public String getToRead() {
		return to_read;
	}
	public String getFromRead() {
		return from_read;
	}
	public String getAppendix() {
		return appendix;
	}
	public String getFileId() {
		return file_id;
	}
	public String toString() {
		return from_id
				+ ":" + to_id
				+ ":" + subject
				+ ":" + mail_id
				+ ":" + time
				+ ":" + file_id ;
	}
}
