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
		<script type="text/javascript" src="js/contacts.js"> </script>

	</head>
  
	<body>
		<div class="container">
			<div class="total">
				<button id="delete" class="left">Delete</button>
				<button id="add" class="right">Add</button>
				<input id="mail_address" class="textbox" type="text" placeholder="example@myhadoop.com">
				<span class="right">Mail address: </span><br>
				<table>
					<tr class="title">
						<td width="1%"><input class="title_checkbox" type="checkbox"></td>
						<td width="33%">Nickname</td>
						<td width="33%">Mail</td>
						<td width="33%">Phone</td>
					</tr>
				</table>
			</div>
		</div>
	</body>
</html>
