/*********************************

	DEMO PAGE BEHAVIOR

*********************************/

var demoColor = new Object();
	demoColor.h = 0;
	demoColor.s = 0;
	demoColor.v = 0;
var rgbCurr = new Object();
	rgbCurr.r = 0;
	rgbCurr.g = 0;
	rgbCurr.b = 0;	
	
var intervalLength = 25;
var cycleInterval;

var	timeElapsed = 0;
var pct, increment, durationInSeconds, fadeColor1, fadeColor2, tempColor, numColors, lum;
var fadeColors = [];
var fadeDurations = [];
	pct = 0;		
var currIndex1 = 0;
var currIndex2 = 1;

$('.demo-grid-square').live('click', function(e) {
	var buttonName = $(this).attr('id').replace('demo-', '');
	$('.demo-grid-square.active').removeClass('active');
	$(this).addClass('active');
	runDemoEffect(buttonName);

});	
 
$('#configuration-popup.demo input.test').live('click', function(e) {
	var testSettings = compileSettings();
	var colorsObj = testSettings.colorSettings.colors;
/* 	var durationsObj = testSettings.colorSettings.durations; */
	var testColors = [];
	var testDurations = [];
	console.log(testSettings);
	for(var key in colorsObj) {
		/* console.log(colorsObj[key].split('(')[1].split(',')[0]); */
		var colorArray = [parseInt(colorsObj[key].split('(')[1].split(',')[0]), parseInt(colorsObj[key].split('(')[1].split(',')[1]), parseInt(colorsObj[key].split('(')[1].split(',')[2].split(')')[0])];	
		testColors.push(colorArray);

	}

	$(testSettings.colorSettings.durations).each(function(index, value) {
		testDurations.push(parseFloat(value));
	});
	console.log(testDurations);
	setCycleValues(testColors, testDurations);
	playColorCycle();
});

$('#navbar li').live('click', function(e) {
	$('#main-content').attr('style', '');
	clearInterval(cycleInterval);
});

/*
 $('#configuration-popup.demo a.cancel').live('click', function(e) {
 	$('#configuration-popup.demo').removeClass('demo');
 });
*/

/*************************************

	FUNCTIONS: DEMO PAGE

*************************************/

function setCycleValues(colors, durations) {
/* console.log(colors); */
	numColors = colors.length;
	if (numColors < 2) {
		colors.push([0, 0, 0]);
		durations.push(durations[0]);		
		numColors = 2;
	}
	fadeColors = colors;
	fadeDurations = durations;
	durationInSeconds = durations[0];
	increment = 1/(durationInSeconds*(1000/intervalLength));
	cycleInterval = setInterval(playColorCycle, intervalLength);			
}

function playColorCycle() {
	timeElapsed += intervalLength;
	demoColor.r = (1-pct)*fadeColors[currIndex1][0] + pct*fadeColors[currIndex2][0];
	demoColor.g = (1-pct)*fadeColors[currIndex1][1] + pct*fadeColors[currIndex2][1];
	demoColor.b = (1-pct)*fadeColors[currIndex1][2] + pct*fadeColors[currIndex2][2];	
	increment = 1/(fadeDurations[currIndex1]*(1000/intervalLength));
	
	pct += increment;
	
	if(pct > (1 - increment)) {
		// reset percentage for next color transition
		pct = 0;
		if(currIndex2 > currIndex1) {
			if(currIndex2 < numColors - 1) {
				currIndex1 ++;
				currIndex2 ++;			
			}
			else {
				currIndex1 ++;
				currIndex2 = 0;					
			}
		}
		else {
			currIndex1 = 0;
			currIndex2  = 1;
		}
	}
	// calculate luminosity for potential use
	var tempHsl = rgbToHsl(demoColor.r, demoColor.g, demoColor.b);
	lum = tempHsl[2];
	var tempColor = 'rgb(' + parseInt(demoColor.r) + ', ' + parseInt(demoColor.g) + ', ' + parseInt(demoColor.b) + ')';
/* 	console.log(tempColor); */
	$('#virtual-blink-demo').css('background-color', tempColor );
	/* $('#main-content').css('background-color', 'rgba(220, 220, 220, ' + (1- lum - .1) + ')' ); */
    backendSetColor( tempColor );

}	


function runDemoEffect(demoName) {
	// reset values whenever a demo button is pressed 
	clearInterval(cycleInterval);
	pct = 0;
	currIndex1 = 0;
	currIndex2 = 1;	
	// cases for different buttons
	switch(demoName) {
		case 'light-on':
			//console.log('rgb(255, 255, 255)');
			$('#virtual-blink-demo').css('background-color', 'white');
			$('#virtual-blink-overlay-demo').removeClass('dark');
			$('#main-content').attr('class', '').attr('style', '');
            backendSetColor( 'rgb(255, 255, 255)' );
			break;
		case 'light-off':
			//console.log('rgb(0, 0, 0)');
			$('#virtual-blink-overlay-demo').addClass('dark');
			$('#virtual-blink-demo').css('background-color', '#000');
			$('#main-content').attr('style', '').attr('class', 'dark');
            backendSetColor( 'rgb(0, 0, 0)' );
			break;
		case 'rgb':
			$('#virtual-blink-overlay-demo').removeClass('dark');
			$('#main-content').attr('class', '');
			setCycleValues([[255, 0, 0],[0, 255, 0], [0, 0, 255]],[2, 2, 2]);
			playColorCycle();											
			break;	
		case 'mood':
			$('#virtual-blink-overlay-demo').removeClass('dark');
			$('#virtual-blink-demo').css('background-color', 'magenta');
			$('#main-content').attr('class', '');
            var moodColors = [];
            var moodDurations = [];
            for(var i = 0; i < 20; i++){
                var r = Math.floor( (Math.random()*255) );
                var g = Math.floor( (Math.random()*255) );
                var b = Math.floor( (Math.random()*255) );
                moodColors.push( [r,g,b] );
                moodDurations.push( 1 );
            }
            console.log(moodColors);
            setCycleValues( moodColors, moodDurations );
			//setCycleValues( [[255, 84, 229], [35, 138, 255]], [1, 1]);
			playColorCycle();										
			break;	
		case 'color-picker':
			$('#virtual-blink-overlay-demo').removeClass('dark');
			$('#main-content').attr('class', '');	
			openColorPanel();								
			break;																
	}
}


/*-------------------------
	OPEN POPUP FUNCTIONS
--------------------------*/

function openColorPanel() {
	// hide configuration related columns to use as blank color picker
/* 	resetConfigurationPanel(); */
	$('#popup-title > input').val('Color Picker');
	$('#configuration-popup').removeClass('existing').removeClass('new').addClass('demo');	
	$('#configuration-popup #submit-options-buttons input.save-as-new').val('new trigger');		
	// and finally, open the panel
	$('#gray-out').fadeIn('fast');
	$('#configuration-popup').fadeIn('fast');
	$('#picker').fadeIn('fast');	
}	



