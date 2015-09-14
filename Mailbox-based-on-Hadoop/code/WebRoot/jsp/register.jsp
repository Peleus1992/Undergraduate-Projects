<%@ page language="java" import="java.util.*" pageEncoding="UTF-8"%>
<%
String path = request.getContextPath();
String basePath = request.getScheme()+"://"+request.getServerName()+":"+request.getServerPort()+path+"/";
%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
	<head>
		<base href="<%=basePath%>">
	    
		<title>My JSP 'register.jsp' starting page</title>
	    
		<meta http-equiv="pragma" content="no-cache">
		<meta http-equiv="cache-control" content="no-cache">
		<meta http-equiv="expires" content="0">    
		<meta http-equiv="keywords" content="keyword1,keyword2,keyword3">
		<meta http-equiv="description" content="This is my page">
		<link rel="stylesheet" type="text/css" href="css/style.css">
		<script type="text/javascript" src="js/jquery-1.11.2.js"> </script>
		<script type="text/javascript" src="js/register.js"> </script>

	</head>
  	
	<body>
		<div class="container">
			<img class="bgi">
			<div class="toast">
				<img id="loading" class="loading" src="image/loading.gif">
				<p>Please waiting...</p>
			</div>
			<div class="register_form">
				<span class="title">Register form</span>
				<span >Mail addr.*:	</span><input id="mail_address" placeholder="example@myhadoop.com" class="textbox" type="text">
				<span id="mail_address_hint"class="hint"></span><br>
				<span >Password*:	</span><input id="password" placeholder="eg. aBc123" class="textbox" type="password">
				<span id="password_hint"class="hint"></span><br>
				<span >Confirm*:	</span><input id="confirm" class="textbox" type="password">
				<span id="confirm_hint"class="hint"></span><br>
				<span >Nickname*:	</span><input id="nickname" placeholder="eg. Onion" class="textbox" type="text">
				<span id="nickname_hint"class="hint"></span><br>
				<span >Sex:	</span>
				<select id="sex">
					<option selected="selected" value=""></option>
					<option value="male">male</option>
					<option value="female">female</option>
					<option value="other">other</option>					
				</select>
				<span id="sex_hint"class="hint"></span><br>
				<span>Birthday: </span>
				<select class="date" id="date_month">  
					<option label="" value="" selected="selected"></option> 
					<option label="January" value="01">January</option>  
					<option label="February" value="02">February</option>  
					<option label="March" value="03">March</option>  
					<option label="April" value="04" >April</option>  
					<option label="May" value="05">May</option>  
					<option label="June" value="06">June</option>  
					<option label="July" value="07">July</option>  
					<option label="August" value="08">August</option>  
					<option label="September" value="09">September</option>  
					<option label="October" value="10">October</option>  
					<option label="November" value="11">November</option>  
					<option label="December" value="12">December</option>  
				</select>  
				<select class="date" id="date_day">  
					<option label="" value="" selected="selected"></option> 
					<option label="01" value="1">01</option>  
					<option label="02" value="2">02</option>  
					<option label="03" value="3">03</option>  
					<option label="04" value="4">04</option>  
					<option label="05" value="5">05</option>  
					<option label="06" value="6">06</option>  
					<option label="07" value="7">07</option>  
					<option label="08" value="8">08</option>  
					<option label="09" value="9">09</option>  
					<option label="10" value="10">10</option>  
					<option label="11" value="11">11</option>  
					<option label="12" value="12">12</option>  
					<option label="13" value="13">13</option>  
					<option label="14" value="14">14</option>  
					<option label="15" value="15">15</option>  
					<option label="16" value="16">16</option>  
					<option label="17" value="17">17</option>  
					<option label="18" value="18">18</option>  
					<option label="19" value="19">19</option>  
					<option label="20" value="20">20</option>  
					<option label="21" value="21">21</option>  
					<option label="22" value="22">22</option>  
					<option label="23" value="23">23</option>  
					<option label="24" value="24">24</option>  
					<option label="25" value="25">25</option>  
					<option label="26" value="26">26</option>  
					<option label="27" value="27">27</option>  
					<option label="28" value="28">28</option>  
					<option label="29" value="29">29</option>  
					<option label="30" value="30">30</option>  
					<option label="31" value="31">31</option>
				</select>
				<span id="date_hint"class="hint"></span><br>
				<span>Phone:	</span><input id="phone" placeholder="13352282701" class="textbox" type="text">
				<span id="phone_hint"class="hint"></span><br>
				<a id="submit">Submit Now</a>
			</div>
		</div>
	</body>
</html>
