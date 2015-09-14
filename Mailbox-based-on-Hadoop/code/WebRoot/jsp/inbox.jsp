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
		<script type="text/javascript" src="js/inbox.js"> </script>

	</head>
  
	<body>
		<div class="container">
			<div class="total">
				<button id="delete_file">Delete</button>
				<!-- <button>Forward</button> -->
				<select id="mark">
					<option selected="selected" value="">Mark as...</option>
					<option value="read">Read</option>
					<option value="unread">Unread</option>
					<option value="star">Starred messages</option>
					<option value="unstar">Remove star</option>
				</select>
				<select id="move">
					<option selected="selected" value="">Move to...</option>
					<option value="inbox">Inbox</option>
					<option value="outbox">Outbox</option>
					<option value="trash">Trash</option>
				</select>
				<a id="next_page">Next</a><a id="previous_page">Previous</a><br>
				<table>
					<tr class="title">
						<td width="20px"><input class="title_checkbox" type="checkbox"></td>
						<td width="60px"><img src="image/small_mail.png"></img><img src="image/small_file.png"></img></td>
						<td width="160px">From</td>
						<td>Subject</td>
						<td width="300px">Time</td>
						<td width="30px"></td>
					</tr>
				</table>
			</div>
		</div>
	</body>
</html>
