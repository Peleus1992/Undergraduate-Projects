<%@ page language="java" import="java.util.*" pageEncoding="UTF-8"%>
<%
String path = request.getContextPath();
String basePath = request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+path+"/";
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
	<head>
		<base href="<%=basePath%>">
    
		<title>My JSP 'inbox.jsp' starting page</title>
    
		<meta http-equiv="pragma" content="no-cache">
		<meta http-equiv="cache-control" content="no-cache">
		<meta http-equiv="expires" content="0">    
		<meta http-equiv="keywords" content="keyword1,keyword2,keyword3">
		<meta http-equiv="description" content="This is my page">
		<link rel="stylesheet" type="text/css" href="css/style.css">
		<script type="text/javascript" src="js/jquery-1.11.2.js"> </script>
		<script type="text/javascript" src="js/main.js"> </script>

	</head>
  
	<body>
		<div class="container">
			<img class="bgi" src="<%=(String)session.getAttribute("skin")%>" tag1="<%= (String)session.getAttribute("color")%>" tag2="<%= (String)session.getAttribute("hover_color")%>">
			<div class="toast">
				<img id="loading" class="loading" src="image/loading.gif">
				<p>Please waiting...</p>
			</div>
			<div class="top">
				<img class="logo">
				<div class="left_info">
					<span><%=session.getAttribute("nickname") %></span>
					<a id="mail_address">&lt<%=session.getAttribute("mail_address") %>&gt</a><br>
					<a id="homepage">Homepage</a> <span>|&nbsp</span><a id="skin">Change skin</a>
				</div>
				<div class="right_info">
					<a id="help">Help</a> <span>|&nbsp</span><a id="logout">Logout</a><br>
					<span id="time"></span>
				</div>
			</div>
			<div class="navigator">
				<ul>
					<li id="compose" class="navigator_li"><img class="icon" src="image/small_compose.jpg"></img><a>Compose</a></li>
					<li id="inbox" class="navigator_li"><img class="icon" src="image/small_inbox.jpg"></img><a>Inbox</a></li>
					<li id="contacts" class="navigator_li"><img class="icon" src="image/small_contacts.jpg"></img><a>Contacts</a></li>
					<li id="outbox" class="navigator_small_li">Outbox</li>
					<li id="drafts" class="navigator_small_li">Drafts</li>
					<li id="trash" class="navigator_small_li">Trash</li>
					<li id="netdisk" class="navigator_small_li2">Netdisk</li>
					
				</ul>
			</div>
			<iframe class="content">
			
			</iframe> 
		</div>
	</body>
</html>
