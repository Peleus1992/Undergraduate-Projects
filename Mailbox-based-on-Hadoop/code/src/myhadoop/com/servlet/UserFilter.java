package myhadoop.com.servlet;

import java.io.IOException;

import javax.servlet.Filter;
import javax.servlet.FilterChain;
import javax.servlet.FilterConfig;
import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;
import javax.servlet.http.HttpServletRequest;

public class UserFilter implements Filter {

	@Override
	public void destroy() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void doFilter(ServletRequest req, ServletResponse resp,
			FilterChain chain) throws IOException, ServletException {
		// TODO Auto-generated method stub
		if(((HttpServletRequest)req).getSession().getAttribute("id") == null) {
			if(((HttpServletRequest)req).getRequestURI().equals("/mailtest/jsp/homepage.jsp") 
					|| ((HttpServletRequest)req).getRequestURI().equals("/mailtest/jsp/register.jsp")) {
				chain.doFilter(req, resp);
			} else {
				RequestDispatcher rd = req.getRequestDispatcher("/jsp/homepage.jsp");
				rd.forward(req, resp);
			}
			
		} else {
			chain.doFilter(req, resp);
		}
	}

	@Override
	public void init(FilterConfig arg0) throws ServletException {
		// TODO Auto-generated method stub
		
	}

}
