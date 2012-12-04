
var z=display.appendChild(document.createElement("p"));
var D = 60;
var rangeSlider = document.getElementById("darkness-range");
var g=150;
var n=160;
var w=g*g;
var o=255;
var yPos=D;
xPos=-yPos;
var r=12832;
var B=2*Math.PI;

var drawMe = false;

$('#gradient-circle').bind('mousedown', function(e){
	e.preventDefault();
	drawMe = true;
	draw(e);
	$('#gradient-circle').bind('mousemove', function(e){
		if(drawMe) {
			draw(e);
		}
		else {
		
		}
	});
	$('#color-zoom').bind('mousemove', function(e){
		if(drawMe) {
			draw(e);
		}
		else {
		
		}
		
	});
});



$(document).bind('mouseup', function(e){
	drawMe = false;
});


function draw(e){

	xPos=e.pageX-circle.offsetLeft-container.offsetLeft-picker.offsetLeft-n-12;
	yPos=e.pageY-circle.offsetTop-container.offsetTop-n-18;
	var angle=Math.atan2(yPos,xPos);
	var t = xPos*xPos + yPos*yPos;
	// if outside the circle, constrain x and y
	t > w && (xPos = g*Math.cos(angle), yPos=g*Math.sin(angle), angle = Math.atan2(yPos,xPos), t = xPos*xPos + yPos*yPos);
	var rVal = String(getRGBValues((angle+Math.PI)/B,Math.sqrt(t)/g,rangeSlider.value/D)[0]).split('.')[0];
	var gVal = String(getRGBValues((angle+Math.PI)/B,Math.sqrt(t)/g,rangeSlider.value/D)[1]).split('.')[0];
	var bVal = String(getRGBValues((angle+Math.PI)/B,Math.sqrt(t)/g,rangeSlider.value/D)[2]).split('.')[0];
	color = 'rgb(' + rVal + ', ' + gVal + ', ' + bVal + ')';

 	
	if($('#picker').css('display') != 'none') {
     // FIXME: these are in colorWhilePicking()
     $('body').css('background-color', color);
		$('#color-zoom').css('left', xPos + 60);
		$('#color-zoom').css('top', yPos - 194);	
		$('#color-zoom').css('background-image', 'none');			
     $('#color-zoom').css('background-color', color);
     $('.currently-picking').css('background', color);
		$('.currently-picking').attr('data-yPos', $('#color-zoom').css('top'));
		$('.currently-picking').css('background-image', 'none');
		$('.currently-picking').attr('data-xPos', $('#color-zoom').css('left'));
     $('#virtual-blink').css('background-color', color);
		$('#r').val(rVal);
		$('#g').val(gVal);
		$('#b').val(bVal);
		
		// logging the color for you to use with the hardware settings here :o)
		//console.log(color);
        backendSetColor(color);
	}
}


function getRGBValues(angle,yPos,h){
	var angle=6*angle,xPos=~~angle,g=angle-xPos,angle=h*(1-yPos),i=h*(1-g*yPos),yPos=h*(1-(1-g)*yPos),rangeSlider=xPos%6,xPos=[h,i,angle,angle,yPos,h][rangeSlider]*o,g=[yPos,h,h,i,angle,angle][rangeSlider]*o,h=[angle,angle,yPos,h,h,i][rangeSlider]*o;
	return[xPos,g,h,"rgb ("+~~xPos+", "+~~g+", "+~~h+")", "rgba ("+~~xPos+", "+~~g+", "+~~h+", 1)"];
}
	
	
	
	
	
