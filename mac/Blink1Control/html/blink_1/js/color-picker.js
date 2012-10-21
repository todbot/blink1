function m(d){

/*
	if() {
	
	}
	else {
*/
		xPos=d.pageX-canvas.offsetLeft-container.offsetLeft-picker.offsetLeft-n-12||xPos;
		yPos=d.pageY-canvas.offsetTop-container.offsetTop-n-8||yPos;
		d=s(yPos,xPos);
		t=xPos*xPos+yPos*yPos;
		t>w&&(xPos=g*Math.cos(d),yPos=g*Math.sin(d),d=s(yPos,xPos),t=xPos*xPos+yPos*yPos);
/* 	}	 */
	
	a.putImageData(E,0,0);
	a.font="2em bold arial";
	color = 'rgb(' + parseInt(A((d+u)/B,C(t)/g,j.value/D)[0]) + ', ' + parseInt(A((d+u)/B,C(t)/g,j.value/D)[1]) + ', ' + parseInt(A((d+u)/B,C(t)/g,j.value/D)[2]) + ')';
	a.fillStyle = color;
	z.textContent/* =doneColor.style.backgroundColor */=color;
	
	if($('#picker').css('display') != 'none') {
		$('body').css('background-color', color);
		$('.currently-picking').css('background-color', color);
		$('.currently-picking').attr('data-xPos', xPos);
		$('.currently-picking').attr('data-yPos', yPos);
		$('#virtual-blink').css('background-color', color);

        var rgbhex = colorToHex(color);
        var fadeurl = "/blink1/fadeToRGB/?rgb=" + encodeURIComponent(rgbhex) + "&time=" + 0.1;
        console.log("fadeurl="+fadeurl);
        jQuery.ajax(fadeurl);
	}
	
/* 	a.fillText("\u2295",xPos+n-4,yPos+n+4); */
	a.fillRect(xPos+n-4-11, yPos+n+4-21, 30, 30);
	a.strokeRect(xPos+n-4-11, yPos+n+4-21, 30, 30);
}


function colorToHex(color) {
    if (color.substr(0, 1) === '#') {
        return color;
    }
    var digits = /(.*?)rgb\((\d+), (\d+), (\d+)\)/.exec(color);
    
    var red = parseInt(digits[2]);
    var green = parseInt(digits[3]);
    var blue = parseInt(digits[4]);
    
    var rgb = blue | (green << 8) | (red << 16);
    return digits[1] + '#' + rgb.toString(16);
};


function A(d,yPos,h){
	var d=6*d,xPos=~~d,g=d-xPos,d=h*(1-yPos),i=h*(1-g*yPos),yPos=h*(1-(1-g)*yPos),j=xPos%6,xPos=[h,i,d,d,yPos,h][j]*o,g=[yPos,h,h,i,d,d][j]*o,h=[d,d,yPos,h,h,i][j]*o;
	return[xPos,g,h,"rgb ("+~~xPos+", "+~~g+", "+~~h+")", "rgba ("+~~xPos+", "+~~g+", "+~~h+", 1)"];
}
	

document.c=document.createElement;
display.a=display.appendChild;
p=canvas.width=canvas.height=320;
z=display.appendChild(document.c("p"));
j=display.appendChild(document.c("input"));
E=a.createImageData(p,p);
q=E.data,D=j.value=j.max=60,g=150,n=160;
w=g*g,o=255;
yPos=D;
xPos=-yPos;
r=12832;
u=Math.PI;
B=2*u;
C=Math.sqrt;
s=Math.atan2;
z.style.font='1em "Helvetica Neue", Helvetica, Arial, sans-serif';
/* j.type="range"; */
j.id="darkness-range";

for(y=j.min=0;y<p;y++)for(x=0;x<p;x++){
	i=x-g;
	v=y-g;
	F=i*i+v*v;
	i=A((s(v,i)+u)/B,C(F)/g,1);
	q[r++]=i[0];
	q[r++]=i[1];
	q[r++]=i[2];
	q[r++]=F>w?0:o
}

j.onchange=m;
canvas.onmousedown=function(d){
	d.preventDefault();
	document.onmousemove=
	/p/.test(d.type)?0:(m(d),m)};
	m(0)




document.onmouseup=function(d){
	document.onmousemove=
	/p/.test(d.type)?0:(m(d),m)};
	m(0)

