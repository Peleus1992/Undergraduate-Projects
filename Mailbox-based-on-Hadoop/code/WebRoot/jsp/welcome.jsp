<%@ page language="java" import="java.util.*" pageEncoding="UTF-8"%>
<%
String path = request.getContextPath();
String basePath = request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+path+"/";
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
	<head>
		<base href="<%=basePath%>">
	    
		<title>My JSP 'welcome.jsp' starting page</title>
	    
		<meta http-equiv="pragma" content="no-cache">
		<meta http-equiv="cache-control" content="no-cache">
		<meta http-equiv="expires" content="0">    
		<meta http-equiv="keywords" content="keyword1,keyword2,keyword3">
		<meta http-equiv="description" content="This is my page">
		<link rel="stylesheet" type="text/css" href="css/main.css">
		<script type="text/javascript" src="js/jquery-1.11.2.js"> </script>
		<script type="text/javascript" src="js/ajaxfileupload.js"></script>
		<script type="text/javascript" src="js/welcome.js"> </script>

	</head>
  
	<body>
		<div class="container">
			<div class="left">
				<span id="welcome"></span><span><%=session.getAttribute("nickname")%></span><br><br>
				&nbsp &nbsp &nbsp <a href="/mailtest/jsp/inbox.jsp">Inbox</a>&nbsp &nbsp &nbsp <a href="/mailtest/jsp/compose.jsp">Compose</a>&nbsp &nbsp &nbsp <a href="/mailtest/jsp/netdisk.jsp">Netdisk</a>	
			</div>
			<div class="right">
				<span class="title">Contacts</span>
				<ul class="list">
					
					
				</ul>
			</div>
			
		</div>
	</body>
</html>
