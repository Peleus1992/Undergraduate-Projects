package myhadoop.com.test;

import java.util.Iterator;

import myhadoop.com.nosql.MailBoxHBaseImpl;
import myhadoop.com.nosql.MailBoxHBaseOp;
import myhadoop.com.nosql.vo.Complex;
import myhadoop.com.nosql.vo.Mail;



public class Main {
	public static void addComplex(){
//		Complex complex = new Complex();
//		complex.setSkin("image/blue.jpg");
//		complex.setColor("#0000ff");
//		complex.setHoverColor("rgba(0, 0, 255, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
//		complex.setSkin("image/sao.jpg");
//		complex.setColor("#0000ff");
//		complex.setHoverColor("rgba(0, 0, 255, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
//		complex.setSkin("image/tulip.gif");
//		complex.setColor("#ff0000");
//		complex.setHoverColor("rgba(255, 0, 0, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
//		complex.setSkin("image/christina.jpg");
//		complex.setColor("#00ff00");
//		complex.setHoverColor("rgba(255, 0, 0, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
//		complex.setSkin("image/crown.jpg");
//		complex.setColor("#ff00ff");
//		complex.setHoverColor("rgba(255, 0, 255, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
//		complex.setSkin("image/danwanlunpo.jpg");
//		complex.setColor("#ff0000");
//		complex.setHoverColor("rgba(255, 0, 50, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
//		complex.setSkin("image/headmagic.gif");
//		complex.setColor("#555555");
//		complex.setHoverColor("rgba(55, 55, 55, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
//		complex.setSkin("image/snow.jpg");
//		complex.setColor("#2222ff");
//		complex.setHoverColor("rgba(100, 100, 255, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
//		complex.setSkin("image/zolo.jpg");
//		complex.setColor("#00ff00");
//		complex.setHoverColor("rgba(0, 255, 0, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
		
	}
	public static void main(String args[]) throws Exception {
//		addComplex();
//
//		User user1 = new User();
//		user1.setUsername("user1");
//		user1.setPassword("123");
//		user1.setCreationDate("2014-02-11");
//		User user2 = new User();
//		user2.setUsername("user2");
//		user2.setPassword("234");
//		user2.setCreationDate("2014-02-12");
//		User user3 = new User();
//		user3.setUsername("user3");
//		user3.setPassword("345");
//		user3.setCreationDate("2014-02-13");
//		System.out.println(MailBoxHBaseImpl.registerUser(user1, "192.168.100.1"));
//		System.out.println(MailBoxHBaseImpl.registerUser(user2, "192.168.100.2"));
//		System.out.println(MailBoxHBaseImpl.registerUser(user3, "192.168.100.3"));
//		System.out.println(MailBoxHBaseImpl.findUserByUsername("user4"));
		
		
//		String user_id = MailBoxHBaseImpl.findRowIdByUsername("user1");
//		String friend_id = MailBoxHBaseImpl.findRowIdByUsername("user2");
//		
//		System.out.println(MailBoxHBaseImpl.registerFriend(user_id, friend_id, "202.202.202.0"));
//		Mail mail1 = new Mail();
//		mail1.setSenderId("1");
//		mail1.setReceiverId("2");
//		mail1.setTitle("1->2");
//		mail1.setMailId("mailid2");
//		mail1.setSendingDate("2015-02-02");
//		mail1.setPhoneId("01");
//		mail1.setPhoneName("1->2");
////		MailBoxHBaseImpl.registerMail(mail1, "192.168.1.1");
//		ArrayList<Mail> alMail1 = MailBoxHBaseImpl.findMailBySenderId("1");
//		if(alMail1 != null) {
//			for(Mail mail : alMail1) {
//				System.out.println(mail);
//			}
//		} else {
//			System.out.println("empty");
//		}
//		String user1 = MailBoxHBaseImpl.findRowIdByUsername("user1");
//		String user2 = MailBoxHBaseImpl.findRowIdByUsername("user2");
//		String user3 = MailBoxHBaseImpl.findRowIdByUsername("user3");
//		
//		System.out.println(MailBoxHBaseImpl.registerFriend(user1, user2, "202.202.202.0"));
//		System.out.println(MailBoxHBaseImpl.registerFriend(user2, user1, "202.202.202.0"));
//		System.out.println(MailBoxHBaseImpl.registerFriend(user2, user3, "202.202.202.0"));
//		
//		ArrayList<String> alFriend = MailBoxHBaseImpl.findFriendByUserId(user2);
//		if(alFriend != null) {
//			for(String friend : alFriend) {
//				System.out.println(friend);
//			}
//		} else {
//			System.out.println("empty");
//		}
//		System.out.println(MailBoxHBaseImpl.deleteFriend(user1, user2));
//		System.out.println(MailBoxHBaseImpl.deleteFriend(user2, user1));
//		File file1 = new File();
//		file1.setUserId("1");
//		file1.setFileId("01");
//		file1.setFileName("elo");
//		file1.setFileSize(1024);
//		file1.setBlockNum(1);
//		File file2 = new File();
//		file2.setUserId("1");
//		file2.setFileId("02");
//		file2.setFileName("huang");
//		file2.setFileSize(1024*1024*64);
//		file2.setBlockNum(8);
//		File file3 = new File();
//		file3.setUserId("2");
//		file3.setFileId("01");
//		file3.setFileName("elo1");
//		file3.setFileSize(1024);
//		file3.setBlockNum(1);
//		MailBoxHBaseImpl.registerFile(file1, "192.168.1.1");
//		MailBoxHBaseImpl.registerFile(file2, "192.168.1.1");
//		MailBoxHBaseImpl.registerFile(file3, "192.168.1.2");
//		System.out.println(MailBoxHBaseImpl.deleteFile("02"));
//		ArrayList<File> alFile = MailBoxHBaseImpl.findFileByUserId("2");
//		if(alFile != null) {
//			for(File file : alFile) {
//				System.out.println(file);
//			}
//		} else {
//			System.out.println("empty");
//		}
//		System.out.println(MailBoxHBaseImpl.deleteShare("01", "1"));
//		System.out.println(MailBoxHBaseImpl.registerShare("01", "1", "192.1.1.1"));
//		System.out.println(MailBoxHBaseImpl.registerShare("01", "2", "192.1.1.1"));
//		System.out.println(MailBoxHBaseImpl.registerShare("01", "2", "192.2.1.1"));
//		MailBoxHBaseImpl.createAllTable();
//		System.out.println(HdfsOp.mkdir("/test"));
//		File file = new File("/home/grid/1.pdf");
//		long a =System.currentTimeMillis();
//		HdfsOp.createFile("/test/1.pdf", new FileInputStream(file), file.length());
//		long b =System.currentTimeMillis();
//		System.out.println(b-a);
//		long a =System.currentTimeMillis();
//		
//		FileOutputStream fos = new FileOutputStream("/home/grid/2.pdf");
//		System.out.println(HdfsOp.readFile(fos, "/test/1.pdf"));
//		long b =System.currentTimeMillis();
//		System.out.println(b-a);
/************************************************************************************/
//		MailBoxHBaseImpl.dropAllTable();
//		MailBoxHBaseImpl.createAllTable();
		MailBoxHBaseOp.drop("mail_table");
		MailBoxHBaseOp.create("mail_table", new String[]{"user", "mail", "file"});
//		Iterator<Share> it = MailBoxHBaseImpl.findShareByUserId("19216810010020150421085906961279").iterator();
//		System.out.println(it.next().getFileId());
//		Iterator<Mail> it = MailBoxHBaseImpl.findMailByToId("19216810010020150421085906961279", "inbox").iterator();
//		System.out.println(it.next().getTime());
//		Iterator<Mail> it = MailBoxHBaseImpl.findMailByFromId("19216810010020150421085906961279", "drafts").iterator();
//		System.out.println(it.next().next().getSubject());
//		System.out.println(MailBoxHBaseImpl.deleteMail("19216810010020150421160727075330"));
//		Complex complex = new Complex();
//		complex.setSkin("image/sao.jpg");
//		complex.setColor("#0000ff");
//		complex.setHoverColor("rgba(0, 0, 255, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
//		complex.setSkin("image/tulip.gif");
//		complex.setColor("#ff0000");
//		complex.setHoverColor("rgba(255, 0, 0, 0.3)");
//		MailBoxHBaseImpl.registerComplex(complex);
		
/****/
//		String user_id = MailBoxHBaseImpl.findRowIdByUsername("Derek@d.com");
//		ArrayList<Mail> alMail = null;
//		ArrayList<String[]> alStr = new ArrayList<String[]>();
//		try {
//			alMail = MailBoxHBaseImpl.findMailByReceiverId(user_id);
//			if(alMail == null) {
//				alMail = new ArrayList<Mail>();
//			}
//			for(Mail mail : alMail) {
//				String sender_name = MailBoxHBaseImpl.findUsernameByRowId(mail.getSenderId());
//				String strArr[] = new String[]{
//						mail.getTitle(),
//						sender_name,
//						mail.getSendingDate(),
//						mail.getPhoneName(),
//						mail.getMailId(),
//						mail.getPhoneId()
//						};
//				alStr.add(strArr);
//				System.out.println("h");
//			}
//		} catch (Exception e) {
//			e.printStackTrace();
//		}
//		System.out.println(HdfsImpl.readMail("19216810020120150402205835103554").length());
	} 
//	public static void test1() throws Exception {
//		User userArr[] = new User[3];
//		userArr[0] = new User();
//		userArr[0].setUsername("Derek@d.com");
//		userArr[0].setPassword("123456");
//		userArr[0].setCreationDate(new Date());
//		userArr[1] = new User();
//		userArr[1].setUsername("Christina@d.com");
//		userArr[1].setPassword("11223344");
//		userArr[1].setCreationDate(new Date());
//		userArr[2] = new User();
//		userArr[2].setUsername("HashiriNio@d.com");
//		userArr[2].setPassword("654321");
//		userArr[2].setCreationDate(new Date());
//		for(User user : userArr) {
//			DAOFactory.getMailBoxDAOInstance().register(user);
//		}
//	}
//	public static void test2() throws Exception {
//		String usernameArr[] = {"Derek@d.com", "Check@d.com"};
//		for(String username : usernameArr) {
//			User user = null;
//			if((user = DAOFactory.getMailBoxDAOInstance().findByUsername(username)) != null) {
//				System.out.println(user.getId() + " " + user.getUsername() + " " + user.getPassword() + " " + user.getCreationDate());
//			} else {
//				System.out.println(username + " not found.");
//			}
//		}
//	}
//	public static void test3() throws Exception {
//		Mail mail1 = new Mail();
//		mail1.setTitle("mail1");
//		mail1.setContent(new FileInputStream("/root/tmp/mail/mail1"));
//		mail1.setSenderId(DAOFactory.getMailBoxDAOInstance().findByUsername("Christina@d.com").getId());
//		mail1.setReceiverId(DAOFactory.getMailBoxDAOInstance().findByUsername("Derek@d.com").getId());
//		mail1.setSendingDate(new Date());
//		DAOFactory.getMailBoxDAOInstance().registerMail(mail1);
//		Mail mail2 = new Mail();
//		mail2.setTitle("mail2");
//		mail2.setContent(new FileInputStream("/root/tmp/mail/mail2"));
//		mail2.setSenderId(DAOFactory.getMailBoxDAOInstance().findByUsername("HashiriNio@d.com").getId());
//		mail2.setReceiverId(DAOFactory.getMailBoxDAOInstance().findByUsername("Derek@d.com").getId());
//		mail2.setSendingDate(new Date());
//		DAOFactory.getMailBoxDAOInstance().registerMail(mail2);
//		Mail mail3 = new Mail();
//		mail3.setTitle("mail3");
//		mail3.setContent(new FileInputStream("/root/tmp/mail/mail3"));
//		mail3.setSenderId(DAOFactory.getMailBoxDAOInstance().findByUsername("Derek@d.com").getId());
//		mail3.setReceiverId(DAOFactory.getMailBoxDAOInstance().findByUsername("Christina@d.com").getId());
//		mail3.setSendingDate(new Date());
//		DAOFactory.getMailBoxDAOInstance().registerMail(mail3);
//		Mail mail4 = new Mail();
//		mail4.setTitle("mail4");
//		mail4.setContent(new FileInputStream("/root/tmp/mail/mail4"));
//		mail4.setSenderId(DAOFactory.getMailBoxDAOInstance().findByUsername("Derek@d.com").getId());
//		mail4.setReceiverId(DAOFactory.getMailBoxDAOInstance().findByUsername("HashiriNio@d.com").getId());
//		mail4.setSendingDate(new Date());
//		DAOFactory.getMailBoxDAOInstance().registerMail(mail4);
//		
//	}
//	public static void test4() throws Exception {
//		ArrayList<Mail> alMail1 = DAOFactory.getMailBoxDAOInstance().findMailBySenderId(DAOFactory.getMailBoxDAOInstance().findByUsername("Derek@d.com").getId());
//		ArrayList<Mail> alMail2 = DAOFactory.getMailBoxDAOInstance().findMailByReceiverId(DAOFactory.getMailBoxDAOInstance().findByUsername("Derek@d.com").getId());
//		Iterator<Mail> it = alMail1.iterator();
//		System.out.println("Sended: ");
//		while(it.hasNext()) {
//			Mail mail = it.next();
//			System.out.println(mail.getId() + " "
//			+ mail.getTitle() + " "
//			+ mail.getSenderId() + " "
//			+ mail.getReceiverId() + " "
//			+ mail.getSendingDate());
//		}
//		it = alMail2.iterator();
//		System.out.println("Receive:");
//		while(it.hasNext()) {
//			Mail mail = it.next();
//			System.out.println(mail.getId() + " "
//			+ mail.getTitle() + " "
//			+ mail.getSenderId() + " "
//			+ mail.getReceiverId() + " "
//			+ mail.getSendingDate());
//		}
//	}
//	public static void test5() throws Exception {
//		InputStream in = DAOFactory.getMailBoxDAOInstance().findContentByMailId(2);
//		BufferedReader br = new BufferedReader(new InputStreamReader(in));
//		String str = null;
//		while((str = br.readLine()) != null) {
//			System.out.println(str);
//		}
//		in.close();
//	}
	
}
