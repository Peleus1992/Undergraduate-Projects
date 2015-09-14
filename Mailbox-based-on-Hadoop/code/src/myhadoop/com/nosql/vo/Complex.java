package myhadoop.com.nosql.vo;

public class Complex {
	private String skin;
	private String color;
	private String hover_color;
	private String complex_id;
	public void setComplexId(String complex_id) {
		this.complex_id = complex_id;
	}
	public String getComplexId() {
		return complex_id;
	}
	public void setSkin(String skin) {
		this.skin = skin;
	}
	public void setColor(String color) {
		this.color = color;
	}
	public void setHoverColor(String hover_color) {
		this.hover_color = hover_color;
	}
	
	public String getSkin(){
		return skin;
	}
	public String getColor() {
		return color;
	}
	public String getHoverColor() {
		return hover_color;
	}
 }
