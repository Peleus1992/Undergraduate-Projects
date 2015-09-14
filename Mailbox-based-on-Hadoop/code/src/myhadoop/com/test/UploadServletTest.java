package myhadoop.com.test;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.OutputStream;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

public class UploadServletTest extends HttpServlet {

	@Override
	protected void doGet(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		
		File file = new File("/home/grid/932.mp3");
		FileInputStream fis = new FileInputStream(file);
		//attachment/inline
		resp.setContentType("audio/mpeg"); 
//		 resp.setContentType("application/octet-stream");
		resp.setHeader("Content-Disposition", "inline; filename=" + "world.pdf");
//		resp.setContentType("application/octet-stream");
		
		OutputStream out = resp.getOutputStream();
		byte buffer[] = new byte[1024];
		int iFlag = -1;
		while((iFlag = fis.read(buffer)) != -1 ) {
			out.write(buffer);
		}
		out.flush();
		out.close();
		fis.close();
	}

	@Override
	protected void doPost(HttpServletRequest req, HttpServletResponse resp)
			throws ServletException, IOException {
		doGet(req, resp);
	}
	
}
