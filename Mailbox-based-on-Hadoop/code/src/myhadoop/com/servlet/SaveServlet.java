package myhadoop.com.servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Date;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import myhadoop.com.hdfs.HdfsImpl;
import myhadoop.com.nosql.MailBoxHBaseImpl;
import myhadoop.com.nosql.vo.Mail;
import net.sf.json.JSONObject;

public class SaveServlet extends HttpServlet {

	/**
	 * Constructor of the object.
	 */
	public SaveServlet() {
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
		//save receiver email address in convenience
		String to_id = request.getParameter("to");
		if(to_id == null) {
			to_id = "";
		}
		String from_id = (String)request.getSession().getAttribute("id");
		String subject = request.getParameter("subject");
		if(subject == null) {
			subject = "";
		}
		String mail_id = null;
		if(request.getParameter("content") == null) {
			String content = "";
			mail_id = HdfsImpl.saveMail(content
					, request.getRemoteAddr());
		} else {
			mail_id = HdfsImpl.saveMail(request.getParameter("content")
					, request.getRemoteAddr());
		}
		String to_position = Mail.VOID;
		String from_position = Mail.DRAFTS;
		String time = new Date().toString();
		String to_star = "false";			
		String from_star = "false";
		String to_read = "false";
		String from_read = "false";
		String appendix = null;
		String file_id = null;
		if(request.getParameter("file_id").equals("")) {
			appendix = "false";
			file_id = "";
		} else {
			appendix = "true";
			file_id = request.getParameter("file_id");
		}
		Mail mail = new Mail();
		mail.setToId(to_id);
		mail.setFromId(from_id);
		mail.setSubject(subject);
		mail.setMailId(mail_id);
		mail.setToPosition(to_position);
		mail.setFromPosition(from_position);
		mail.setTime(time);
		mail.setToStar(to_star);
		mail.setFromStar(from_star);
		mail.setToRead(to_read);
		mail.setFromRead(from_read);
		mail.setAppendix(appendix);
		mail.setFileId(file_id);
		MailBoxHBaseImpl.registerMail(mail, request.getRemoteAddr());
		flag = true;
		
		response.setContentType("text/html;charset=UTF-8");
		PrintWriter out = response.getWriter();
		JSONObject obj = new JSONObject();
		if(flag) {
			obj.put("status", "success");
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
