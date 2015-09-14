function startTime() {
	var today=new Date();
	var h=today.getHours();
	var m=today.getMinutes();
	var s=today.getSeconds();
	// add a zero in front of numbers<10
	m=checkTime(m);
	s=checkTime(s);
	$("#time").html(h+":"+m+":"+s);
	t=setTimeout("startTime()",500);
}

function checkTime(i) {
	if (i<10) {
		i="0" + i;
	}
	  return i;
}

$(document).ready(function(){
	var bgi = $(".bgi").attr("src");//"image/sao.jpg"
	var color = $(".bgi").attr("tag1");//#0000ff
	var hover_color = $(".bgi").attr("tag2");//"rgba(0, 0, 255, 0.3)";
	if(bgi == "") {
		$(".bgi").attr("src", "image/sao.jpg");
	}
	if(color == "") {
		color = "#0000ff";
		$(".bgi").attr("tag1", color);
	}
	if(hover_color == "") {
		hover_color = "rgba(0, 0, 255, 0.3)";
		$(".bgi").attr("tag2", hover_color);
	}
	$(".navigator").css("color", color);
	$(".top").css("color", color);
	$(".content").css("color", color);
//	$(".bgi").attr("src", bgi);
	$(".logo").attr("src", "image/logo.png");
	$(".content").attr("src", "jsp/welcome.jsp");
	
	
//	alert($(".bgi").attr("tag1"));
	startTime();
	$("li").mouseover(function() {
		$(this).css("background-color",hover_color);
		$(this).css("cursor","pointer");
	}).mouseleave(function() {
		$(this).css("background-color","rgba(0,0,0,0)");
		$(this).css("cursor","auto");
	});
	$("#compose").click(function() {
		$(".content").attr("src", "jsp/compose.jsp");
	});
	$("#inbox").click(function() {
		$(".content").attr("src", "jsp/inbox.jsp");
	});
	$("#contacts").click(function() {
		$(".content").attr("src", "jsp/contacts.jsp");
	});
	$("#outbox").click(function() {
		$(".content").attr("src", "jsp/outbox.jsp");
	});
	$("#drafts").click(function() {
		$(".content").attr("src", "jsp/drafts.jsp");
	});
	$("#trash").click(function() {
		$(".content").attr("src", "jsp/trash.jsp");
	});
	$("#netdisk").click(function() {
		$(".content").attr("src", "jsp/netdisk.jsp");
	});
	$("#skin").click(function() {
		$(".content").attr("src", "jsp/skin.jsp");
	});
	$("#homepage").click(function() {
		$(".content").attr("src", "jsp/welcome.jsp");
	});
	$("#help").click(function() {
		alert("Sorry this part is not available!");
	});
	
	$("#logout").click(function() {
		$(".toast").animate({
			top: "0px"
		});
		$.ajax({  
			type: "GET",
			url:"/mailtest/servlet/LogoutServlet",  
			dataType:'json', //很重要!!!.      预期服务器返回的数据类型  
			error:function(){
				$(".toast").animate({
					top: "-12%"
				});
				alert("error");
	    	},  
			success:function(data){  
				$(".toast").animate({
					top: "-12%"
				});
				if(data.status == "success") {
					alert("Logout successfully!");
					window.location.href = "/mailtest/jsp/homepage.jsp";
				} else if(data.status == "failure"){
					alert("Logout unsuccessfully!");
				}
			}   
		});  
	});
	
	
	
	//$("#mail_address").html($.session("username"));
	/*$.ajax({  
		type: "GET",
		url:"/mailtest/servlet/LoginServlet",  
		data:{
			username:$("#username").val(),
			password:$("#password").val()
		},  
		dataType:'json', //很重要!!!.      预期服务器返回的数据类型  
		error:function(){
			$(".toast").animate({
				top: "-12%"
			});
			alert("error");
    	},  
		success:function(data){  
			$(".toast").animate({
				top: "-12%"
			});
			if(data.status == "success") {
				window.location.href = "/mailtest/jsp/inbox.jsp";
			} else if(data.status == "failure"){
				alert("Wrong username or password!");
			}
			//alert(dat.username + ";" + dat.password);
			//alert(data.status);
		}   
	}); */
});