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
import myhadoop.com.nosql.vo.File;
import myhadoop.com.nosql.vo.Share;
import net.sf.json.JSONArray;
import net.sf.json.JSONObject;

public class ListFileServlet extends HttpServlet {

	/**
	 * Constructor of the object.
	 */
	public ListFileServlet() {
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
		ArrayList<Share> alShare = MailBoxHBaseImpl.findShareByUserId(user_id);
		JSONArray jArr = new JSONArray();
		
		if(alShare != null) {
			Iterator<Share> it = alShare.iterator();
			while(it.hasNext()) {
				Share share = it.next();
				String file_id = share.getFileId();
				File file = MailBoxHBaseImpl.findFileByFileId(file_id);
				JSONObject jObj = new JSONObject();
				jObj.put("file_name", file.getFileName());
				jObj.put("file_size", file.getFileSize());
				jObj.put("time", file.getTime());
				jObj.put("file_id", file.getFileId());
				jArr.add(jObj);
			}
			
		} else {
			alShare = new ArrayList<Share>();
		}
		flag = true;
		response.setContentType("text/html;charset=UTF-8");
		PrintWriter out = response.getWriter();
		JSONObject obj = new JSONObject();
		if(flag) {
			obj.put("status", "success");
			obj.put("array", jArr.toString());
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
