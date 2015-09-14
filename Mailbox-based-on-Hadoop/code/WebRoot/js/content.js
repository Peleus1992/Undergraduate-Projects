function describeMail(from, to, subject, appendix, time) {
	var content = "From:  "+from+"\r\n"
					+ "To:  " + to + "\r\n"
					+ "Subject:  " + subject + "\r\n"
					+ "Appendix:  " + appendix + "\r\n"
					+ "Time:  " + time + "\r\n"
					+ "-----------------------------------------------------------------------------------------------------------------------------------------------------\r\n";
	
	
		/*To: 
		Subject: 
		Appendix: no
		Time: 
		------------------------------------------------------------------------------------------------------------------------------------------------------
		Content:*/
	$(".mail_content").html(content);
}
function addMailContent(content) {
	$(".mail_content").html($(".mail_content").html()+"Content: \r\n" + content);
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
				addMailContent(data.content);
			} else if(data.status == "failure"){
				alert("Read Mail unsuccessfully!");
			}
			//alert(dat.username + ";" + dat.password);
			//alert(data.status);
		}   
	});  
}
function getUrlParam(name) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)"); //构造一个含有目标参数的正则表达式对象
    var r = window.location.search.substr(1).match(reg);  //匹配目标参数
    if (r != null) return unescape(r[2]); return null; //返回参数值
}
$(document).ready(function(){
	var mail_id = getUrlParam("mail_id");
	var time = getUrlParam("time");
	var subject = getUrlParam("subject")
	var file_id = getUrlParam("file_id");
	var file = getUrlParam("file");
	var file_name = decodeURI(getUrlParam("file_name"));
	var from = getUrlParam("from");
	var from_nickname = getUrlParam("from_nickname");
	var to = getUrlParam("to");
	var to_nickname = getUrlParam("to_nickname");
	$("button#appendix").attr("tag", file_id);
	
	describeMail(from_nickname+"<"+from+">", to_nickname+"<"+to+">", subject, file+" "+file_name, time)
//	alert(mail_id);
//	alert(time);
//	alert(subject);
//	alert(from);
//	alert(to_nickname);
	
	readMailContent(mail_id);
	$("button#appendix").click(function() {
//		var form=$("<form>");//定义一个form表单
		if(getUrlParam("file") == "true"){
			var form=$("<form>");
			form.attr("style","display:none");
			//form.attr("target","");
			form.attr("method","post");
			form.attr("action","/mailtest/servlet/DownloadFileServlet");
			var input1=$("<input>");
			input1.attr("type","hidden");
			input1.attr("name","file_id");
			input1.attr("value",getUrlParam("file_id"));
			var input2=$("<input>");
			input2.attr("type","hidden");
			input2.attr("name","file_name");
			input2.attr("value",decodeURI(getUrlParam("file_name")));
			$("body").append(form);//将表单放置在web中
			form.append(input1);
			form.append(input2);
			form.submit();//表单提交 
//			alert();
		} else {
			alert("This mail does not contain an appendix!");
		}
		

	});
	$("button#return").click(function() {
		window.location.href = document.referrer;
	});
});