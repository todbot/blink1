$(document).ready(function(){

	var triggerObjects = [];
	var swatchId = '';
	var swatchColor = '';
	
	// make the colors watch order sortable inside the configuration interface
	$('#color-swatches-container').sortable();
	$( "#sortable" ).disableSelection();
	
	
	$('#navbar li').live('click', function(e) {
		if($(this).attr('id') == 'settings-tab') {
			if($(this).hasClass('active')){
				$('#settings-popup').fadeOut('fast');
				$('#gray-out').fadeOut('fast');	
				$('#navbar').css('z-index', 0);
				$(this).removeClass('active');	
				$('.ui-state-active').addClass('active');
			} else {
				$('#settings-popup').fadeIn('fast');
				$('#gray-out').fadeIn('fast');	
				$('#navbar').css('z-index', 1);	
				$('#navbar li.active').removeClass('active');
				$(this).addClass('active');	
			}	

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
		$('#container').addClass($('.active').attr('id').split('-')[0]);

	});
	
	

   /*
 $('#demo-tab').click(function(e) {
    	$.get('ajax/color-picker.html', function(data){
    		alert("data loaded: " + data);
    	});
    }); 
*/  
        
	$('#add-new-trigger').live('click', function(e) {
		resetConfigurationPanel();
		openConfigurationPanel();
	});


	$('.trigger-behavior').live('click', function(e) {
		var index = $(this).closest('.trigger-options').index();
		var id = $(this).closest('.trigger-options').attr('id').split('-')[1];
/* 		applyConfigurationOptions(); */
		openConfigurationPanel(triggerObjects[index-1], id, index);
	});

	$('#configuration-popup a.cancel').live('click', function(e) {
		closeConfigurationPanel();		
	});
	
	
	$('#configuration-popup .save-button').live('click', function(e) {
		closeConfigurationPanel();
		
		// if it's the first trigger, get rid of the welcome message
		if($('#trigger-list .trigger-options').length < 2) {
			$('#welcome-message').hide();
			$('#trigger-headers li').css('opacity', 1);
		}
		// get all the settings we just set wrapped into a nice little object
		var settings = compileSettings();
		resetConfigurationPanel();
		
		// if we're creating a new trigger...
		if($(this).hasClass('save-as-new')) {
			
			if($('#trigger-list .trigger-options').length > 0) {	
				var idOfLast = $('#trigger-list .trigger-options').last().attr('id').split('-')[1];
				$('#trigger-list').append($('#trigger-options-to-clone').clone());
				$('#trigger-list .trigger-options').last().attr('id', 'trigger-' + (parseInt(idOfLast) + 1));
			}		
			else {
				$('#trigger-list').append($('#trigger-options-to-clone').clone());
				$('#trigger-list .trigger-options').last().attr('id', 'trigger-1');
			}	
				

			// push this new trigger to the triggerObjects array
			triggerObjects.push(settings);
			
			// color the summary swatches on the main homepage
			$('#trigger-list .trigger-options').last().find('.summary-color-swatch').removeClass('active');
			$(settings.colorSettings.colors).each(function(index){
				$($('#trigger-list .trigger-options').last().find('.summary-color-swatch')[index]).addClass('active').css('background-color', this);
			});
		}
		// otherwise if we're saving an existing one...
		else if($(this).hasClass('save-changes')) {
			var indexToReplace = $('#popup-title').attr('data-array-index');
			var idToReplace = $('#popup-title').attr('data-id');
			// swap out the old object with the new one we just modified
			triggerObjects.splice(indexToReplace, 1, settings); 
			
			// color the summary swatches on the main homepage for the one we edited
			$('#trigger-list .trigger-options').last().find('.summary-color-swatch').removeClass('active');
			$(settings.colorSettings.colors).each(function(index){
				$($('#trigger-list .trigger-options#trigger-' + idToReplace).find('.summary-color-swatch')[index]).addClass('active').css('background-color', this);
			});
		}
		console.log(triggerObjects);
	});
	
	
	$('.delete-trigger').live('click', function(e) {
		var $triggerObject = $(this).parents('.trigger-options');
		var index = $(this).closest('.trigger-options').index();
		$triggerObject.slideUp('normal', function() {
			$triggerObject.remove();
			if($('#trigger-list .trigger-options').length < 1) {
			$('#welcome-message').fadeIn('slow');
			$('#trigger-headers li').css('opacity', .2);
		}
		});
		
		triggerObjects.splice((index-1), 1);
		console.log(triggerObjects);
		
	});
	
	$('#number-of-colors-group').change(function(e){
		resizeSwatches($(this).val());
	});

	$('#behavior-selector select').change(function(e){
		if($(this).val() == 'blink-pattern') {
			$('#color-selector label[for="number-of-colors-group"]').html('Blink States');
		}
		else if($(this).val() == 'change-hue'){
			$('#color-selector label[for="number-of-colors-group"]').html('# of Colors');		
		}
	});
	
	$('.color-swatch').click(function(e){
		var currentColor = $('this').css('background-color');
		var $currentSwatch = $(this);
		$('.currently-picking').removeClass('currently-picking');
		$(this).addClass('currently-picking');
		swatchId = $(this).attr('id');
		swatchColor = $(this).css('background-color');
		$('#virtual-blink').css('background-color', $(this).css('background-color'));
		$('body').css('background-color', $(this).css('background-color'));		
		$('#picker').fadeIn('fast');
		
		$('#picker a.cancel').live('click', function(e) {
			$('#picker').fadeOut('fast');
			$currentSwatch.css('background-color', currentColor);
			$('#virtual-blink').css('background-color', currentColor);
			$('.currently-picking').removeClass('currently-picking');
			$('.currently-picking').removeClass('currently-picking');
			$('body').css('background-color', "#F0F0F0");		
		});
		$('#picker a.done').live('click', function(e) {
			$('.currently-picking').removeClass('currently-picking');
			$('#picker').fadeOut('fast');
			$('body').css('background-color', "#F0F0F0");
		});
	
	});
	
	$('#settings-popup a.cancel').live('click', function(e) {
		$('#settings-popup').fadeOut('fast');
		$('#gray-out').fadeOut('fast');
		$('#navbar').css('z-index', 0);	
		$('#settings-tab').removeClass('active');	
		$('.ui-state-active').addClass('active');
		$('#container').attr('class', '');	
		$('#container').addClass($('.active').attr('id').split('-')[0]);	
	});
		
	function resetNeutralColors() {
		$('body').css('background-color', "#F0F0F0");
		$('#virtual-blink').css('background-color', "#eee");		
	}
	
	function compileSettings() {
		// find number of active colors 
		var numberOfSwatches = parseInt($('#number-of-colors-group').val());
		var reorderedSwatches = [];
		var compiledSettings = new Object();
		
		//collect all settings in one object
		compiledSettings.colorSettings = new Object();
		compiledSettings.colorSettings.colors = [];
		compiledSettings.inputSettings = new Object();
		compiledSettings.inputSettings.state = new Object();
		
		compiledSettings.colorSettings.behavior = $('#behavior-selector select').val();
		compiledSettings.colorSettings.transition = $('#transition-options > input:checked').val();
		compiledSettings.colorSettings.duration = $('#duration-group > option:checked').val();		
		compiledSettings.inputSettings.source = $('#source-selector .options-group > input:checked').val();		
		compiledSettings.inputSettings.state.type = $('#state-selector select').val();	
		compiledSettings.inputSettings.state.min = $('#min-value').val();						
		compiledSettings.inputSettings.state.max = $('#max-value').val();						
		if($('#popup-title > input').val() == '') {
			compiledSettings.title = "Untitled";
		}
		else {
			compiledSettings.title = $('#popup-title > input').val();						
		}			
		
		// serialize the sorted color choices into an array
		var activeSwatches = $('#color-swatches-container').sortable('toArray');	

		// create new array with color values and the correct order
		for(var i = 0; i < numberOfSwatches; i++){
		    compiledSettings.colorSettings.colors.push($('#' + activeSwatches[i]).css('background-color'));
		}
		return(compiledSettings);
	}
	
	// enable escape key (and enter key) to exit/submit popups
	$(document).keyup(function(e) {
		// enter key
		if (e.keyCode == 13 ) { 
		
			if($('#picker').css('display') == 'block') {
				$('#picker .done').click(); 		
			}
			else {
				if( $('#configuration-popup').hasClass('existing')) {
					$('.save-button.save-changes').click(); 
				}
				else {
					$('.save-button.save-as-new').click(); 
				} 			
			}
		
		}     
		// escape key
		if (e.keyCode == 27) { 
			if($('#picker').css('display') == 'block') {
				$('#picker .cancel').click(); 		
			}
			else {
				$('.cancel').click(); 			
			}

		}   
	});

        
	$( "#tabs" ).tabs({
		hide: false,
		show: true,
        beforeLoad: function( event, ui ) {
            ui.jqXHR.error(function() {
                ui.panel.html("Couldn't load this tab. We'll try to fix this as soon as possible." );
            });
        }
    });



	function openConfigurationPanel(existingObject, id, index) {
		// if we passed it an existing trigger object
		if(existingObject) {
			applyConfigurationOptions(existingObject, id, index);
			$('#configuration-popup').addClass('existing');
			$('#configuration-popup #submit-options-buttons input.save-as-new').val('save as new');	
		} 
		// otherwise, we're creating a new trigger
		else {
			// so we need to clear any residual values
			resetConfigurationPanel();
			$('#configuration-popup').addClass('new');	
			$('#configuration-popup #submit-options-buttons input.save-as-new').val('save');		
		}
		// and finally, open the panel
		$('#gray-out').fadeIn('fast');
		$('#configuration-popup').fadeIn('fast');
	}

	function resetConfigurationPanel() {
		// reset the color swatches, input selections and classes to neutral/unassigned status
		$('.color-swatch').removeClass('active');
		
		
		$('#number-of-colors-group').val('1');
		$('.color-swatch').removeClass('active').css('background-color', '#eee');
		$('#virtual-blink').css('background-color', '#eee');
		$('#color-1').addClass('active');
		$('.color-swatch').css('width', 128);
		
		
		$('body').css('background-color', "#F0F0F0");		
		$('#configuration-popup input[type="text"]').val('');
		// reset radio button inputs to first option
		$('#configuration-popup #source-selector #source-option-network-load').attr('checked', 'checked');		
		$('#configuration-popup #transition-options #fade').attr('checked', 'checked');		
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
		
	function applyConfigurationOptions(triggerObj, id, index) {
		$('#configuration-popup').addClass('existing');
		// fill in all the values from that object
		
		$('#popup-title').attr('data-id', id).attr('data-array-index', index-1);
		
		$('#popup-title > input').val(triggerObj.title);
		
		$('#source-selector .options-group > input[value="' + triggerObj.inputSettings.source + '"]').attr('checked', 'checked');
		
		$('#state-selector select').val(triggerObj.inputSettings.state.type);
		$('#min-value').val(triggerObj.inputSettings.state.min);
		$('#max-value').val(triggerObj.inputSettings.state.max);
		
		$('#behavior-selector > select').val(triggerObj.colorSettings.behavior);
		$('#duration-group').val(triggerObj.colorSettings.duration);
		
		$('#transition-options > input[value="' + triggerObj.colorSettings.transition + '"]').attr('checked', 'checked');
		$('#duration-group').val(triggerObj.colorSettings.duration);
		
		$('#number-of-colors-group').val(triggerObj.colorSettings.colors.length);
		resizeSwatches(triggerObj.colorSettings.colors.length);
		recolorSwatches(triggerObj.colorSettings.colors);
		
/* 		compiledSettings.colorSettings.transition = $('#transition-options > input:checked').val(); */
		
		
		
		
		
	}
	
	function resizeSwatches(numberOfColors) {
		var currentNum = $('.color-swatch.active').length;		
		var numColors = numberOfColors;
		var margin = 4;
		var boxWidth = parseInt(  (127 - ((margin + 2)*(numColors-1)) ) / numColors  );
		$('.color-swatch').css('width', boxWidth);
		$('.color-swatch').css('margin-left', margin);
		/* $('#color-selector .color-swatch:nth-of-type(1)').css('margin-left', 0); */
		
		if(numColors > 1) {
			$('#color-selector label[for="color-swatches"]').html('State Colors');	
		}
		else {
			$('#color-selector label[for="color-swatches"]').html('State Color');	
		}
	
		if(numColors < currentNum) {
			for(var i = numColors; i < currentNum; i++) {
				$($('.color-swatch')[i]).removeClass('active').hide();
			}
		}
		else if(numColors > currentNum) {
			for(var i = currentNum; i < numColors; i++) {
				$($('.color-swatch')[i]).addClass('active').show();
			}
		}
	}
	
	function recolorSwatches(colors) {
		for(var i = 0; i < colors.length; i++) {
			$('#color-' + (i + 1)).css('background-color', colors[i]).addClass('active');
		}
	}
	

	function closeConfigurationPanel() {
		$('#gray-out').fadeOut('fast');
		$('#configuration-popup').fadeOut('fast');
	}
	
});