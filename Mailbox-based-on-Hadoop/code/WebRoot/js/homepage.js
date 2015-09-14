$(document).ready(function(){
	$(".bgi").attr("src", "image/blue.jpg");
	$(".logo").attr("src", "image/logo.png");
	$("#login").click(function() {
		//alert($("#username").val() + ";" + $("#password").val());
		$(".toast").animate({
			top: "0px"
		});
		$.ajax({  
			type: "GET",
			url:"/mailtest/servlet/LoginServlet",  
			data:{
				mail_address:$("#mail_address").val(),
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
					window.location.href = "/mailtest/jsp/main.jsp";
				} else if(data.status == "failure"){
					alert("Wrong mail address or password!");
				}
				//alert(dat.username + ";" + dat.password);
				//alert(data.status);
			}   
    	});  
	});
});