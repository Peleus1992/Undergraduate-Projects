function sayHello() {
	var hour = new Date().getHours();
	var wel;
	if(hour > 0 && hour < 12) {
		wel = "Good morning! ";
	} else if(hour >= 12 && hour < 18) {
		wel = "Good afternoon! ";
	} else {
		wel = "Good evening! ";
	}
	$("#welcome").html(wel);
}
function addContact(nickname, mail_address, friend_id) {
	var html = '<li class="element"><span>'+nickname+'&lt' +
	mail_address+'&gt</span><img tag="'+mail_address+'"></img></li>';
	$(".container .right ul.list").append(html);
//	$(".container .right ul").append('<li><span>'+nickname+'&lt' 
//			+ '&gt</span><img tag="'+friend_id+'"></img></li>');
}
function readFriend() {
	$(".toast", parent.document).animate({
		top: "0px"
	});
	$.ajax({  
		type: "GET",
		url:"/mailtest/servlet/ListFriendServlet",  
		dataType:'json', //很重要!!!.      预期服务器返回的数据类型  
		error:function(){
			$(".toast", parent.document).animate({
				top: "-12%"
			});
			alert("error");
    	},  
		success:function(data){  
			$(".toast", parent.document).animate({
				top: "-12%"
			});
			if(data.status == "success") {
				for(var i = 0; i < data.array.length; i++) {
					addContact(data.array[i].nickname, data.array[i].mail_address);
				}
			} else if(data.status == "failure"){
				alert("Read friend unsuccessfully!");
			}
		}   
	});  
}
function checkTo() {
	var to = $("input#to").val();
	if(to == "") {
		return false;
	} else {
		return true;
	}
}
function clearAll() {
	$("input#file_text").attr("tag", "");
	$("input#file_text").val("");
	$("input#to").val("");
	$("input#subject").val("");
	$("textarea#content").val("");
	
}

function deleteFile() {
	if($("input#file_text").attr("tag") != "") {
		$.ajax({  
			type: "GET",
			url:"/mailtest/servlet/DeleteFileServlet",  
			data:{
				file_id: $("input#file_text").attr("tag")
			},  
			dataType:'json', //很重要!!!.      预期服务器返回的数据类型  
			error:function(){
				$(".toast", parent.document).animate({
					top: "-12%"
				});
				alert("error");
        	},  
			success:function(data){  
				$(".toast", parent.document).animate({
					top: "-12%"
				});
				if(data.status == "success") {
					alert("Delete file successfully!");
				} else if(data.status == "failure"){
					alert("Delete file unsuccessfully!");
				}
				//alert(dat.username + ";" + dat.password);
				//alert(data.status);
			}   
    	});  
	} 
	$("input#file_text").attr("tag", "");
	$("textarea#content").attr("tag", "");
}
function getUrlParam(name) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)"); //构造一个含有目标参数的正则表达式对象
    var r = window.location.search.substr(1).match(reg);  //匹配目标参数
    if (r != null) return unescape(r[2]); return null; //返回参数值
}
function readMailContent(mail_id) {
	$(".toast", parent.document).animate({
		top: "0px"
	});
	$.ajax({  
		type: "GET",
		url:"/mailtest/servlet/ReadMailServlet", 
		data: {
			mail_id: mail_id
		},
		dataType:'json', //很重要!!!.      预期服务器返回的数据类型  
		error:function(){
			$(".toast", parent.document).animate({
				top: "-12%"
			});
			alert("error");
    	},  
		success:function(data){  
			$(".toast", parent.document).animate({
				top: "-12%"
			});
			if(data.status == "success") {
				$("textarea#content").html(data.content);
			} else if(data.status == "failure"){
				alert("Read Mail unsuccessfully!");
			}
			//alert(dat.username + ";" + dat.password);
			//alert(data.status);
		}   
	});  
}
$(document).ready(function(){
	var font_color = $(".bgi", parent.document).attr("tag1");
	var hover_color = $(".bgi", parent.document).attr("tag2");
	
	sayHello();
//	for(var i = 0; i < 20; i++) {
//		addContact("Juice");
//	}
	readFriend();
	if(getUrlParam("mail_address") != null) {
		$("input#to").val(getUrlParam("mail_address"));
	}
	if(getUrlParam("to") != null) {
		$("input#to").val(getUrlParam("to"));
	}
	if(getUrlParam("subject") != null) {
		$("input#subject").val(getUrlParam("subject"));
	}
	if(getUrlParam("file_id") != null) {
		$("input#file_text").attr("tag", getUrlParam("file_id"));
	}
	if(getUrlParam("file_name") != null) {
		$("input#file_text").val(decodeURI(getUrlParam("file_name")));
	}
	if(getUrlParam("mail_id") != null) {
		readMailContent(getUrlParam("mail_id"));
	}
	$(".container").css("color", font_color);
	
	$(document).on('mouseover',".container .right .list li.element",function(){
		$(this).css("background-color", hover_color);
		$(this).find("img").attr("src", "image/small_compose.jpg");
		$(this).find("img").show();
	}).on('mouseleave', '.container .right .list li.element',function() {
		$(this).css("background-color", "rgba(0,0,0,0)");
		$(this).find("img").removeAttr("src");
		$(this).find("img").hide();
	});
	$(document).on('click',".container .right .list .element img",function(){
//		alert($(this).attr("tag"));
		$("input#to").val($(this).attr("tag"));
	});
	$("button#upload").click(function() {
		if($("input#file_text").val() == "") {
			alert("Please choose a file!");
		} else {
			$(".toast", parent.document).animate({
				top: "0px"
			});
			$.ajaxFileUpload({
				url: "/mailtest/servlet/UploadServlet", //用于文件上传的服务器端请求地址
				secureuri: false, //是否需要安全协议，一般设置为false
				fileElementId: 'fileToUpload', //文件上传域的ID
				dataType: 'json', //返回值类型 一般设置为json
				error: function (data) { //服务器成功响应处理函数
					$(".toast", parent.document).animate({
						top: "-12%"
					});
					alert("error");
				},
				success: function (data) {//服务器响应失败处理函数
					$(".toast", parent.document).animate({
						top: "-12%"
					});
					//alert("s"+data.status + data.file_id);
					if(data.status == "failure") {
						alert("Upload file unsuccessfully!");
					} else if(data.status == "success") {
						alert("Upload file successfully!");
						$("input#file_text").attr("tag", data.file_id);
					}
				}
			});
		}
		
	});
	$("button#send").click(function(){
		if(checkTo()) {
			$(".toast", parent.document).animate({
				top: "0px"
			});
			$.ajax({  
				type: "GET",
				url:"/mailtest/servlet/SendServlet",  
				data:{
					to:$("input#to").val(),
					subject:$("input#subject").val(),
					content:$("textarea#content").val(),
					file_id: $("input#file_text").attr("tag")
				},  
				dataType:'json', //很重要!!!.      预期服务器返回的数据类型  
				error:function(){
					$(".toast", parent.document).animate({
						top: "-12%"
					});
					alert("error");
	        	},  
				success:function(data){  
					$(".toast", parent.document).animate({
						top: "-12%"
					});
					if(data.status == "success") {
						alert("Send mail successfully!");
						clearAll();
						window.location.href = "/mailtest/jsp/welcome.jsp";
					} else if(data.status == "failure"){
						alert("Send mail unsuccessfully! Please check mail address!");
					}
					//alert(dat.username + ";" + dat.password);
					//alert(data.status);
				}   
	    	});  
		} else {
			alert("Receiver should not be empty!");
		}
	});
	$("button#save").click(function(){
		$(".toast", parent.document).animate({
			top: "0px"
		});
		$.ajax({  
			type: "GET",
			url:"/mailtest/servlet/SaveServlet",  
			data:{
				to:$("input#to").val(),
				subject:$("input#subject").val(),
				content:$("textarea#content").val(),
				file_id: $("input#file_text").attr("tag")
			},  
			dataType:'json', //很重要!!!.      预期服务器返回的数据类型  
			error:function(){
				$(".toast", parent.document).animate({
					top: "-12%"
				});
				alert("error");
	    	},  
			success:function(data){  
				$(".toast", parent.document).animate({
					top: "-12%"
				});
				if(data.status == "success") {
					alert("Save mail successfully!");
					clearAll();
					window.location.href = "/mailtest/jsp/welcome.jsp";
				} else if(data.status == "failure"){
					alert("Save mail unsuccessfully!");
				}
				//alert(dat.username + ";" + dat.password);
				//alert(data.status);
			}   
		}); 
	});
	$("button#clearFile").click(function(){
		
		$("input#file_text").val("");
		$("input#file_text").attr("tag", "");
	});
	$("button#close").click(function(){
		clearAll();
		window.location.href = "/mailtest/jsp/welcome.jsp";
	});

	
});
