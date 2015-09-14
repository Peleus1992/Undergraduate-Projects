package myhadoop.com.servlet;

import java.io.IOException;
import java.io.PrintWriter;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import javax.servlet.http.HttpSession;

import myhadoop.com.nosql.MailBoxHBaseImpl;
import myhadoop.com.nosql.vo.Complex;
import myhadoop.com.nosql.vo.User;
import net.sf.json.JSONObject;

public class LoginServlet extends HttpServlet {

	/**
	 * Constructor of the object.
	 */
	public LoginServlet() {
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
		HttpSession session = request.getSession();
		String mail_address = request.getParameter("mail_address");
		String password = request.getParameter("password");
		//hbase op
		String user_id = null;
		User user = null;
		try {
			user_id = MailBoxHBaseImpl.findIdByMailAddress(mail_address);
			user = MailBoxHBaseImpl.findUserByMailAddress(mail_address);
		} catch (Exception e){
			e.printStackTrace();
		}
		if(user != null && user.getPassword().equals(password)) {
			session.setAttribute("mail_address", mail_address);
			session.setAttribute("id", user_id);
			session.setAttribute("nickname", user.getNickname());
			String complex_id = MailBoxHBaseImpl.getComplexId(user_id);
			if(complex_id != null) {
				Complex complex = MailBoxHBaseImpl.getComplex(complex_id);
				session.setAttribute("skin", complex.getSkin());
				session.setAttribute("color", complex.getColor());
				session.setAttribute("hover_color", complex.getHoverColor());
				
			} else {
				session.setAttribute("skin", "");
				session.setAttribute("color", "");
				session.setAttribute("hover_color", "");
			}
			flag = true;
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
