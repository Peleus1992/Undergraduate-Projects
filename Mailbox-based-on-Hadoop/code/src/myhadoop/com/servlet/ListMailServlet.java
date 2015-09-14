package myhadoop.com.servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Iterator;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import myhadoop.com.nosql.MailBoxHBaseImpl;
import myhadoop.com.nosql.vo.Mail;
import myhadoop.com.nosql.vo.User;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

public class ListMailServlet extends HttpServlet {

	/**
	 * Constructor of the object.
	 */
	public ListMailServlet() {
		super();
	}

	/**
	 * Destruction of the servlet. <br>
	 */
	public void destroy() {
		super.destroy(); // Just puts "destroy" string in log
		// Put your code here
	}

	/**
	 * The doGet method of the servlet. <br>
	 *
	 * This method is called when a form has its tag value method equals to get.
	 * 
	 * @param request the request send by the client to the server
	 * @param response the response send by the server to the client
	 * @throws ServletException if an error occurred
	 * @throws IOException if an error occurred
	 */
	public void doGet(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {

		boolean flag = false;
		String user_id = (String)request.getSession().getAttribute("id");
		String from_position = request.getParameter("from_position");
		String to_position = request.getParameter("to_position");
		ArrayList<Mail> alMail = null;
		JSONArray jArr = new JSONArray();
		if(to_position != null) {
//			System.out.println(to_position + user_id);
			alMail = MailBoxHBaseImpl.findMailByToId(user_id, to_position);
			if(alMail != null) {
				Iterator<Mail> it = alMail.iterator();
				while(it.hasNext()) {
					/*mail_list[i].read, mail_list[i].file
					, mail_list[i].from, mail_list[i].subject, mail_list[i].time
					, mail_list[i].star, mail_list[i].mail_id;*/
					Mail mail = it.next();
					String read = mail.getToRead();
					String file = mail.getAppendix();
					String file_id = mail.getFileId();
					User from_user = MailBoxHBaseImpl.findUserById(mail.getFromId());
					String from = from_user.getMailAddress();
					String from_nickname = from_user.getNickname();
					String to = (String)request.getSession().getAttribute("mail_address");
					String to_nickname = (String)request.getSession().getAttribute("nickname");
					String subject = mail.getSubject();
					String time = mail.getTime();
					String star = mail.getToStar();
					String mail_id = mail.getMailId();
					JSONObject jObj = new JSONObject();
					jObj.put("read", read);
					jObj.put("file", file);
					jObj.put("file_id", file_id);
					if("true".equals(file))
						jObj.put("file_name", MailBoxHBaseImpl.findFileByFileId(file_id).getFileName());
					else
						jObj.put("file_name", "");
					jObj.put("from", from);
					jObj.put("from_nickname", from_nickname);
					jObj.put("to", to);
					jObj.put("to_nickname", to_nickname);
					jObj.put("subject", subject);
					jObj.put("time", time);
					jObj.put("star", star);				
					jObj.put("mail_id", mail_id);
					jArr.add(jObj);
				}
				
			} else {
				alMail = new ArrayList<Mail>();
			}
			
			flag = true;
		}
		if(from_position != null) {
			alMail = MailBoxHBaseImpl.findMailByFromId(user_id, from_position);
			if(alMail != null) {
				Iterator<Mail> it = alMail.iterator();
				while(it.hasNext()) {
					/*mail_list[i].read, mail_list[i].file
					, mail_list[i].from, mail_list[i].subject, mail_list[i].time
					, mail_list[i].star, mail_list[i].mail_id;*/
					
					Mail mail = it.next();
					String read = mail.getFromRead();
					String file = mail.getAppendix();
					String file_id = mail.getFileId();
					User to_user = null;
					String to = null;
					String to_nickname = null;
					if(from_position.equals(Mail.DRAFTS)) {
						to = mail.getToId();
						to_nickname = "";
					} else {
						to_user = MailBoxHBaseImpl.findUserById(mail.getToId());
						to = to_user.getMailAddress();
						to_nickname = to_user.getNickname();
					}
					String from = (String)request.getSession().getAttribute("mail_address");
					String from_nickname = (String)request.getSession().getAttribute("nickname");
					
					String subject = mail.getSubject();
					String time = mail.getTime();
					String star = mail.getFromStar();
					String mail_id = mail.getMailId();
					JSONObject jObj = new JSONObject();
					jObj.put("read", read);
					jObj.put("file", file);
					jObj.put("file_id", file_id);
//					System.out.println(mail_id);
					if("true".equals(file))
						jObj.put("file_name", MailBoxHBaseImpl.findFileByFileId(file_id).getFileName());
					else
						jObj.put("file_name", "");
					jObj.put("from", from);
					jObj.put("from_nickname", from_nickname);
					jObj.put("to", to);
					jObj.put("to_nickname", to_nickname);
					jObj.put("subject", subject);
					jObj.put("time", time);
					jObj.put("star", star);				
					jObj.put("mail_id", mail_id);
					jArr.add(jObj);
				}
				
			} else {
				alMail = new ArrayList<Mail>();
			}
			flag = true;
		} 
		
		
		
		
		response.setContentType("text/html;charset=UTF-8");
		PrintWriter out = response.getWriter();
		JSONObject obj = new JSONObject();
		if(flag) {
			obj.put("status", "success");
			obj.put("array", jArr.toString());
			obj.put("mail_address", (String)request.getSession().getAttribute("mail_address"));
		} else {
			obj.put("status", "failure");
		}
		
		out.print(obj.toString());
		out.close();
	}

	/**
	 * The doPost method of the servlet. <br>
	 *
	 * This method is called when a form has its tag value method equals to post.
	 * 
	 * @param request the request send by the client to the server
	 * @param response the response send by the server to the client
	 * @throws ServletException if an error occurred
	 * @throws IOException if an error occurred
	 */
	public void doPost(HttpServletRequest request, HttpServletResponse response)
			throws ServletException, IOException {

		doGet(request, response);
	}

	/**
	 * Initialization of the servlet. <br>
	 *
	 * @throws ServletException if an error occurs
	 */
	public void init() throws ServletException {
		// Put your code here
	}

}
