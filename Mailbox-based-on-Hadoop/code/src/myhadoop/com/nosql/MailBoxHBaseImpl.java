package myhadoop.com.nosql;


import java.util.ArrayList;
import java.util.Iterator;

import myhadoop.com.nosql.vo.Complex;
import myhadoop.com.nosql.vo.File;
import myhadoop.com.nosql.vo.Mail;
import myhadoop.com.nosql.vo.Share;
import myhadoop.com.nosql.vo.User;
import myhadoop.com.util.RandomName;

import org.apache.hadoop.hbase.client.Result;
import org.apache.hadoop.hbase.client.ResultScanner;
import org.apache.hadoop.hbase.util.Bytes;


public class MailBoxHBaseImpl {
	public static void dropAllTable() {
		MailBoxHBaseOp.drop("user_table");
		MailBoxHBaseOp.drop("friend_table");
		MailBoxHBaseOp.drop("mail_table");
		MailBoxHBaseOp.drop("file_table");
		MailBoxHBaseOp.drop("share_table");
		MailBoxHBaseOp.drop("complex_table");
		
	}
	public static void createAllTable() {
		MailBoxHBaseOp.create("user_table", new String[]{"user"});
		MailBoxHBaseOp.create("friend_table", new String[]{"user"});
		MailBoxHBaseOp.create("mail_table", new String[]{"user", "mail", "file"});
		MailBoxHBaseOp.create("file_table", new String[]{"user", "file"});
		MailBoxHBaseOp.create("share_table", new String[]{"file", "user"});
		MailBoxHBaseOp.create("complex_table", new String[]{"complex"});
	}
	public static boolean registerComplex(Complex complex) {

		String id = RandomName.getRandomName("0.0.0.0").toString();
		MailBoxHBaseOp.put("complex_table", id,
				"complex", "skin", complex.getSkin());
		MailBoxHBaseOp.put("complex_table", id,
				"complex", "color", complex.getColor());
		MailBoxHBaseOp.put("complex_table", id,
				"complex", "hover_color", complex.getHoverColor());
				
		return true;
	}
	public static Complex getComplex(String complex_id) {
		Result r = MailBoxHBaseOp.get("complex_table", complex_id);
		String skin = new String(r.getValue(Bytes.toBytes("complex"), Bytes.toBytes("skin")));
		String color = new String(r.getValue(Bytes.toBytes("complex"), Bytes.toBytes("color")));
		String hover_color = new String(r.getValue(Bytes.toBytes("complex"), Bytes.toBytes("hover_color")));
		Complex complex = new Complex();
		complex.setSkin(skin);
		complex.setColor(color);
		complex.setHoverColor(hover_color);
		return complex;
	}
	public static ArrayList<Complex> getAllComplex() {
		ArrayList<Complex> alComplex = null;
		ResultScanner rs = MailBoxHBaseOp
				.scan("complex_table", null, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			while(it.hasNext()) {
				if(alComplex == null) {
					alComplex = new ArrayList<Complex>();
				}
				Result r = it.next();
				Complex complex = new Complex();
				complex.setColor(new String(r.getValue("complex".getBytes()
						, "color".getBytes())));
				complex.setSkin(new String(r.getValue("complex".getBytes()
						, "skin".getBytes())));
				complex.setHoverColor(new String(r.getValue("complex".getBytes()
						, "hover_color".getBytes())));
				complex.setComplexId(new String(r.getRow()));
				alComplex.add(complex);
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return alComplex;
	}
	public static boolean registerUser(User user, String ip) {
		if(findIdByMailAddress(user.getMailAddress()) != null) {
			return false;
		}
		String id = RandomName.getRandomName(ip).toString();
		MailBoxHBaseOp.put("user_table", id,
				"user", "mail_address", user.getMailAddress());
		MailBoxHBaseOp.put("user_table", id,
				"user", "password", user.getPassword());
		MailBoxHBaseOp.put("user_table", id,
				"user", "nickname", user.getNickname());
		MailBoxHBaseOp.put("user_table", id,
				"user", "sex", user.getSex());
		MailBoxHBaseOp.put("user_table", id,
				"user", "date_month", user.getDateMonth());
		MailBoxHBaseOp.put("user_table", id,
				"user", "date_day", user.getDateDay());
		MailBoxHBaseOp.put("user_table", id,
				"user", "phone", user.getPhone());
				
		return true;
	}
	public static boolean setComplexId(String user_id, String complex_id) {
		return MailBoxHBaseOp.put("user_table", user_id,
				"user", "complex_id", complex_id);
	}
	public static String getComplexId(String user_id) {
		
		byte[] complex_id = MailBoxHBaseOp.get("user_table", user_id, "user", "complex_id");
		if(complex_id != null) {
			return new String(complex_id);
		} else {
			return null;
		}
	}
	public static String findIdByMailAddress(String mail_address) {
		String id = null;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"user", "mail_address", mail_address});
		ResultScanner rs = MailBoxHBaseOp
				.scan("user_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			Result r = null;
			if(it.hasNext() && !(r=it.next()).isEmpty()) {
				id = new String(r.getRow());
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return id;
	}
	public static User findUserByMailAddress(String mail_address) {
		User user = null;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"user", "mail_address", mail_address});
		ResultScanner rs = MailBoxHBaseOp
				.scan("user_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			Result r = null;
			if(it.hasNext() && !(r=it.next()).isEmpty()) {
					user = new User();
					user.setMailAddress(new String(r.getValue(
							"user".getBytes(), "mail_address".getBytes())));
					user.setPassword(new String(r.getValue(
							"user".getBytes(), "password".getBytes())));
					user.setNickname(new String(r.getValue(
							"user".getBytes(), "nickname".getBytes())));
					user.setSex(new String(r.getValue(
							"user".getBytes(), "sex".getBytes())));
					user.setDateMonth(new String(r.getValue(
							"user".getBytes(), "date_month".getBytes())));
					user.setDateDay(new String(r.getValue(
							"user".getBytes(), "date_day".getBytes())));
					user.setPhone(new String(r.getValue(
							"user".getBytes(), "phone".getBytes())));
					
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return user;
	}
	public static String findMailAddressById(String id) {
		byte[] mail_address = MailBoxHBaseOp.get("user_table", id, "user", "mail_address");
		if(mail_address == null) {
			return null;
		} else {
			return new String(mail_address);
		}
	}
	public static User findUserById(String user_id) {
		Result r = MailBoxHBaseOp.get("user_table", user_id);
		String mail_address = new String(r.getValue(Bytes.toBytes("user"), Bytes.toBytes("mail_address")));
		String password = new String(r.getValue(Bytes.toBytes("user"), Bytes.toBytes("password")));
		String nickname = new String(r.getValue(Bytes.toBytes("user"), Bytes.toBytes("nickname")));
		String sex = new String(r.getValue(Bytes.toBytes("user"), Bytes.toBytes("nickname")));
		String date_month = new String(r.getValue(Bytes.toBytes("user"), Bytes.toBytes("date_month")));
		String date_day = new String(r.getValue(Bytes.toBytes("user"), Bytes.toBytes("date_day")));
		String phone = new String(r.getValue(Bytes.toBytes("user"), Bytes.toBytes("phone")));
		
		User user = new User();
		user.setMailAddress(mail_address);
		user.setPassword(password);
		user.setNickname(nickname);
		user.setSex(sex);
		user.setDateMonth(date_month);
		user.setDateDay(date_day);
		user.setPhone(phone);
		return user;
	}
	public static boolean isFriends(String user_id, String friend_id) {
		boolean flag = false;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"user", "user_id", user_id});
		columnQualifiers.add(new String[]{"user", "friend_id", friend_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("friend_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			if(it.hasNext() && !it.next().isEmpty()) {
				flag = true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return flag;
	}
	public static boolean registerFriend(String user_id, String friend_id, String ip) {
		if(isFriends(user_id, friend_id)) {
			return false;
		}
		String id = RandomName.getRandomName(ip).toString();
		MailBoxHBaseOp.put("friend_table", id,
				"user", "user_id", user_id);
		MailBoxHBaseOp.put("friend_table", id,
				"user", "friend_id", friend_id);
		return true;
	}
	public static ArrayList<String> findFriendByUserId(String user_id) {
		ArrayList<String> alFriend = null;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"user", "user_id", user_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("friend_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			while(it.hasNext()) {
				if(alFriend == null) {
					alFriend = new ArrayList<String>();
				}
				Result r = it.next();
				alFriend.add(new String(r.getValue("user".getBytes()
						, "friend_id".getBytes())));
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return alFriend;
	}
	public static boolean registerMail(Mail mail, String ip) {
		String id = RandomName.getRandomName(ip).toString();
		MailBoxHBaseOp.put("mail_table", id,
				"user", "to_id", mail.getToId());
		MailBoxHBaseOp.put("mail_table", id,
				"user", "from_id", mail.getFromId());
		
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "subject", mail.getSubject());
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "mail_id", mail.getMailId());
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "to_position", mail.getToPosition());
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "from_position", mail.getFromPosition());
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "time", mail.getTime());
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "to_star", mail.getToStar());
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "from_star", mail.getFromStar());
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "to_read", mail.getToRead());
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "from_read", mail.getFromRead());
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "appendix", mail.getAppendix());
		
		MailBoxHBaseOp.put("mail_table", id,
				"file", "file_id", mail.getFileId());
		
		return true;
	}
	public static String findIdByMailId(String mail_id) {
		String id = null;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"mail", "mail_id", mail_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("mail_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			Result r = null;
			if(it.hasNext() && !(r=it.next()).isEmpty()) {
				id = new String(r.getRow());
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return id;
	}
	public static boolean changeMailFromPosition(String mail_id, String from_position) {
		String id = findIdByMailId(mail_id);
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "from_position", from_position);
		return true;
	}
	public static boolean changeMailToPosition(String mail_id, String to_position) {
		String id = findIdByMailId(mail_id);
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "to_position", to_position);
		return true;
	}
	public static boolean changeMailFromRead(String mail_id, String from_read) {
		String id = findIdByMailId(mail_id);
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "from_read", from_read);
		return true;
	}
	public static boolean changeMailToRead(String mail_id, String to_read) {
		String id = findIdByMailId(mail_id);
		System.out.println(id);
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "to_read", to_read);
		return true;
	}
	public static boolean changeMailFromStar(String mail_id, String from_star) {
		String id = findIdByMailId(mail_id);
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "from_star", from_star);
		return true;
	}
	public static boolean changeMailToStar(String mail_id, String to_star) {
		String id = findIdByMailId(mail_id);
		MailBoxHBaseOp.put("mail_table", id,
				"mail", "to_star", to_star);
		return true;
	}
	public static ArrayList<Mail> findMailByFromId(String from_id, String from_position) {
		ArrayList<Mail> alMail = null;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"user", "from_id", from_id});
		columnQualifiers.add(new String[]{"mail", "from_position", from_position});
		ResultScanner rs = MailBoxHBaseOp
				.scan("mail_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			while(it.hasNext()) {
				if(alMail == null) {
					alMail = new ArrayList<Mail>();
				}
				Result r = it.next();
				Mail mail = new Mail();
				mail.setFromId(new String(r.getValue("user".getBytes(),
						"from_id".getBytes())));
				mail.setToId(new String(r.getValue("user".getBytes(),
						"to_id".getBytes())));
				
				mail.setSubject(new String(r.getValue("mail".getBytes(),
						"subject".getBytes())));
				mail.setMailId(new String(r.getValue("mail".getBytes(),
						"mail_id".getBytes())));
				mail.setToPosition(new String(r.getValue("mail".getBytes(),
						"to_position".getBytes())));
				mail.setFromPosition(new String(r.getValue("mail".getBytes(),
						"from_position".getBytes())));				
				mail.setTime(new String(r.getValue("mail".getBytes(),
						"time".getBytes())));
				mail.setToStar(new String(r.getValue("mail".getBytes(),
						"to_star".getBytes())));
				mail.setFromStar(new String(r.getValue("mail".getBytes(),
						"from_star".getBytes())));
				mail.setToRead(new String(r.getValue("mail".getBytes(),
						"to_read".getBytes())));
				mail.setFromRead(new String(r.getValue("mail".getBytes(),
						"from_read".getBytes())));
				mail.setAppendix(new String(r.getValue("mail".getBytes(),
						"appendix".getBytes())));
				
				mail.setFileId(new String(r.getValue("file".getBytes(),
						"file_id".getBytes())));
				alMail.add(mail);
				
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return alMail;
	}


	public static ArrayList<Mail> findMailByToId(String to_id, String to_position) {
		ArrayList<Mail> alMail = null;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"user", "to_id", to_id});
		columnQualifiers.add(new String[]{"mail", "to_position", to_position});
		ResultScanner rs = MailBoxHBaseOp
				.scan("mail_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			while(it.hasNext()) {
				if(alMail == null) {
					alMail = new ArrayList<Mail>();
				}
				Result r = it.next();
				Mail mail = new Mail();
				mail.setFromId(new String(r.getValue("user".getBytes(),
						"from_id".getBytes())));
				mail.setToId(new String(r.getValue("user".getBytes(),
						"to_id".getBytes())));
				
				mail.setSubject(new String(r.getValue("mail".getBytes(),
						"subject".getBytes())));
				mail.setMailId(new String(r.getValue("mail".getBytes(),
						"mail_id".getBytes())));
				mail.setToPosition(new String(r.getValue("mail".getBytes(),
						"to_position".getBytes())));
				mail.setFromPosition(new String(r.getValue("mail".getBytes(),
						"from_position".getBytes())));				
				mail.setTime(new String(r.getValue("mail".getBytes(),
						"time".getBytes())));
				mail.setToStar(new String(r.getValue("mail".getBytes(),
						"to_star".getBytes())));
				mail.setFromStar(new String(r.getValue("mail".getBytes(),
						"from_star".getBytes())));
				mail.setToRead(new String(r.getValue("mail".getBytes(),
						"to_read".getBytes())));
				mail.setFromRead(new String(r.getValue("mail".getBytes(),
						"from_read".getBytes())));
				mail.setAppendix(new String(r.getValue("mail".getBytes(),
						"appendix".getBytes())));
				
				mail.setFileId(new String(r.getValue("file".getBytes(),
						"file_id".getBytes())));

				alMail.add(mail);
				
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return alMail;
	}
	public static boolean deleteMail(String mail_id) {
		boolean flag = false;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"mail", "mail_id", mail_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("mail_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			Result r = null;
			if(it.hasNext() && !(r=it.next()).isEmpty()) {
				String row_id = new String(r.getRow());
				MailBoxHBaseOp.delete("mail_table", row_id);
				flag = true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return flag;
	}
	public static boolean deleteFriend(String user_id, String friend_id) {
		boolean flag = false;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"user", "user_id", user_id});
		columnQualifiers.add(new String[]{"user", "friend_id", friend_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("friend_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			Result r = null;
			if(it.hasNext() && !(r=it.next()).isEmpty()) {
				String row_id = new String(r.getRow());
				MailBoxHBaseOp.delete("friend_table", row_id);
				flag = true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return flag;
	}
	public static boolean registerFile(File file, String ip) {
		String id = RandomName.getRandomName(ip).toString();
		MailBoxHBaseOp.put("file_table", id,
				"user", "user_id", file.getUserId());
		MailBoxHBaseOp.put("file_table", id,
				"file", "file_name", file.getFileName());
		MailBoxHBaseOp.put("file_table", id,
				"file", "file_id", file.getFileId());
		MailBoxHBaseOp.put("file_table", id,
				"file", "file_size", ""+file.getFileSize());
		MailBoxHBaseOp.put("file_table", id,
				"file", "time", ""+file.getTime());
		return true;
	}
	public static ArrayList<File> findFileByUserId(String user_id) {
		ArrayList<File> alFile = null;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"user", "user_id", user_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("file_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			while(it.hasNext()) {
				if(alFile == null) {
					alFile = new ArrayList<File>();
				}
				Result r = it.next();
				File file = new File();
				file.setUserId(user_id);
				file.setFileName(new String(r.getValue("file".getBytes()
						, "file_name".getBytes())));
				file.setFileId(new String(r.getValue("file".getBytes()
						, "file_id".getBytes())));
				file.setFileSize(Long.parseLong(new String(r.getValue("file".getBytes()
						, "file_size".getBytes()))));
				file.setTime(new String(r.getValue("file".getBytes()
						, "time".getBytes())));
				alFile.add(file);
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return alFile;
	}
	public static File findFileByFileId(String file_id) {
		File file = null;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"file", "file_id", file_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("file_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			if(it.hasNext()) {
				Result r = it.next();
				file = new File();
				file.setUserId(new String(r.getValue("user".getBytes()
						, "user_id".getBytes())));
				file.setFileName(new String(r.getValue("file".getBytes()
						, "file_name".getBytes())));
				file.setFileId(new String(r.getValue("file".getBytes()
						, "file_id".getBytes())));
				file.setFileSize(Long.parseLong(new String(r.getValue("file".getBytes()
						, "file_size".getBytes()))));
				file.setTime(new String(r.getValue("file".getBytes()
						, "time".getBytes())));
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return file;
	}
	public static boolean deleteFile(String file_id) {
		boolean flag = false;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"file", "file_id", file_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("file_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			Result r = null;
			if(it.hasNext() && !(r=it.next()).isEmpty()) {
				String id = new String(r.getRow());
				MailBoxHBaseOp.delete("file_table", id);
				flag = true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return flag;
	}
	public static boolean isShared(String file_id) {
		boolean flag = false;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"file", "file_id", file_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("share_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			if(it.hasNext() && !it.next().isEmpty()) {
				flag = true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return flag;
	}
	public static boolean isShared(String file_id, String user_id) {
		boolean flag = false;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"file", "file_id", file_id});
		columnQualifiers.add(new String[]{"user", "user_id", user_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("share_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			if(it.hasNext() && !it.next().isEmpty()) {
				flag = true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return flag;
	}
	public static boolean registerShare(Share share, String ip) {
		if(isShared(share.getFileId(), share.getUserId())) {
			return false;
		}
		String id = RandomName.getRandomName(ip).toString();
		MailBoxHBaseOp.put("share_table", id,
				"file", "file_id", share.getFileId());
		MailBoxHBaseOp.put("share_table", id,
				"file", "time", share.getTime());
		MailBoxHBaseOp.put("share_table", id,
				"user", "user_id", share.getUserId());
		
		return true;
	}
	public static boolean deleteShare(String file_id, String user_id) {
		boolean flag = false;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"file", "file_id", file_id});
		columnQualifiers.add(new String[]{"user", "user_id", user_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("share_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			Result r = null;
			if(it.hasNext() && !(r = it.next()).isEmpty()) {
				String row_id = new String(r.getRow());
				MailBoxHBaseOp.delete("share_table", row_id);
				flag = true;
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return flag;
	}
	public static ArrayList<Share> findShareByUserId(String user_id) {
		ArrayList<Share> alShare = null;
		ArrayList<String[]> columnQualifiers = new ArrayList<String[]>();
		columnQualifiers.add(new String[]{"user", "user_id", user_id});
		ResultScanner rs = MailBoxHBaseOp
				.scan("share_table", columnQualifiers, new ArrayList<String[]>());
		try {
			Iterator<Result> it = rs.iterator();
			while(it.hasNext()) {
				if(alShare == null) {
					alShare = new ArrayList<Share>();
				}
				Result r = it.next();
				String file_id = new String(r.getValue("file".getBytes()
						, "file_id".getBytes()));
				String time = new String(r.getValue("file".getBytes()
						, "time".getBytes()));
				Share share = new Share();
				share.setFileId(file_id);
				share.setUserId(user_id);
				share.setTime(time);
				alShare.add(share);
			}
		} catch (Exception e) {
			e.printStackTrace();
		} finally {
			rs.close();
		}
		return alShare;
	}
}

