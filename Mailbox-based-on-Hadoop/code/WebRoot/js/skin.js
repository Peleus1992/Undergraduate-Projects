function addComplex(skin, color, 
							hover_color, complex_id) {
//	$("textarea#test1").html($("textarea#test1").html()+"\r\n" +skin+color+hover_color+complex_id);
	var html = '<tr class="element">'
	+'<td><img class="big" src="'+skin+'" tag="'+complex_id+'"></img></td>'
		+'<td><div class="big" style="background-color: '+color+'"></div></td>'
		+'<td><div class="big" style="background-color: '+hover_color+'"></div></td>';
	$(".container .total table").append(html);
	
}
function readSkin(){
	$(".toast", parent.document).animate({
		top: "0px"
	});
	$.ajax({  
		type: "GET",
		url:"/mailtest/servlet/ReadComplexServlet", 
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
				var complex_list = data.array;
				for(var i = 0; i < complex_list.length; i++) {
					addComplex(complex_list[i].skin, complex_list[i].color, 
							complex_list[i].hover_color, complex_list[i].complex_id);
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
function changeSkin(complex_id) {
	$(".toast", parent.document).animate({
		top: "0px"
	});
	$.ajax({  
		type: "GET",
		url:"/mailtest/servlet/ChangeComplexServlet", 
		data: {
			complex_id: complex_id,
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
				parent.location.reload();
			} else if(data.status == "failure"){
				alert("Change skin unsuccessfully!");
			}
			//alert(dat.username + ";" + dat.password);
			//alert(data.status);
		}   
	});  
}
$(document).ready(function(){
	var hover_color = $(".bgi", parent.document).attr("tag2");
	readSkin();
//	$("button#test2").click(function() {
//		$(".bgi", parent.document).attr("src", "image/tulip.gif");
//		$(".bgi", parent.document).attr("tag1", "#ff0000");
//		$(".bgi", parent.document).attr("tag2", "rgba(255, 0, 0, 0.3)");
//		parent.location.reload();
//		changeSkin("00000000000020150423163611537103");
//		alert('<%=session.getAttribute("fs")%>');
//	});
	$(document).on("mouseover", ".container .total table tr.element", function() {
		$(this).css("background-color", hover_color);
		
	}).on("mouseleave", ".container .total table tr.element", function() {
		$(this).css("background-color", "rgba(0,0,0,0)");
		
	});
	$(document).on("click", ".container .total table .element img.big", function() {
		changeSkin($(this).attr("tag"));
		
	});
	
});