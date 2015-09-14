<%@ page language="java" import="java.util.*" pageEncoding="UTF-8"%>
<%
String path = request.getContextPath();
String basePath = request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+path+"/";
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
	<head>
		<base href="<%=basePath%>">
	    
		<title>My JSP 'homepage.jsp' starting page</title>
    
		<meta http-equiv="pragma" content="no-cache">
		<meta http-equiv="cache-control" content="no-cache">
		<meta http-equiv="expires" content="0">    
		<meta http-equiv="keywords" content="keyword1,keyword2,keyword3">
		<meta http-equiv="description" content="This is my page">
	
		<link rel="stylesheet" type="text/css" href="css/style.css">
		<script type="text/javascript" src="js/jquery-1.11.2.js"> </script>
		<script type="text/javascript" src="js/homepage.js"> </script>
	</head>
  
	<body>
		<div class="container">
			<img class="bgi">
			<div class="toast">
				<img id="loading" class="loading" src="image/loading.gif">
				<p>Please waiting...</p>
			</div>
			<img class="logo" >
				
				
			<div class="login_form">
				<form>
					<span>Mail Address:</span>
					<input id="mail_address" class="textbox" type="text" placeholder="example@myhadoop.com">
					<span>Password: </span>
					<input id="password" class="textbox" type="password" placeholder="eg. aBc123">
					<a id="login">Login</a>
					<a href="jsp/register.jsp">Register</a>
					
				</form>
				
			</div>
		</div>
    	
	</body>
</html>
