$(document).ready(function(){

    var blink1Settings = new Object;
	var swatchId = '';

    var triggerObjects = [];

	// load previously-saved triggerObjects from back-end 
    var newTriggerObjects = backendLoadTriggers();

    // add them to the screen
    $(newTriggerObjects).each(function(index) {
            addNewTrigger(this);
            hideWelcomeMessageIfNeeded(); // hack, should only need to do this once
        });

    // load up the blink1_id info
    backendLoadBlink1Settings();

    var liveValue = new Object; // FIXME: should be ref to current obj
    liveValue.source = new Object;

    
/*********************************

	NAVBAR TABS BEHAVIOR

*********************************/

	// active tabs behavior for ajax transitions (jquery-ui)
	$( "#tabs" ).tabs({
		hide: false,
		show: true,
        beforeLoad: function( event, ui ) {
            ui.jqXHR.error(function() {
                ui.panel.html("Couldn't load this tab. We'll try to fix this as soon as possible." );
            });
        }
    });
    
    $('.color-swatch').css('width', 126);
    $('#color-1').addClass('currently-picking');

    $('#demo-tab').live('click', function(e) {
            $('#demo-light-on').click(); 	// reset demo page back to default
        });

	$('#navbar li').live('click', function(e) {
$("#tabs #navbar a").removeData("cache.tabs");
	// settings tab
		if($(this).attr('id') == 'settings-tab') {
			// if settings tab already active, close it
			if($(this).hasClass('active')){
				$('#settings-popup').fadeOut('fast');
				$('#gray-out').fadeOut('fast');	
				$('#navbar').css('z-index', 0);
				$(this).removeClass('active');	
				$('.ui-state-active').addClass('active');
			} else {
                // otherwise, get settings & trigger popup/opacity effect
                backendLoadBlink1Settings();
                $('#about-status').text( blink1Settings.statustext );
                $('#about-serial-number').text( blink1Settings.serialnum );
                $('#about-ifttt-uid').text( blink1Settings.blink1_id );

				$('#settings-popup').fadeIn('fast');
				$('#gray-out').fadeIn('fast');	
				$('#navbar').css('z-index', 5);	
				$('#navbar li.active').removeClass('active');
				$(this).addClass('active');	
			}	
		// other tabs
		} 	else {
			if ($('#settings-tab').hasClass('active')){
				$('#settings-popup').fadeOut('fast');
				$('#gray-out').fadeOut('fast');
				$('#navbar').css('z-index', 0);		
			}
			$('#navbar li.active').removeClass('active');
			$(this).addClass('active');
		}
		$('#container').attr('class', '');	
		// give tab-based class to container so that styling can be dependent on active tab
		$('#container').addClass($('.active').attr('id').split('-')[0]);

	});
	
	
/*********************************

	HOMEPAGE BUTTONS/BEHAVIOR

*********************************/

	// add a new trigger (clear config panel and open)
	$('#add-new-trigger').live('click', function(e) {
		resetConfigurationPanel();
		logSettings();
		openConfigurationPanel();
	});

	// edit existing (apply existing settings to panel and open)
	$('.edit-trigger-button').live('click', function(e) {
		var index = $(this).closest('.trigger-options').index();
		var id = $(this).closest('.trigger-options').attr('id').split('-')[1];
		openConfigurationPanel(triggerObjects[index], id, index);
	});

	// delete a trigger from the homepage
	$('.delete-trigger').live('click', function(e) {
		var $triggerObject = $(this).parents('.trigger-options');
		var index = $(this).closest('.trigger-options').index();

        backendDeleteTrigger( triggerObjects[index] );

		// hide the deleted trigger from the view
		$triggerObject.slideUp('normal', function() {
			$triggerObject.remove();
			if($('#trigger-list .trigger-options').length < 1) {
				$('#welcome-message').fadeIn('slow');
				$('#trigger-headers li').css('opacity', .2);
				
			}
			//else if($('#trigger-list .trigger-options').length < 6) {
			//	$('#scroll-for-more').fadeOut('fast');
			//}
		});

		// and then delete that trigger's object from the triggers array
		triggerObjects.splice( ((index==0)?0:(index-1)), 1);
        console.log("delete-trigger");
		console.log(triggerObjects);
	});
	
/*********************************

	CONFIGURATION POPUP BEHAVIOR

*********************************/
	
	
	// make the color swatch order sortable inside the configuration interface
	$('#color-swatches-container').sortable();
	$( "#sortable" ).disableSelection();
	

	// cancel out of popup
	$('#configuration-popup a.cancel').live('click', function(e) {
		closeConfigurationPanel();		
	});
	
	
    //
    function hideWelcomeMessageIfNeeded() {
		// if it's the first trigger, get rid of the welcome message on the homepage
		if($('#trigger-list .trigger-options').length < 2) {
			$('#welcome-message').hide();
			$('#trigger-headers li').css('opacity', 1);
		}
    }

	/*--------------------
		SAVE SETTINGS
	---------------------*/


    function addNewTrigger(settings) { 

			// push this new trigger to the triggerObjects array
			triggerObjects.push(settings);
			
			// if not the first one, then give the next available sequential id number (need to check since some could have been deleted from the middle of the sequence and we don't want duplicates)
			var currentID;
			if($('#trigger-list .trigger-options').length > 0) {	
				var idOfLast = $('#trigger-list .trigger-options').last().attr('id').split('-')[1];
				currentID = parseInt(idOfLast) + 1;
				$('#trigger-list').append($('#trigger-options-to-clone').clone());
				$('#trigger-list .trigger-options').last().attr('id', 'trigger-' + currentID);
			}		
			else { // if this is the first one we can skip the checking step and just label it trigger-1
				$('#trigger-list').append($('#trigger-options-to-clone').clone());
				$('#trigger-list .trigger-options').last().attr('id', 'trigger-1');
				currentID = 1;
			}	

			// then color the summary swatches on the main homepage
			$('#trigger-list .trigger-options').last().find('.summary-color-swatch').removeClass('active');
			resizeSwatches(settings.colorSettings.colors.length, '#trigger-' + currentID + ' .summary-color-swatch', 120);
			$(settings.colorSettings.colors).each(function(index){
				if(this == 'rgb(0, 0, 0)') {
					$($('#trigger-list .trigger-options').last().find('.summary-color-swatch')[index]).css('background-color', '#999').addClass('light-off').html(settings.colorSettings.durations[index]);
				} else {
					$($('#trigger-list .trigger-options').last().find('.summary-color-swatch')[index]).css('background-color', this).removeClass('light-off').html(settings.colorSettings.durations[index]);
				}
			});
			// and print out the text settings
			$('#trigger-list .trigger-options').last().find('.trigger-name').html(settings.title);
			$('#trigger-list .trigger-options').last().find('.number-of-repetitions').html(settings.colorSettings.repeatTimes);
			// change icon if only plays once
			if(settings.colorSettings.repeatTimes < 2) {
				$('#trigger-list .trigger-options').last().find('.repeat-option').removeClass('true').addClass('false');
			} else {
				$('#trigger-list .trigger-options').last().find('.repeat-option').removeClass('false').addClass('true');	
			}

			// and check for scroll reminder  (FIXME: this check belongs elsewhere)
			//if($('#trigger-list .trigger-options').length > 5) {
			//	$('#scroll-for-more').fadeIn('slow');
			//}

			if(settings.source.arg1) {
                $('#trigger-list .trigger-options').last().find('.trigger-source').html('<span class="light">[' + settings.source.type.toUpperCase() + ']</span> ' + settings.source.arg1.replace('http://', '').replace('www.', ''));
			}
			else if (settings.source.type) {
				$('#trigger-list .trigger-options').last().find('.trigger-source').html('<span class="light">[' + settings.source.type.toUpperCase() + ']</span> ');
			}
    }


	$('#configuration-popup .save-button').live('click', function(e) {
console.log("save click!");
		closeConfigurationPanel();
        
		// get all the settings we just configured wrapped into a nice little object
		var settings = compileSettings();
		
        hideWelcomeMessageIfNeeded();

		/*------if we're CREATING A NEW TRIGGER------*/
		
		if($(this).hasClass('save-as-new')) {

            if( $(this).val() != 'save' ) { // FIXME: super big hack
                settings.title = settings.title + " copy";
            }
            addNewTrigger(settings);

        }

		/*-------otherwise if we're SAVING AN EXISTING TRIGGER----*/
		
		else if($(this).hasClass('save-changes')) {
			var indexToReplace = $('#popup-title').attr('data-array-index'); 
			var idToReplace = $('#popup-title').attr('data-id');
			// swap out the old object with the new one we just modified
			triggerObjects.splice(indexToReplace, 1, settings); 
			
			// color the summary swatches on the main homepage for the one we edited
			
			$('#trigger-list .trigger-options#trigger-' + idToReplace).find('.summary-color-swatch').removeClass('active');
			resizeSwatches(settings.colorSettings.colors.length, '#trigger-list .trigger-options#trigger-' + idToReplace + ' .summary-color-swatch', 120);
			$(settings.colorSettings.colors).each(function(index){
				if(this == 'rgb(0, 0, 0)') {
					$($('#trigger-list .trigger-options#trigger-' + idToReplace).find('.summary-color-swatch')[index]).css('background-color',  '#999').addClass('light-off').html(settings.colorSettings.durations[index]);
				} else {
					$($('#trigger-list .trigger-options#trigger-' + idToReplace).find('.summary-color-swatch')[index]).css('background-color', this).removeClass('light-off').html(settings.colorSettings.durations[index]);
				}
			});

			// and change out the text settings to reflect new inputs
			$('#trigger-list .trigger-options#trigger-' + idToReplace).find('.trigger-name').html(settings.title);
			$('#trigger-list .trigger-options#trigger-' + idToReplace).find('.number-of-repetitions').html(settings.colorSettings.repeatTimes);
			if(settings.colorSettings.repeatTimes < 2) {
				$('#trigger-list .trigger-options#trigger-' + idToReplace).find('.repeat-option').removeClass('true').addClass('false');
			} else {
				$('#trigger-list .trigger-options#trigger-' + idToReplace).find('.repeat-option').removeClass('false').addClass('true');	
			}
		
			if(settings.source.arg1) {	// FIXME: this is basically dupe of code in addNewTrigger()
                $('#trigger-list .trigger-options#trigger-' + idToReplace).find('.trigger-source').html('<span class="light">[' + settings.source.type.toUpperCase() + ']</span> ' + settings.source.arg1.replace('http://', '').replace('www.', ''));
			}
			else if (settings.source.type) {
				$('#trigger-list .trigger-options#trigger-' + idToReplace).find('.trigger-source').html('<span class="light">[' + settings.source.type.toUpperCase() + ']</span> ');
			}

		}

		//console.log(triggerObjects);
        backendUpdateTriggers(triggerObjects); 
	});
	
	
	/*--------------------
		COLOR SETTINGS
	---------------------*/ 
	
	// if user changes number of swatches, resize the swatches to fit
	$('#number-of-colors-group').change(function(e){
		resizeSwatches($(this).val(), '.color-swatch', 127);
	});
	
	// change language to match selected behavior setting on change
	$('#behavior-selector select').change(function(e){
		if($(this).val() == 'blink-pattern') {
			$('#color-selector label[for="number-of-colors-group"] ').html('Blink States');
		}
		else if($(this).val() == 'change-hue'){
			$('#color-selector label[for="number-of-colors-group"] ').html('# of Colors');		
		}
	});
	
	
	/*----------------------------------
		COLOR SWATCHES/COLOR PICKER 
	-----------------------------------*/
	var $currentSwatch;
	
	$('.color-swatch').click(function(e){

		// store current color of the swatch
		var currentColor = $(this).css('background-color');
		$currentSwatch = $(this);
		$('.currently-picking').removeClass('currently-picking');
		// and assign class to current swatch to allow styling/targeting
		$(this).addClass('currently-picking');
		// put the duration in the duration spinner
		$('#duration-setting').val($(this).html());
		swatchId = $(this).attr('id');
		// color the virtual blink and body background with the same color
		$('#virtual-blink').css('background-color', currentColor);
		// move the color zoom/view to the place of the color
		$('#color-zoom').css({'top': $currentSwatch.attr('data-yPos'), 'left' : $currentSwatch.attr('data-xPos'), 'background': currentColor});
		// and bring on the color picker!
		$('#picker').fadeIn('fast');
		if(!$('#configuration-popup').hasClass('demo')) {
			$('#configuration-popup').addClass('color-picker-open');
		}

		// choose a preset value (white or off)
		$('#picker .picker-swatch').live('click', function(e) {
			$currentSwatch.css('background', $(e.target).css('background'));
			$('#virtual-blink').css('background-color', $(e.target).css('background-color'));
			$('#color-zoom').css('background', $(e.target).css('background')).css({'top': -190, 'left': 61});
			if($(e.target).attr('id') != "light-off") {
                backendSetColor("rgb(255, 255, 255)");
				$('#color-display #rgb input').val('255');
				$('#color-zoom').css('background-color', '#ffffff');
				$('.currently-picking').css('background-color', '#ffffff');	
				$('#color-zoom').css('background-image', 'none');											
				$('.currently-picking').css('background-image', 'none');
			}
			else {
                backendSetColor("rgb(0, 0, 0)");
				$('#color-display #rgb input').val('0');
				$('#color-zoom').css('background', '#999 url("images/no-light-bg-scalable.png") no-repeat center center');
				$('.currently-picking').css('background', '#999 url("images/no-light-bg-scalable.png") no-repeat center center');
				$('.currently-picking').css('background-size', '100% 100%');
			}
			
		
		});


		// manually set RGB values
		
		$('#r').change(function(e) {
			colorWhilePicking('rgb(' + $('#r').val() + ', ' + $('#g').val() + ', ' + $('#b').val() + ')');
		});
		$('#g').change(function(e) {
			colorWhilePicking('rgb(' + $('#r').val() + ', ' + $('#g').val() + ', ' + $('#b').val() + ')');
		});
		$('#b').change(function(e) {
			colorWhilePicking('rgb(' + $('#r').val() + ', ' + $('#g').val() + ', ' + $('#b').val() + ')');
		});
		
		// change corresponding swatch while picking duration
		$('#duration-setting').change(function(e) {
			$('.currently-picking').html($(this).val());
		});

		// cancel the selection
		$('#picker a.cancel').live('click', function(e) {
			$('#picker').fadeOut('fast');
			$currentSwatch.css('background-color', currentColor);
			$('#virtual-blink').css('background-color', currentColor);
			$('.currently-picking').removeClass('currently-picking');
			$('#configuration-popup').removeClass('color-picker-open');
		});
		
		// save the current selection
		$('#picker a.done').live('click', function(e) {
			$('.currently-picking').html($('#duration-setting').val());
			$('.currently-picking').removeClass('currently-picking');
			$('#picker').fadeOut('fast');
			$('#configuration-popup').removeClass('color-picker-open');			
		});
	
	});
	
	
	/*---------------------------------
		SOURCE-DEPENDENT OPTIONS
	-----------------------------------*/
	
	$('#source-selector .options-group input').change(function(e) {
		var slug = $(this).attr('id').split('-')[2];

        // why was this here at all?
		//if($('.column-2').hasClass(slug)) { 
		//	// do nothing if already selected
		//}
		//else {
			$('.column-2 > div').hide();
			
			$('.column-2').attr('class', 'column column-2');
			$('.column-2').addClass(slug);
			$('.column-2 > div.' + slug).fadeIn();
            //}
        
        // FIXME: how to get triggerObject in this section?
        var fakeTriggerObject = new Object;
        fakeTriggerObject.source = new Object;
        fakeTriggerObject.source.type = slug;
        backendLiveValueStart( fakeTriggerObject );
	});
	


	
	
	
/*********************************

	SETTINGS POPUP BEHAVIOR

*********************************/

	
	// cancel out of settings popup
	$('#settings-popup a.cancel').live('click', function(e) {
		$('#settings-popup').fadeOut('fast');
		$('#gray-out').fadeOut('fast');
		$('#navbar').css('z-index', 0);	
		$('#settings-tab').removeClass('active');	
		$('.ui-state-active').addClass('active');
		$('#container').attr('class', '');	
		$('#container').addClass($('.active').attr('id').split('-')[0]);	
	});
	
    $('#delete-all-triggers').live('click', function(e) {
        console.log('delete all triggers');
        backendDeleteAllTriggers();
        window.location.reload();
    });

		


		
			
/****************************************

	KEYBOARD BEHAVIOR (escape and enter)

****************************************/
		
	// enable escape key (and enter key) to exit/submit popups
	$(document).keyup(function(e) {
		// enter key
		if (e.keyCode == 13 ) { 
		
			if($('#picker').css('display') == 'block') {
				$('#picker .done').click(); 		
			} else {
				if( $('#configuration-popup').hasClass('existing')) {
					$('.save-button.save-changes').click(); 
				} else {
					$('.save-button.save-as-new').click(); 
				} 			
			}
		}     
		// escape key
		if (e.keyCode == 27) { 
			if($('#picker').css('display') == 'block') {
				if($('#configuration-popup').hasClass('demo')) {
					$('.cancel').click(); 							
				} else {
					$('#picker .cancel').click(); 		
				}			
			} else {
				$('.cancel').click(); 			
			}
		}   
	});
	
	
		
/*************************************

	FUNCTIONS: CONFIGURATION POPUP

*************************************/		
		
	/*-------------------------
		OPEN POPUP FUNCTIONS
	--------------------------*/

	function openConfigurationPanel(existingObject, id, index) {

        backendStopInputs();

		// if we passed it an existing trigger object
		if(existingObject) {
			applyConfigurationOptions(existingObject, id, index);
			$('#configuration-popup').addClass('existing');
			$('#configuration-popup #submit-options-buttons input.save-as-new').val('create new');	
            backendLiveValueStart( existingObject );
		} 
		// otherwise, we're creating a new trigger
		else {
			// so we need to clear any residual values
			resetConfigurationPanel();
			logSettings();
			$('#configuration-popup').addClass('new');	
			$('#configuration-popup #submit-options-buttons input.save-as-new').val('save');
            setTimeout( "console.log('todfoo'); $('#source-option-ifttt').click();", 200); // FIXME: super hack to default to IFTTT on new 
		}
		// and finally, open the panel
		$('#gray-out').fadeIn('fast');
		$('#configuration-popup').fadeIn('fast');

	}		
	
	
	function applyConfigurationOptions(triggerObj, id, index) {
		resetConfigurationPanel();
		logSettings();

		$('#configuration-popup').addClass('existing');
		// fill in all the values from that object
		
		$('#popup-title').attr('data-id', id).attr('data-array-index', index);
		
		$('#popup-title > input').val(triggerObj.title); // set title
		
		$('#source-selector .options-group > input[value="' + triggerObj.source.type + '"] ').attr('checked', 'checked'); // input source
		$('.column-2 div.' + triggerObj.source.type + ' .arg1').val(triggerObj.source.arg1); // input source path
		if(triggerObj.source.colorOption) {
			$('.column-2.url #url-options-selector input[value="' + triggerObj.source.colorOption + '"] ').attr('checked', 'checked');
		}
		if(triggerObj.source.colorRetrieved) {
			$('.column-2.url #url-options-selector #value-retrieved-text-box').val(triggerObj.source.colorRetrieved);
		}
		$('.column-2 div.' + triggerObj.source.type).show();
		
		$('#behavior-selector > select').val(triggerObj.colorSettings.behavior); // color selector
		$('#transition-options > input[value="' + triggerObj.colorSettings.transition + '"] ').attr('checked', 'checked'); // color transition
		$('#repeat-options-group').val(triggerObj.colorSettings.repeatTimes); // color repeat
		
		$('#number-of-colors-group').val(triggerObj.colorSettings.colors.length); // number of colors
		resizeSwatches(triggerObj.colorSettings.colors.length, '.color-swatch', 127); // resize to appropriate number
		recolorSwatches(triggerObj.colorSettings.colors); // set colors
		reassignSwatchDurations(triggerObj.colorSettings.durations); // set durations	

	}

	
	
	/*-------------------------
		SAVE/CLOSE FUNCTIONS
	--------------------------*/



	function closeConfigurationPanel() {
		if($('#picker').css('display') != 'none') {
			$('#picker').fadeOut('fast');
			$('#configuration-popup').removeClass('color-picker-open');
		}	
		$('#gray-out').fadeOut('fast');
		$('#configuration-popup').fadeOut('fast');
        backendLiveValueStop();
        backendStartInputs();
	}

    //
    // ------------------------------------------------------------------------
    //

    /*------------------------------
      BLINK1 BACK-END FUNCTIONS
      (note: if they are declared outside of document.ready they can be used
      in other .js files. This also means they can't use any 'globals' above)
      --------------------------------*/

    //
    function backendLiveValueStart( aTriggerObject ) {
        console.log("backendLiveValueStart:"); 
        console.log(aTriggerObject);

        liveValue.source = new Object();
        liveValue.source.type = aTriggerObject.source.type;
        liveValue.startTime = new Date();

        backendLiveValueStop();
        backendLiveValue();
    }
    //
    function backendLiveValueStop() {
        console.log("backendLiveValueStop");
        if( liveValue.timer ) {
            clearTimeout( liveValue.timer );
        }
    }
    //
    function backendLiveValue() {
        var type = liveValue.source.type;
        var arg1 = '';
        if( type === 'url' ) {        arg1 = $('#web-page-url').val();  }
        else if( type == 'file' ) {   arg1 = $('#file-path').val();  }
        else if( type == 'ifttt' ) {  arg1 = $('#ifttt-rulename').val();  }
        else if( type == 'script' ) { arg1 = $('#script-path').val(); }
        
        console.log("backendLiveValue: "+type+ ", arg1:"+arg1);

        var b1url = "../blink1/input/"+type;
        
        var parms = { arg1: arg1,
                      iname: 'test',
                      test: 'true' };  // most important, doesn't add input, just runs input logic
        
        $('.ifttt #ifttt-event-received-icon').hide();
        $.getJSON( b1url, parms, function(result) { 
                if( type == 'url' ) { 
                    $('.url #value-retrieved-text-box').val( result.input.lastVal );
                } else if( type == 'file' ) {
                    $('.file #value-retrieved-text-box').val( result.input.lastVal );
                } else if( type == 'script' ) {
                    $('.script #value-retrieved-text-box').val( result.input.lastVal );
                } else if( type == 'ifttt' ) {
                    //console.log("ifttt result"); console.log(result);
                    var possibleVals = result.input.possibleVals;
                    var lastVal      = result.input.lastVal;
                    var lastTime     = result.input.lastTime;
                    var nowTime = new Date().getTime()/1000;
                    //console.log("lastVal: "+ lastVal);  console.log(possibleVals);
                    console.log("lastTime: "+lastTime+", now: "+ nowTime +", dt:"+(nowTime-lastTime));
                    var minsago = ((new Date().getTime()/1000 - lastTime )/60).toFixed(1);
                    minsago = (isNaN(minsago)) ? 'n/a' : minsago;
                    possibleVals = (possibleVals!=null && possibleVals.length) ? possibleVals.toString() : '-none-';

                    $('.ifttt #value-retrieved-timestamp').text("received " + minsago + " mins ago" );
                    $('.ifttt #value-retrieved-text-box').text( lastVal );
                    $('.ifttt #value-retrieved-possibles').text( possibleVals );
                }
            });

        liveValue.timer = setTimeout( backendLiveValue, 5000 );
    }

    //
    function backendLoadBlink1Settings() {
        //$.ajaxSetup({ cache: false, async: false  });
        var b1url = '../blink1/id';
        $.getJSON( b1url, function(result) { 
                var serialnums = result.blink1_serialnums;
                blink1Settings.statustext = "no blink(1) found";
                blink1Settings.serialnum = "-none-";
                blink1Settings.blink1_id = result.blink1_id;
                if( serialnums.length > 0 ) { 
                    blink1Settings.statustext = "Connected!";
                    blink1Settings.serialnum = serialnums[0];
                }
            });
        //$.ajaxSetup({ cache: true, async: true  });
    }

    //
    function backendStopInputs() {
        var b1url = '../blink1/inputs';
        var parms = { 'enabled' : false };
        $.getJSON( b1url, parms, function(result) {
            });
        backendSetColor("rgb(0, 0, 0)");
    }
    //
    function backendStartInputs() {
        var b1url = '../blink1/inputs';
        var parms = { 'enabled' : true };
        $.getJSON( b1url, parms, function(result) {
            });
    }

	function logSettings() {
		console.log("logSettings: setting ifttt-uid "+ blink1Settings.blink1_id);
        $('#ifttt-uid').text(    blink1Settings.blink1_id );
	}


}); // document.ready


/*************************************

	PUBLIC FUNCTIONS: CONFIGURATION POPUP

*************************************/	

/*-------------------------
		RESET POPUP FUNCTIONS
	--------------------------*/
		
	function resetNeutralColors() {
		$('#virtual-blink').css('background-color', "#eee");
		$('#color-zoom').css({'top': '-190px', 'left': '61px', 'background-color' : '#eee'});		
		$('.color-swatch').removeClass('.light-off').css('background-image', 'none');
		$('#color-display #rgb input').val('255');	
	}
	
	function resetConfigurationPanel() {
		// reset the color swatches, input selections and classes to neutral/unassigned status
		$('.color-swatch').removeClass('active').removeClass('.light-off').css('background-image', 'none').html('1.0');
		$('#color-zoom').css({'top': '-190px', 'left': '61px', 'background-color' : '#eee'});
					
		$('#rgb input').val('255');
		$('#duration-setting').val('1.0');
		
		if($('#configuration-popup.demo').hasClass('demo')) {
		 	$('#configuration-popup.demo').removeClass('demo');
		}
		$('#number-of-colors-group').val('1');
		$('.color-swatch').removeClass('active').css('background-color', '#eee');
		$('#virtual-blink').css('background-color', '#eee');
		$('#color-1').addClass('active');
		$('.color-swatch').css('width', 128);
		
		
		$('#configuration-popup input[type="text"] ').val('');
		$('#popup-title input').val('[Click to Edit Title]');		
		// uncheck all source radio buttons
		$('#configuration-popup #source-selector input[type="radio"] ').removeAttr('checked');

		// set default color setting to fade and repeat times to 1		
		$('#configuration-popup #transition-options #fade').attr('checked', 'checked');		
		$('#configuration-popup #repeat-options-group').val('1');		
		// clear out all column-2 visible divs
		$('#configuration-popup .column-2 div').hide();
		
		// turn all the dropdowns back to the top option
		$('#configuration-popup select').each(function(e){
			$(this).val($(this).children().first().val());
		});
		$('#configuration-popup').removeClass('existing').removeClass('new');
		
		// and finally, get the color swatches back in the right order
		$('#color-selector .color-swatch').each(function(index){
			$(this).attr('id', 'color-' + (index + 1));
		});

   	}

	
	
	
		
	
	
	/*--------------------
		SWATCH FUNCTIONS
	---------------------*/
	
	function resizeSwatches(numberOfColors, swatchSelector, containerWidth) {
		var currentNum = $(swatchSelector + '.active').length;		
		var numColors = numberOfColors;
		var margin = 4;
		var boxWidth = parseInt(  (containerWidth - ((margin + 2)*(numColors-1)) ) / numColors  );
		$(swatchSelector).css('width', boxWidth);
		
		if(swatchSelector == '.color-swatch') {	
			$(swatchSelector).css('margin-left', margin);
			if(numColors > 1) {
				$('#color-selector label[for="color-swatches"] ').html('State Colors');	
			} else {
				$('#color-selector label[for="color-swatches"] ').html('State Color');	
			}
		}
	
		if(numColors < currentNum) {
			for(var i = numColors; i < currentNum; i++) {
				$($(swatchSelector)[i]).removeClass('active').hide();
			}
		} else if(numColors > currentNum) {
			for(var i = currentNum; i < numColors; i++) {
				$($(swatchSelector)[i]).addClass('active').show();
			}
		}
	}

	
	function recolorSwatches(colors) {
		for(var i = 0; i < colors.length; i++) {
			if(colors[i] == 'rgb(0, 0, 0)') {
				$('#color-' + (i + 1)).css('background-color', '#999').addClass('light-off');
			} else {
				$('#color-' + (i + 1)).css('background-color', colors[i]).addClass('active').removeClass('light-off');
			}		
		}
	}
	
	function reassignSwatchDurations(durations) {
		for(var i = 0; i < durations.length; i++) {
			$('#color-' + (i + 1)).html(durations[i]);
		}
	} 
	
	
	function colorWhilePicking(color) {
		$('.currently-picking').css('background-color', color);
		$('#color-zoom').css('background-color', color);
		$('#virtual-blink').css('background-color', color);
	}
	

/*--------------------
	CLOSE POPUP FUNCTIONS
---------------------*/

function compileSettings() {
	// find number of active colors 
	var numberOfSwatches = parseInt($('#number-of-colors-group').val());
	var reorderedSwatches = [];
	var newSettings = new Object();
	
	//collect all settings in one object
	newSettings.colorSettings = new Object();
	newSettings.colorSettings.colors = [];
	newSettings.colorSettings.durations = [];		
	newSettings.source = new Object();

	
	newSettings.colorSettings.behavior = $('#behavior-selector select').val();
	newSettings.colorSettings.transition = $('#transition-options > input:checked').val();
	newSettings.colorSettings.repeatTimes = $('#repeat-options-group').val();
/* 		newSettings.colorSettings.duration = $('#duration-setting').val();		 */
	newSettings.source.type = $('#source-selector .options-group > input:checked').val();	

    if( newSettings.source.type == 'ifttt' )  {
		newSettings.source.arg1 = $('#ifttt-rulename').val();
    }
    else if( newSettings.source.type == 'url' )  {
		newSettings.source.arg1 = $('#web-page-url').val(); 
		newSettings.source.colorOption = $('.column-2.url #url-options-selector input[type="radio"]:checked').val();
		newSettings.source.colorRetrieved = $('.column-2.url #url-options-selector #value-retrieved-text-box').val();
    }
    else if( newSettings.source.type == 'file' )  {
		newSettings.source.arg1 = $('#file-path').val();
    }
    else if( newSettings.source.type == 'script' )  {
		newSettings.source.arg1 = $('#script-path').val();
    }


	if($('#popup-title > input').val() == '' || $('#popup-title > input').val() == '[Click to Edit Title]') {
        var randid = Math.floor((Math.random()*100)+1);  // if no title, make up a random one
		newSettings.title = 'Untitled'+ randid +'';
	}
	else {
		newSettings.title = $('#popup-title > input').val();						
	}			
	
	// serialize the sorted color choices into an array
	var activeSwatches = $('#color-swatches-container').sortable('toArray');	

	// create new array with color values and the correct order
	for(var i = 0; i < numberOfSwatches; i++){
		if($('#' + activeSwatches[i]).css('background-color') == 'rgb(153, 153, 153)') {
			newSettings.colorSettings.colors.push('rgb(0, 0, 0)');
			newSettings.colorSettings.durations.push($('#' + activeSwatches[i]).html());
		} else {
		    newSettings.colorSettings.colors.push($('#' + activeSwatches[i]).css('background-color'));
			newSettings.colorSettings.durations.push($('#' + activeSwatches[i]).html());		
		}
	}
	return(newSettings);
}




//
// 'triggerObjects' is an array of objects with the form:
//   obj.title                     = human readable name of trigger
//   obj.colorSettings.colors      = array of colors
//   obj.colorSettings.durations   = array of durations
//   obj.colorSettings.repeatTimes = 1,2,3,
//   obj.colorSettings.transition  = {'fade','flash'}
//   obj.colorSettings.behavior    = {'exact-color',...}
//   obj.source.type = {'file','ifttt','url',...}
//   obj.source.arg1 = 1st argument for type , url for 'url' type, filepath for 'file' type, 
//   obj.source.arg2 = 2nd argument for type 
//   obj.source.arg3 = 3rd argument for type 
// IF type == 'ifttt':
//   obj.source.arg1 = ifttt rule name
// IF type == 'file':
//   obj.source.arg1 = filepath
// IF type == 'url':
//   obj.source.arg1 = url
//   obj.source.colorOption = 'url-specified'
//   obj.source.colorRetrieved = ''  // FIXME
//
//
// $.getJSON( url, data, function() ) is equal to:
//    $.ajax(    { url: url,
//            dataType: 'json',
//                data: data,
//                success: fuction() 
//                });        
//
function backendDeleteTrigger(triggerObject) { 
    var parms = {};
    parms.iname = triggerObject.title;
    parms.pname = triggerObject.title;
    var iurl = '../blink1/input/del';
    var purl = '../blink1/pattern/del';

    $.ajaxSetup({ cache: false, async: false  });
    
    // delete input
    $.getJSON( iurl, parms, function(result) { 
            console.log("input del status:"+result.status+"'");
        })
        .error( function() { 
                console.log("error on input delete");
            }); 
    // delete pattern
    $.getJSON( purl, parms, function(result) { 
            console.log("pattern del status:"+result.status+"'");
        })
        .error( function() { 
                console.log("error on pattern delete");                    
            }); 
    $.ajaxSetup({ cache: true, async: true  });
}

//
function backendDeleteAllTriggers() {
    var iurl = '../blink1/input/delall';
    var purl = '../blink1/pattern/delall';
    $.ajaxSetup({ cache: false, async: false  });
    $.getJSON( iurl );
    $.getJSON( purl );
    $.ajaxSetup({ cache: true, async: true  });
}

// Send the new configuration to blink1 back-end server
// by traversing the 'triggerObects' array and constructing
// appropriate Ajax commands for the back-end
//
function backendUpdateTriggers(triggerObjects) {
    console.log("backendUpdateTriggers");
    console.log(triggerObjects);
    backendDeleteAllTriggers();
    $.ajaxSetup({ cache: false, async: false  });
    for( var i=0; i<triggerObjects.length; i++ ) {
        var trigObj = triggerObjects[i];
        var source = trigObj.source;
        var type = source.type;
        
        // first lets do inputs
        var iparms = {}; 
        iparms.iname = trigObj.title;
        iparms.pname = trigObj.title;
        iparms.type  = source.type;
        iparms.arg1  = source.arg1;
        iparms.arg2  = source.arg2;
        iparms.arg3  = source.arg3;
        
        console.log("title: '"+trigObj.title +"' arg1: '"+ source.arg1 +"'");

        if( trigObj.type != "undefined" ) {
            var iurl = '/blink1/input/' + type;
            $.getJSON( iurl, iparms, function(result) { 
                    console.log("input add status '"+ result.status+"'");  // FIXME: parse status
                    //console.log(result);
                } )
                .error( function() { 
                        console.log("error on input update!");
                    });
        }
        
        // now do color patterns
        var pparms = {};
        pparms.pname = trigObj.title;
        var colorSettings = trigObj.colorSettings; 
        var colorString = colorSettings.repeatTimes; // to start, then add to it
        for( var j=0; j< colorSettings.colors.length; j++ ) {
            var c = colorToHex( colorSettings.colors[j] );
            var d = colorSettings.durations[j];
            colorString += "," + c + "," + d ;
        }
        pparms.pattern = colorString;
        
        var purl = '/blink1/pattern/add';
        $.getJSON( purl, pparms, function(result) { 
                console.log("pattern add status '"+result.status+"'"); // FIXME: parse status
                //console.log(result);
            } );
    } // for each triggerObject
    $.ajaxSetup({ cache: true, async: true  });
}

// Retrieve input and pattern settings from the Blink1 backend server
// parses JSON output from back-end and turns it into an array of triggerObjects
//
function backendLoadTriggers() {
    console.log("backendLoadTriggers");
    var newTriggerObjects = []; 
    
    $.ajaxSetup({ cache: false, async: false  });
    
    // first, load up the info from the input side of things
    $.getJSON( '../blink1/inputs', function(result) { // FIXME: don't use '..'
            //console.log("input data status '"+ result.status+"'");
            //console.log(result);
            var inputs = result.inputs;
            for( var i=0; i< inputs.length; i++ ) {
                var inp = inputs[i]; 
                var trigger = new Object();
                trigger.title = inp.iname;
                trigger.source = new Object();
                trigger.source = inp;
                newTriggerObjects.push( trigger );
            }
            /*                 newTriggerObjects.reverse(); */
        })
        .error(function() { 
                console.log("error! could not read blink1/input json!"); 
            });
    
    // then add in the color patterns for each input
    $.getJSON( '../blink1/patterns', function(result) {  // FIXME: don't use '..'
            //console.log("pattern data status '"+ result.status+"'");
            //console.log(result);
            var patterns = result.patterns;
            for( var i=0; i< patterns.length; i++ ) {
                var patt = patterns[i]; 
                var pattparts = patt.pattern.split(',');
                var colors = [];
                var durations = [];
                for( var j=1; j< pattparts.length; j+=2 ) { 
                    colors.push( hexToColor(pattparts[j+0]) );
                    durations.push(         pattparts[j+1] );
                }
                
                var colorSettings = new Object();
                colorSettings.behavior    = 'exact-color';
                colorSettings.transition  = 'fade';
                colorSettings.repeatTimes = patt.repeats;
                colorSettings.colors      = colors;
                colorSettings.durations   = durations;
                
                // add pattern to correct newTriggerObject
                $(newTriggerObjects).each( function() { 
                        if( this.title == patt.name ) { 
                            this.colorSettings = colorSettings;
                        }
                    });
            }
        })
        .error(function() { 
                console.log("error! could not read blink1/pattern json!"); 
            });
    
    for( var i=0; i< newTriggerObjects.length; i++ ) {
        var obj = newTriggerObjects[i];
        if( ! obj.colorSettings ) {
            var colorSettings = new Object();
            colorSettings.behavior = 'exact-color';
            colorSettings.transition = 'fade';
            colorSettings.repeatTimes = 1;
            colorSettings.colors = [ hexToColor('#ff0000') ];
            colorSettings.durations = [ 1.0 ];
            obj.colorSettings = colorSettings;
            console.log("no pattern for input "+ obj.title);
        }
    }

    console.log("newTriggerObjects:");
    console.log(newTriggerObjects);
    
    $.ajaxSetup({ cache: true, async: true  });
    
    $(newTriggerObjects).each( function() {
			this.title = this.title.replace(/\+/g, ' ');	
		});
    
    return newTriggerObjects;
}


// 
function backendSetColor(color) {
    backendFadeColor(color,0.1);
}

function backendFadeColor(color,secs) { 
    //console.log("backendFadeColor:"+color+","+secs);
    var colr = colorToHex(color); // convert to hex color if not
    var parms = { rgb : colr, 
                  time : secs };
    $.getJSON( '../blink1/fadeToRGB', parms );
}

// convert things like "rgb(255,128,0)" to "#FF8000"
function colorToHex(color) {
    if (color.substr(0, 1) === '#') {
        return color;
    }
    var digits = /(.*?)rgb\((\d+), (\d+), (\d+)\)/.exec(color);
    
    var red   = parseInt(digits[2]);
    var green = parseInt(digits[3]);
    var blue  = parseInt(digits[4]);
    
    var rgb = 0x1000000 + (blue | (green << 8) | (red << 16));
    return digits[1] + '#' + rgb.toString(16).substr(1);
};

// convert things like "#FF0000" to "rgb(255,128,0)"
function hexToColor(hexstring) { 
    var hex = (hexstring.charAt(0)=="#") ? hexstring.substring(1,7) : hexstring;  
    var r = parseInt(hex.substring(0,2),16);
    var g = parseInt(hex.substring(2,4),16);
    var b = parseInt(hex.substring(4,6),16);
    return("rgb(" + r + ", "+g+", "+b+")");
}



	
	
