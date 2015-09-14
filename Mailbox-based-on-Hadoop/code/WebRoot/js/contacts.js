function addContact(nickname, mail, phone, friend_id) {
	var html = '<tr class="element"><td><input class="ele_checkbox" type="checkbox" tag="'+friend_id+'"></td>'
		+'<td>'+nickname+'</td>'
			+'<td>'+mail+'</td>'
			+'<td>'+phone+'</td></tr>';
	$(".container .total table").append(html);
	
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
					addContact(data.array[i].nickname, data.array[i].mail_address, data.array[i].phone, data.array[i].friend_id);
				}
			} else if(data.status == "failure"){
				alert("Read friend unsuccessfully!");
			}
			//alert(dat.username + ";" + dat.password);
			//alert(data.status);
		}   
	});  
}
$(document).ready(function(){
	var hover_color = $(".bgi", parent.document).attr("tag2");
	/*for(var i = 0; i < 20; i++) {
		addContact("Juice", "derek@myhadoop.com", "13352282701");
	}*/
	readFriend();
	
	
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
	$("button#delete").click(function(){
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
			url:"/mailtest/servlet/DeleteFriendServlet",  
			data:{
				friend_list: info
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
					alert("Delete friend successfully!");
					window.location.href = "/mailtest/jsp/contacts.jsp";
				} else if(data.status == "failure"){
					alert("Delete friend unsuccessfully!");
				}
			}   
		}); 
	});
	$("button#add").click(function(){
		if($("input#mail_address").val() != "") {
			$(".toast", parent.document).animate({
				top: "0px"
			});
			$.ajax({  
				type: "GET",
				url:"/mailtest/servlet/AddFriendServlet",  
				data:{
					mail_address: $("input#mail_address").val()
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
						alert("Add friend successfully!");
						window.location.href = "/mailtest/jsp/contacts.jsp";
					} else if(data.status == "failure"){
						alert("Delete friend unsuccessfully! The mail address does not exist!");
					}
				}   
			}); 
		} else {
			alert("Mail address should not be empty!");
		}
		
	});
});