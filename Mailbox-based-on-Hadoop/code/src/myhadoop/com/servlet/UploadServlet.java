package myhadoop.com.servlet;

import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import myhadoop.com.hdfs.HdfsImpl;
import myhadoop.com.nosql.MailBoxHBaseImpl;
import myhadoop.com.nosql.vo.File;
import myhadoop.com.nosql.vo.Share;
import net.sf.json.JSONObject;

import org.apache.commons.fileupload.FileItem;
import org.apache.commons.fileupload.disk.DiskFileItemFactory;
import org.apache.commons.fileupload.servlet.ServletFileUpload;

public class UploadServlet extends HttpServlet {

	/**
	 * Constructor of the object.
	 */
	public UploadServlet() {
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
		try {
			DiskFileItemFactory factory = new DiskFileItemFactory();
			ServletFileUpload upload = new ServletFileUpload(factory);
			upload.setSizeMax(1000*1024*1024);
			List<FileItem> items = upload.parseRequest(request);
			Iterator<FileItem> iter = items.iterator();
			//init
			String user_id = (String)request.getSession().getAttribute("id");
			String file_id = null;
			String file_name = null;
			String time = new Date().toString();
			long file_size = 0;
			InputStream in = null;
			int length = 0;
			while(iter.hasNext()) {
				FileItem item = iter.next();
				String fieldName = item.getFieldName();
				if(!item.isFormField()) {
					file_name = item.getName();
					length = (int)item.getSize();
					file_size = item.getSize();
					if(length > 0) {
						in = item.getInputStream();
					} else {
						file_id = null;
					}
				}
			}
			if(length > 0) {
				file_id = HdfsImpl.saveFile(in, length, request.getRemoteAddr());
				File file = new File();
				file.setUserId(user_id);
				file.setFileId(file_id);
				file.setFileName(file_name);
				file.setFileSize(file_size);
				file.setTime(time);
				file.setUserId(user_id);
				MailBoxHBaseImpl.registerFile(file, request.getRemoteAddr());
				Share share = new Share();
				share.setFileId(file_id);
				share.setTime(new Date().toString());
				share.setUserId(user_id);
				MailBoxHBaseImpl.registerShare(share, request.getRemoteAddr());
				flag = true;
			}
			response.setContentType("text/html;charset=UTF-8");
			PrintWriter out = response.getWriter();
			JSONObject obj = new JSONObject();
			if(flag) {
				obj.put("status", "success");
				obj.put("file_id", file_id);
				obj.put("file_name", file_name);
				obj.put("file_size", "" + file_size);
				obj.put("time", time);
			} else {
				obj.put("status", "failure");
			}
			
			out.print(obj.toString());
			out.close();
			
		} catch(Exception e) {
			e.printStackTrace();
		}
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
