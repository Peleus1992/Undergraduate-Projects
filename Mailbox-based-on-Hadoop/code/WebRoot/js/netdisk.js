var file_list_num = 0;
var cur_page = 0;
var page_size = 16;
var file_list;
function addFile(filename, size, time, id) {
	var html = '<tr class="element"><td><input class="ele_checkbox" type="checkbox" tag="'+id+'"></td>'
		+'<td class="ele_filename">'+filename+'</td>'
			+'<td>'+size+' bytes'+'</td>'
			+'<td>'+time+'</td></tr>';
	$(".container .middle table").append(html);
	
}
function addContact(nickname, mail_address, friend_id) {
	var html = '<li class="element"><span>'+nickname+'&lt' +
	mail_address+'&gt</span><img class="left" tag="'+mail_address+'"></img><img class="right" tag="'+friend_id+'"></img></li>';
	$(".container .right ul.list").append(html);
//	$(".container .right ul").append('<li><span>'+nickname+'&lt' 
//			+ '&gt</span><img tag="'+friend_id+'"></img></li>');
}
function readFriend() {

	$.ajax({  
		type: "GET",
		url:"/mailtest/servlet/ListFriendServlet",  
		dataType:'json', //很重要!!!.      预期服务器返回的数据类型  
		error:function(){

			alert("error");
    	},  
		success:function(data){  

			if(data.status == "success") {
				for(var i = 0; i < data.array.length; i++) {
					addContact(data.array[i].nickname, data.array[i].mail_address, data.array[i].friend_id);
				}
			} else if(data.status == "failure"){
				alert("Read friend unsuccessfully!");
			}
		}   
	});  
}
function readFile() {
	$(".toast", parent.document).animate({
		top: "0px"
	});
	$.ajax({  
		type: "GET",
		url:"/mailtest/servlet/ListFileServlet",  
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
				file_list_num = data.array.length;
				file_list = data.array;
				
				for(var i = 0; i < (file_list_num > page_size ? page_size : file_list_num); i++) {
					addFile(file_list[i].file_name, file_list[i].file_size, file_list[i].time, file_list[i].file_id);
				}
				cur_page = 0;
			} else if(data.status == "failure"){
				alert("Read file unsuccessfully!");
			}
			//alert(dat.username + ";" + dat.password);
			//alert(data.status);
		}   
	});  
}
$(document).ready(function(){
	var hover_color = $(".bgi", parent.document).attr("tag2");
	/*for(var i = 0; i < 20; i++) {
		addFile("helloworld.java", "19", new Date());
	}
	for(var i = 0; i < 20; i++) {
		addContact("Juice");
	}*/
	readFile();
	readFriend();
	$(document).on("mouseover", ".container .middle table tr.element", function() {
		$(this).css("background-color", hover_color);
		
	}).on("mouseleave", ".container .middle table tr.element", function() {
		$(this).css("background-color", "rgba(0,0,0,0)");
		
	});
	
	$(document).on("mouseover", ".container .right .list li.element", function() {
		$(this).css("background-color", hover_color);
		$(this).find("img").first().attr("src", "image/small_compose.jpg");
		$(this).find("img").first().show();
		$(this).find("img").last().attr("src", "image/small_share.png");
		$(this).find("img").last().show();
	}).on("mouseleave", ".container .right .list li.element", function() {
		$(this).css("background-color", "rgba(0,0,0,0)");
		$(this).find("img").first().removeAttr("src");
		$(this).find("img").first().hide();
		$(this).find("img").last().removeAttr("src");
		$(this).find("img").last().hide();
	});
	$(document).on("click", ".container .right .list .element img.left", function() {

		var info = "";
		var items = $('[class = "ele_checkbox"]:checkbox:checked'); 
		if(items.length > 1) {
			alert("You could only attach one copy of file in one mail!");
			return;
		} else if(items.length < 1) {
			alert("You should at least select one copy of file!");
			return;
		}
		var file_id = items.get(0).attributes["tag"].value;
		var file_name = encodeURI(encodeURI($(items).first().parent().next().html()));
		
		window.location.href = "/mailtest/jsp/compose.jsp?mail_address=" + $(this).attr("tag")
			+"&file_id="+file_id
			+"&file_name="+file_name;
	
	}).on("click", ".container .right .list .element img.right", function() {
		var info = "";
		var items = $('[class = "ele_checkbox"]:checkbox:checked'); 
		if(items.length < 1) {
			alert("You should at least select one copy of file!");
			return;
		}
		for (var i = 0; i < items.length; i++) {
		     // 如果i+1等于选项长度则取值后添加空字符串，否则为逗号
		     info = (info + items.get(i).attributes["tag"].value) + (((i + 1)== items.length) ? '':','); 
		}
		$(".toast", parent.document).animate({
			top: "0px"
		});
		$.ajax({  
			type: "GET",
			url:"/mailtest/servlet/ShareFileServlet",  
			data:{
				file_list: info,
				friend_id: $(this).attr("tag")
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
					alert("Share file successfully!");
					
				} else if(data.status == "failure"){
					alert("Share file unsuccessfully!");
				}
			}   
		}); 
		
	});
	$(".title_checkbox").click(function(){
		var ele = $(".container .middle table").find(".ele_checkbox");
		if($(this).is(":checked")) {
			ele.each(function(i) {
				$(this).prop("checked", true);
			});
		} else {
			ele.each(function(i) {
				$(this).prop("checked", false);
			});
		}
	});
	$(".ele_filename").click(function() {
		alert();
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
						$("input#file_text").val("");
						window.location.href = "/mailtest/jsp/netdisk.jsp";
					}
				}
			});
		}
	});
	$("a#next_page").click(function() {
		if((cur_page + 1) * page_size < file_list_num) {
			$("tr").remove(".element");
			cur_page = cur_page + 1;
			var num = file_list_num - (cur_page) * page_size;
			for(var i = 0; i < ((num > page_size) ? page_size : num); i++) {
				var e = file_list[cur_page * page_size + i];
				addFile(e.file_name, e.file_size, e.time, e.file_id);
			}
			
		} else {
			alert("There is no next page");
		}
	});
	$("a#previous_page").click(function() {
		if(cur_page > 0) {
			$("tr").remove(".element");
			cur_page = cur_page - 1;
			for(var i = 0; i < page_size; i++) {
				var e = file_list[(cur_page) * page_size + i];
				addFile(e.file_name, e.file_size, e.time, e.file_id);
			}
		} else {
			alert("There is no previous page");
		}
	});
	$("button#deleteFile").click(function() {
		var info = "";
		var items = $('[class = "ele_checkbox"]:checkbox:checked'); 
		
		for (var i = 0; i < items.length; i++) {
		     // 如果i+1等于选项长度则取值后添加空字符串，否则为逗号
		     info = (info + items.get(i).attributes["tag"].value) + (((i + 1)== items.length) ? '':','); 
		}
		$(".toast", parent.document).animate({
			top: "0px"
		});
		$.ajax({  
			type: "GET",
			url:"/mailtest/servlet/DeleteFileServlet",  
			data:{
				file_list: info
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
					window.location.href = "/mailtest/jsp/netdisk.jsp";
				} else if(data.status == "failure"){
					alert("Delete file unsuccessfully!");
				}
			}   
		}); 
	});
	$(document).on("click", ".ele_filename", function() {
		var i = $(".ele_filename").index(this) + cur_page * page_size;
		var form=$("<form>");
		form.attr("style","display:none");
		form.attr("method","post");
		form.attr("action","/mailtest/servlet/DownloadFileServlet");
		var input1=$("<input>");
		input1.attr("type","hidden");
		input1.attr("name","file_id");
		input1.attr("value",$(this).parent().find(".ele_checkbox").attr("tag"));
		var input2=$("<input>");
		input2.attr("type","hidden");
		input2.attr("name","file_name");
		input2.attr("value",$(this).html());
		$("body").append(form);//将表单放置在web中
		form.append(input1);
		form.append(input2);
		form.submit();//表单提交 
//		alert();
	});
});