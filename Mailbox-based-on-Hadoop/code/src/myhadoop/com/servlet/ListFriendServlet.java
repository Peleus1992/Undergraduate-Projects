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
import myhadoop.com.nosql.vo.User;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

public class ListFriendServlet extends HttpServlet {

	/**
	 * Constructor of the object.
	 */
	public ListFriendServlet() {
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
		ArrayList<String> alFriend = MailBoxHBaseImpl.findFriendByUserId(user_id);
		if(alFriend == null) {
			alFriend = new ArrayList<String>();
		} 
		Iterator<String> it = alFriend.iterator();
		JSONArray jArr = new JSONArray();
		while(it.hasNext()) {
			String friend_id = it.next();
			User user = MailBoxHBaseImpl.findUserById(friend_id);
			JSONObject jObj = new JSONObject();
			jObj.put("nickname", user.getNickname());
			jObj.put("mail_address", user.getMailAddress());
			jObj.put("phone", user.getPhone());
			jObj.put("friend_id", friend_id);
			jArr.add(jObj);
		}
		flag = true;
		response.setContentType("text/html;charset=UTF-8");
		PrintWriter out = response.getWriter();
		JSONObject obj = new JSONObject();
		if(flag) {
			obj.put("status", "success");
			obj.put("array", jArr);
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
