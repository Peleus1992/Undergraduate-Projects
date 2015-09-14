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
		<script type="text/javascript" src="js/netdisk.js"> </script>

	</head>
  
	<body>
		<div class="container">
			<div class="middle">
				<input id="fileToUpload" name="fileToUpload" type="file" class="file_hidden"
                        onchange="file_text.value=this.value;">
				<input id="file_text"  name="file_text" class="textbar" readonly type="text" >
				<button onclick="fileToUpload.click();">Browse</button>
				<button id="upload">Upload</button>
				<button id="deleteFile">Delete</button>
				<a id="next_page">Next</a><a id="previous_page">Previous</a><br>
				<table>
					<tr class="title">
						<td width="1%"><input class="title_checkbox" type="checkbox"></td>
						<td width="33%">Filename</td>
						<td width="33%">Size</td>
						<td width="33%">Time</td>
					</tr>
				</table>
			</div>
			<div class="right">
				<span class="title">Contacts</span>
				<ul class="list">
					
					
				</ul>
			</div>
		</div>
	</body>
</html>
