
z=display.appendChild(document.createElement("p"));
rangeSlider=display.appendChild(document.createElement("input"));
D=rangeSlider.value=rangeSlider.max=60;
g=150;
n=160;
w=g*g;
o=255;
yPos=D;
xPos=-yPos;
r=12832;
B=2*Math.PI;
rangeSlider.id="darkness-range";


/*
gradient.onmousedown=function(e){
	e.preventDefault();
//	$('#color-zoom').css('background-color', 'transparent');
	document.onmousemove=draw;
};


document.onmouseup=function(e){
	document.onmousemove=draw(e);
//	$('#color-zoom').css('background-color', color);
};

*/

gradient.onmousedown=function(e){
	e.preventDefault();
	document.onmousemove=
	/p/.test(e.type)?0:(draw(e),draw)};
	draw(0)




document.onmouseup=function(e){
	document.onmousemove=
	/p/.test(e.type)?0:(draw(e),draw)};
	draw(0)


function draw(e){

	xPos=e.pageX-circle.offsetLeft-container.offsetLeft-picker.offsetLeft-n-12;
	yPos=e.pageY-circle.offsetTop-container.offsetTop-n-8;
	angle=Math.atan2(yPos,xPos);
	t = xPos*xPos + yPos*yPos;
	// if outside the circle, constrain x and y
	t > w && (xPos = g*Math.cos(angle), yPos=g*Math.sin(angle), angle = Math.atan2(yPos,xPos), t = xPos*xPos + yPos*yPos);
	var rVal = String(getRGBValues((angle+Math.PI)/B,Math.sqrt(t)/g,rangeSlider.value/D)[0]).split('.')[0];
	var gVal = String(getRGBValues((angle+Math.PI)/B,Math.sqrt(t)/g,rangeSlider.value/D)[1]).split('.')[0];
	var bVal = String(getRGBValues((angle+Math.PI)/B,Math.sqrt(t)/g,rangeSlider.value/D)[2]).split('.')[0];
	color = 'rgb(' + rVal + ', ' + gVal + ', ' + bVal + ')';

	
	if($('#picker').css('display') != 'none') {
		$('body').css('background-color', color);
		$('#color-zoom').css('left', xPos + 60);
		$('#color-zoom').css('top', yPos - 194);	
		$('#color-zoom').css('background-color', color);
		$('.currently-picking').css('background', color);
		$('.currently-picking').attr('data-yPos', $('#color-zoom').css('top'));
		$('.currently-picking').attr('data-xPos', $('#color-zoom').css('left'));
		$('#virtual-blink').css('background-color', color);
		$('#r').val(rVal);
		$('#g').val(gVal);
		$('#b').val(bVal);
		
		// logging the color for you to use with the hardware settings here :o)
		console.log(color);
	}
}


function getRGBValues(angle,yPos,h){
	var angle=6*angle,xPos=~~angle,g=angle-xPos,angle=h*(1-yPos),i=h*(1-g*yPos),yPos=h*(1-(1-g)*yPos),rangeSlider=xPos%6,xPos=[h,i,angle,angle,yPos,h][rangeSlider]*o,g=[yPos,h,h,i,angle,angle][rangeSlider]*o,h=[angle,angle,yPos,h,h,i][rangeSlider]*o;
	return[xPos,g,h,"rgb ("+~~xPos+", "+~~g+", "+~~h+")", "rgba ("+~~xPos+", "+~~g+", "+~~h+", 1)"];
}
	
	
	
	
	