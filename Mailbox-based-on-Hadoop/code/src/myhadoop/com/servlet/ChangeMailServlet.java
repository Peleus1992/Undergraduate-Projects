package myhadoop.com.servlet;

import java.io.IOException;
import java.io.PrintWriter;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import net.sf.json.JSONObject;

import myhadoop.com.nosql.MailBoxHBaseImpl;

public class ChangeMailServlet extends HttpServlet {

	/**
	 * Constructor of the object.
	 */
	public ChangeMailServlet() {
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
		String mail_id_list = request.getParameter("mail_id_list");
		String to_position=request.getParameter("to_position");
		String from_position=request.getParameter("from_position");
		String to_read=request.getParameter("to_read");
		String from_read=request.getParameter("from_read");
		String to_star=request.getParameter("to_star");
		String from_star=request.getParameter("from_star");
		String [] mail_ids = mail_id_list.split(",");
//		System.out.println(to_position);
//		System.out.println(from_position);
//		System.out.println(to_read);
//		System.out.println(from_read);
//		System.out.println(to_star);
//		System.out.println(from_star);
//		System.out.println("mail_ids:");
//		for(String mail_id : mail_ids) {
//			System.out.println(mail_id);
//		}
		
		for(String mail_id : mail_ids) {
			flag = true;
			if(!"".equals(to_position)) {
				MailBoxHBaseImpl.changeMailToPosition(mail_id, to_position);
			} else if(!"".equals(from_position)) {
				MailBoxHBaseImpl.changeMailFromPosition(mail_id, from_position);
			} else if(!"".equals(to_read)) {
				MailBoxHBaseImpl.changeMailToRead(mail_id, to_read);
			} else if(!"".equals(from_read)) {
				MailBoxHBaseImpl.changeMailFromRead(mail_id, from_read);
			} else if(!"".equals(to_star)) {
				MailBoxHBaseImpl.changeMailToStar(mail_id, to_star);
			} else if(!"".equals(from_star)) {
				MailBoxHBaseImpl.changeMailFromStar(mail_id, from_star);
			} else {
				flag  = false;
			}
		}
		
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
