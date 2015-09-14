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
function addContact(nickname, mail_address) {
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

$(document).ready(function(){
	var font_color = $(".bgi", parent.document).attr("tag1");
	var hover_color = $(".bgi", parent.document).attr("tag2");
	sayHello();
	readFriend();
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
		window.location.href = "/mailtest/jsp/compose.jsp?mail_address=" + $(this).attr("tag");
	});
});