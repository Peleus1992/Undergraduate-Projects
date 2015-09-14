package myhadoop.com.servlet;

import java.io.IOException;
import java.io.PrintWriter;
import java.util.Date;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import myhadoop.com.nosql.MailBoxHBaseImpl;
import myhadoop.com.nosql.vo.User;
import net.sf.ezmorph.object.DateMorpher;
import net.sf.json.JSONObject;

public class RegisterServlet extends HttpServlet {

	/**
	 * Constructor of the object.
	 */
	public RegisterServlet() {
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
		String mail_address = request.getParameter("mail_address");
		String password = request.getParameter("password");
		String nickname = request.getParameter("nickname");
		String sex = request.getParameter("sex");
		String date_month = request.getParameter("date_month");
		String date_day = request.getParameter("date_day");
		String phone = request.getParameter("phone");
		//hbase op
		User user = null;
		try {
			user = MailBoxHBaseImpl.findUserByMailAddress(mail_address);
			if(user == null) {
				user = new User();
				user.setMailAddress(mail_address);
				user.setPassword(password);
				user.setNickname(nickname);
				user.setSex(sex);
				user.setDateMonth(date_month);
				user.setDateDay(date_day);
				user.setPhone(phone);
			 	flag = MailBoxHBaseImpl.registerUser(user, request.getRemoteAddr());
			}
		} catch(Exception e) {
			e.printStackTrace();
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
