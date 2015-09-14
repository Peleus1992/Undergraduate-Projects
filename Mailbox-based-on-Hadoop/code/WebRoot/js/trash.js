var mail_list_num = 0;
var cur_page = 0;
var page_size = 12;
var mail_list;
var mail_address;
function addMail(read,file, file_id, from, from_nickname, to, to_nickname, subject, time, star, mail_id) {
	var html = '<tr class="element"><td><input class="ele_checkbox" type="checkbox" tag="'+mail_id+'"></td>'
		+'<td>'+((file=="true")?'<img class="ele_file" src="image/small_file.png" tag="'+file_id+'"></img>':'')+'</td>'
			+'<td>'+from_nickname+'&lt'+from+'&gt'+'</td>'
			+'<td>'+to_nickname+'&lt'+to+'&gt'+'</td>'
			+'<td class="ele_subject" tag="'+from+'">'+subject+'</td>'
			+'<td>'+time+'</td>'
			+'</tr>';
	$(".container .total table").append(html);
	
}
function readMailList(){
	$(".toast", parent.document).animate({
		top: "0px"
	});
	$.ajax({  
		type: "GET",
		url:"/mailtest/servlet/ListMailServlet", 
		data: {
			to_position: "trash",
			from_position: "trash"
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
				mail_list_num = data.array.length;
				mail_list = data.array;
				mail_address = data.mail_address;
				for(var i = 0; i < (mail_list_num > page_size ? page_size : mail_list_num); i++) {
					addMail(mail_list[i].read, mail_list[i].file, mail_list[i].file_id
							, mail_list[i].from, mail_list[i].from_nickname
							, mail_list[i].to, mail_list[i].to_nickname
							, mail_list[i].subject, mail_list[i].time
							, mail_list[i].star, mail_list[i].mail_id);
				}
				cur_page = 0;
			} else if(data.status == "failure"){
				alert("Read Mail unsuccessfully!");
			}
			//alert(dat.username + ";" + dat.password);
			//alert(data.status);
		}   
	});  
}
function changeMailState(var_mail_id_list, var_to_position, var_from_position,
		var_to_read, var_from_read, var_to_star, var_from_star){
	$(".toast", parent.document).animate({
		top: "0px"
	});
	$.ajax({  
		type: "GET",
		url:"/mailtest/servlet/ChangeMailServlet", 
		data: {
			mail_id_list: var_mail_id_list,
			to_position: var_to_position,
			from_position: var_from_position,
			to_read: var_to_read,
			from_read: var_from_read,
			to_star: var_to_star,
			from_star: var_from_star
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
				window.location.href = "/mailtest/jsp/trash.jsp";
			} else if(data.status == "failure"){
				alert("Read Mail unsuccessfully!");
			}
			//alert(dat.username + ";" + dat.password);
			//alert(data.status);
		}   
	});  
}
function deleteMail(mail_id_list) {
	$(".toast", parent.document).animate({
		top: "0px"
	});
	$.ajax({  
		type: "GET",
		url:"/mailtest/servlet/DeleteMailServlet", 
		data: {
			mail_id_list: mail_id_list,
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
				window.location.href = "/mailtest/jsp/trash.jsp";
			} else if(data.status == "failure"){
				alert("Read Mail unsuccessfully!");
			}
			//alert(dat.username + ";" + dat.password);
			//alert(data.status);
		}   
	});  
}
$(document).ready(function(){
	var hover_color = $(".bgi", parent.document).attr("tag2");
//	addMail("true","true", "Juice", "Helloworld", new Date(), "true");
	
//	addMail("false","false", "Juice", "Helloworld", new Date(), "false");
//	for(var i = 0; i < 10; i++) {
//		addMail("false","true", "Juice", "Helloworld", new Date(), "false");
//	}
	readMailList();
	
	
	$(document).on("mouseover", ".container .total table tr.element", function() {
		$(this).css("background-color", hover_color);
		
	}).on("mouseleave", ".container .total table tr.element", function() {
		$(this).css("background-color", "rgba(0,0,0,0)");
		
	});
	$(".title_checkbox").click(function(){
		var ele = $(".container .total table").find(".ele_checkbox");
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
	
	
	$("button#restore").click(function() {
		var items = $('[class = "ele_checkbox"]:checkbox:checked'); 
		if(items.length == 0) {
			alert("You should at least select one mail!");
			return;
		} 
		var mail_id_list1 = "";
		var mail_id_list2 = "";
		
		for (var i = 0; i < items.length; i++) {
		     // 如果i+1等于选项长度则取值后添加空字符串，否则为逗号
			if($(items.get(i)).parent().parent().find(".ele_subject").attr("tag") == mail_address) {
				mail_id_list1 = (mail_id_list1 + items.get(i).attributes["tag"].value) + (((i + 1)== items.length) ? '':',');
			} else {
				mail_id_list2 = (mail_id_list2 + items.get(i).attributes["tag"].value) + (((i + 1)== items.length) ? '':',');
			}
		}
//			alert(mail_address);
		if(mail_id_list1 != "") {
			changeMailState(mail_id_list1, "", "outbox", "", "", "", "");
		} 
		if(mail_id_list2 != "") {
			changeMailState(mail_id_list2, "inbox", "", "", "", "", "");
		}
		
	});
	$(document).on("click", ".ele_subject", function() {
		//alert($(this).parent().find("input.ele_checkbox").attr("tag"));
		var i = $(".ele_subject").index(this) + cur_page * page_size;
		
		//alert(mail_list[i].subject);
		var html = "/mailtest/jsp/content.jsp?mail_id="+mail_list[i].mail_id+"&time="+mail_list[i].time
									+"&subject="+mail_list[i].subject+"&file_id="+mail_list[i].file_id
									+"&file="+mail_list[i].file+"&file_name="+encodeURI(encodeURI(mail_list[i].file_name))
									+"&from="+mail_list[i].from+"&from_nickname="+mail_list[i].from_nickname
									+"&to="+mail_list[i].to+"&to_nickname="+mail_list[i].to_nickname;
		
		window.location.href = html;
	});
	$("a#next_page").click(function() {
		if((cur_page + 1) * page_size < mail_list_num) {
			$("tr").remove(".element");
			cur_page = cur_page + 1;
			var num = mail_list_num - (cur_page) * page_size;
			for(var i = 0; i < ((num > page_size) ? page_size : num); i++) {
				var e = mail_list[cur_page * page_size + i];
				addMail(e.read, e.file
						, e.from, e.nickname
						, e.subject, e.time
						, e.star, e.mail_id);
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
				var e = mail_list[(cur_page) * page_size + i];
				addMail(e.read, e.file
						, e.from, e.nickname
						, e.subject, e.time
						, e.star, e.mail_id);
			}
		} else {
			alert("There is no previous page");
		}
	});
	$("button#delete_file").click(function(){
		var mail_id_list = "";
		var items = $('[class = "ele_checkbox"]:checkbox:checked'); 
		if(items.length == 0) {
			alert("You should at least select one mail!");
			return;
		} 
		for (var i = 0; i < items.length; i++) {
		     // 如果i+1等于选项长度则取值后添加空字符串，否则为逗号
			mail_id_list = (mail_id_list + items.get(i).attributes["tag"].value) + (((i + 1)== items.length) ? '':','); 
		}
		deleteMail(mail_id_list);
	});
});