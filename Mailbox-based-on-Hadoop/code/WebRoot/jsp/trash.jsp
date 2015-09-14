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
		<script type="text/javascript" src="js/jquerysession.js"> </script>
		<script type="text/javascript" src="js/trash.js"> </script>

	</head>
  
	<body>
		<div class="container">
			<div class="total">
				<button id="delete_file">Delete</button>
				<!-- <button>Forward</button> -->
				<button id="restore">Restore</button>
				
				<a id="next_page">Next</a><a id="previous_page">Previous</a><br>
				<table>
					<tr class="title">
						<td width="20px"><input class="title_checkbox" type="checkbox"></td>
						<td width="60px"><img src="image/small_file.png"></img></td>
						<td width="160px">From</td>
						<td width="160px">To</td>
						<td>Subject</td>
						<td width="300px">Time</td>
					</tr>
				</table>
			</div>
		</div>
	</body>
</html>
