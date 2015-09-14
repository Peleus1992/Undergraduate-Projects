function checkMailAddress() {
	//alert($("#mail_address").val());
	if(/^([a-zA-Z0-9_])+(\.?[a-zA-Z0-9_]+)*@myhadoop.com$/.test($("#mail_address").val())) {
		return true;
	} else {
		return false;
	}	
}
function checkPassword() {
	if(/^[a-zA-Z0-9_]{6,}$/.test($("#password").val())) {
		return true;
	} else {
		return false;
	}
}
function checkConfirm() {
	if($("#password").val() == $("#confirm").val()) {
		return true;
	} else {
		return false;
	}
}
function checkNickname() {
	if($("#nickname").val() != "") {
		return true;
	} else {
		return false;
	}
}
function checkPhone() {
	if($("#phone").val() == "") {
		return true;
	} else if(/(^[0-9]{11}$)|(^[0-9]{6,8}$)|(^[0-9]{3,4}\-[0-9]{6,8}$)/.test($("#phone").val())) {
		return true;
	} else {
		return false;
	}
}
$(document).ready(function(){
	$(".bgi").attr("src", "image/blue.jpg");
	//$("#mail_address").html($.session("username"));
	$("#mail_address").focus(function() {
		$("#mail_address_hint").html("Enter your email address").css("color","green");
	}).blur(function() {
		if(checkMailAddress()) {
			$("#mail_address_hint").html("Correct").css("color","green");
		} else {
			$("#mail_address_hint").html("Wrong format").css("color","red");
		}
	});
	$("#password").focus(function() {
		$("#password_hint").html("Enter your password").css("color","green");
	}).blur(function() {
		if($("#password").val() == "") {
			$("#password_hint").html("Should not be empty").css("color","red");
		} else if(checkPassword()) {
			$("#password_hint").html("Correct").css("color","green");
		} else {
			$("#password_hint").html("At lease 6 [a-zA-Z0-9_]").css("color","red");
		}
	});
	$("#confirm").focus(function() {
		$("#confirm_hint").html("Confirm your password").css("color","green");
	}).blur(function() {
		if($("#confirm").val() == "") {
			$("#confirm_hint").html("Should not be empty").css("color","red");
		} else if(checkConfirm()) {
			$("#confirm_hint").html("Correct").css("color","green");
		} else {
			$("#confirm_hint").html("Not the same as password").css("color","red");
		}
	});
	$("#nickname").focus(function() {
		$("#nickname_hint").html("Enter your nickname").css("color","green");
	}).blur(function() {
		if(checkNickname()) {
			$("#nickname_hint").html("Correct").css("color","green");
		} else {
			$("#nickname_hint").html("Should not be empty").css("color","red");
		}
	});
	$("#sex").focus(function() {
		$("#sex_hint").html("Select your sex").css("color","green");
	}).blur(function() {
		$("#sex_hint").html("");
	});
	$("#date_month").focus(function() {
		$("#date_hint").html("Select your birthday").css("color","green");
	}).blur(function() {
		$("#date_hint").html("");
	});
	$("#date_day").focus(function() {
		$("#date_hint").html("Select your birthday").css("color","green");
	}).blur(function() {
		$("#date_hint").html("");
	});
	$("#phone").focus(function() {
		$("#phone_hint").html("Enter your telephone number").css("color","green");
	}).blur(function() {
		if(checkPhone()){
			$("#phone_hint").html("");
		} else {
			$("#phone_hint").html("Wrong format").css("color", "red");
		}
		
	});
	$("#submit").click(function() {
		if(checkMailAddress()&&checkPassword()&&checkConfirm()&&checkNickname()&&checkPhone()) {
			$(".toast").animate({
				top: "0px"
			});
			$.ajax({  
				type: "GET",
				url:"/mailtest/servlet/RegisterServlet",  
				data:{
					mail_address:$("#mail_address").val(),
					password:$("#password").val(),
					nickname:$("#nickname").val(),
					sex:$("#sex").val(),
					date_month: $("#date_month").val(),
					date_day: $("#date_day").val(),
					phone: $("#phone").val()
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
						alert("Congrats! You have registered successfully!");
						window.location.href = "jsp/homepage.jsp";
					} else if(data.status == "failure"){
						alert("Mail address exists!");
					}
					//alert(dat.username + ";" + dat.password);
					//alert(data.status);
				}   
			}); 
		} else {
			alert("Please check your register form");
		}
		
	});
	
});