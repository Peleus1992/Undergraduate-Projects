package myhadoop.com.nosql.vo;


public class User {
	private String mail_address;
	private String password;
	private String nickname;
	private String sex;
	private String date_month;
	private String date_day;
	private String phone;
	private String complex_id;
	
	public void setMailAddress(String mail_address) {
		this.mail_address = mail_address;
	}
	public void setPassword(String password) {
		this.password = password;
	}
	public void setNickname(String nickname) {
		this.nickname = nickname;
	}
	public void setSex(String sex) {
		this.sex = sex;
	}
	public void setDateMonth(String date_month) {
		this.date_month = date_month;
	}
	public void setDateDay(String date_day) {
		this.date_day = date_day;
	}
	public void setPhone(String phone) {
		this.phone = phone;
	}
	public void setComplexId(String complex_id) {
		this.complex_id = complex_id;
	}
	
	
	
	public String getMailAddress() {
		return mail_address;
	}
	public String getPassword() {
		return password;
	}
	public String getNickname() {
		return nickname;
	}
	public String getSex() {
		return sex;
	}
	public String getDateMonth() {
		return date_month;
	}
	public String getDateDay() {
		return date_day;
	}
	public String getPhone() {
		return phone;
	}
	public String getComplexId(){
		return complex_id;
	}
	public String toString() {
		return mail_address + ":" + password;
	}
}
