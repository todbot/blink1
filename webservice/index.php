<?php
/*
 * Webservice API for blink(1), for IFTTT
 *
 * 2012, ThingM Corporation, Tod E. Kurt, http://thingm.com/
 *
 */

// yeah we're going to be as strict as we can 
error_reporting( E_ALL );

require 'Slim/Slim.php';
\Slim\Slim::registerAutoloader();
$app = new \Slim\Slim();
$req = $app->request();
$log = $app->getLog();
$app->contentType('application/json;charset=utf-8');
$app->error(function (\Exception $e) use ($app) {
        #$app->render('error.php');
        send_json_response("error: $e",NULL);
});
$app->notFound(function () use ($app) {
        send_json_response("error: url handler not found",NULL);
        //echo "var dump of app";
        //var_dump($app);
});
$log->setEnabled(true);  // for debugging


$eventsDir = getcwd() . "/eventscache";

$blink1_id_logfile = getcwd(). "/eventscache/blink1_id_log.txt";


// ----------------------------------------------------------------------------

// Given a blink1_id and a list of events
// write those events out
// return result of write attempt as string
function writeEvents($blink1_id,$events)
{
    global $eventsDir;
    global $log;

    $retstr = "";

    //$blink1_id = $event['blink1_id'];
    $fname = "$eventsDir/" . $blink1_id; // . ".txt";

    if (!$handle = fopen($fname, 'w')) {
        $log->error("Cannot open file ($filename)");
        $retstr = "cannot open event file for '$blink1_id'";
        return $retstr;
    }

    $events['status'] = 'events saved';
    $eventstr = json_pretty( json_encode($events,true) ) . "\n";

    // Write $somecontent to our opened file.
    if (fwrite($handle, $eventstr) === FALSE) {
        $log->error("Cannot write file ($filename)");
        $retstr = "Cannot save event for '$blink1_id'";
        fclose($handle);
        return $retstr;
    }

    fclose($handle);
    $retstr = "event saved";
    return $retstr;
}

// Determine if passed-in blink1 id is invalid
// returns string of error or FALSE
function isInvalidBlink1Id($blink1_id)
{
    if( empty($blink1_id) ) {
        return "empty blink1_id";
    }
    if( !ctype_xdigit($blink1_id)  ) {
        return "blink1_id not hex";
    }
    if( strlen($blink1_id) != 16 ) { 
        return "blink1_id wrong length";
    }
    if( "0x".$blink1_id == 0 ) {  // "0x" hex hack to test for numericness
        return "zero blink1_id";
    }
    return FALSE;
}

// add a "status" entry to passed in data struct 
// convert to JSON string
// and spit it out to client
function send_json_response($status_str, $data)
{
    $data['status'] = $status_str;
    $str = json_encode($data,true);
    $str = str_replace("\/","/",$str); 
    $str =  json_pretty($str) . "\n";
    echo $str;
}

// ----------------------------------------------------------------------------

//
$app->get('/', function () {
        $str = <<<EOT
Welcome to the blink1 api service.  valid paths are GETS to /blink1/sendevent and POSTs to /blink1/sendevents.  Details in the 'web-api.txt' document.
EOT;
        send_json_response($str, NULL);
    });

// simple test to make sure .htaccess works
$app->get('/ping/(:name)', function ($name = "anon") {
        echo "Ping yourself, $name";
    });

// Send an event to a blink1_id
$app->get('/sendevent/:blink1_id', function($blink1_id) use( &$req ) { 
        $name       = $req->get('name');
        $source     = $req->get('source');

        $blink1_id = strtoupper($blink1_id);

        if( ($errorcode = isInvalidBlink1Id( $blink1_id )) ) { 
            send_json_response( "error: invalid blink1_id '$blink1_id': $errorcode", NULL);
            return;
        }

        if( empty($name) ) { $name = 'default'; } 
        if( empty($source) ) { $source = 'default'; } 
        
        $event['blink1_id'] = $blink1_id;
        $event['name']      = $name;
        $event['source']    = $source;
        $event['date']      = "".time(); // FIXME: hack convert to $result

        $events['events'][] = $event; // push onto events list
        $events['event_count'] = 1;
        $str = writeEvents( $blink1_id, $events );

        send_json_response( "success: $str", $events);
    });

//
$app->post('/sendevents/:blink1_id', function($blink1_id) use( &$req ) { 
        $post = $req->post();
        $body = $req->getBody();
        $jsondata = json_decode($body, false);
        $status_str = "";

        $blink1_id = strtoupper($blink1_id);

        if( !isValidBlink1Id( $blink1_id ) ) { 
            $status_str = "error: invalid blink1_id";
            send_json_response($status_str, NULL);
            return;
        }
        
        $events = $jsondata->{'events'};
        if( empty($events) ) { 
            $status_str = "error: no 'events' datastruct found in POST body";
            send_json_response($status_str, NULL);
            return;
        }

        $event_count = 0;
        foreach ( $events as $event ) {
            $bl_id = $event->{'blink1_id'};
            $date = $event->{'date'};
            if( empty($date) ) {
                $event->{'date'} = "".time(); // FIXME: convert to string
            }
                               
            if( isInvalidBlink1Id( $bl_id ) ) { 
                //echo "{\"status\":\"invalid id\"}"; //FIXME: 
                next;
            }
            $events_to_save['events'][] = $event;
            $event_count++;
        }
        $events_to_save['event_count'] = $event_count;

        $result = writeEvents($blink1_id, $events_to_save);

        $status_str = "success: $result";
 
        send_json_response($status_str, $events_to_save);
    });

//
$app->get('/events/:blink1_id', function($blink1_id) use( &$req,$app,$blink1_id_logfile ) { 
        global $eventsDir;

        $blink1_id = strtoupper($blink1_id);

        if( ($errorcode = isInvalidBlink1Id( $blink1_id )) ) { 
            send_json_response( "error: invalid blink1_id '$blink1_id': $errorcode", NULL);
            return;
        }

        $events_present = 0;
        // send out the event data
        $fname = "$eventsDir/" . $blink1_id; // . ".txt";
        if( file_exists( $fname ) ) {
          readfile($fname);
          $events_present = 1;
        }
        else {
            send_json_response( "no events",null );
        }

        // log the blink1_id used
        $logline = time() . "\t" . $blink1_id . "\t" . $events_present . "\n";
        $fhl = fopen( $blink1_id_logfile , 'a'); // append
        fwrite( $fhl, $logline );
        fclose( $fhl );

    });


// run it!
$app->run();



// ------------------------------------------------------------------------
// found here: https://gist.github.com/906036
//
function json_pretty($json) 
{
    $result      = '';
    $pos         = 0;
    $strLen      = strlen($json);
    $indentStr   = '  ';
    $newLine     = "\n";
    $prevChar    = '';
    $outOfQuotes = true;

    for ($i=0; $i<=$strLen; $i++) {
        // Grab the next character in the string.
        $char = substr($json, $i, 1);
        // Are we inside a quoted string?
        if ($char == '"' && $prevChar != '\\') {
            $outOfQuotes = !$outOfQuotes;
        // If this character is the end of an element, 
        // output a new line and indent the next line.
        } else if(($char == '}' || $char == ']') && $outOfQuotes) {
            $result .= $newLine;
            $pos --;
            for ($j=0; $j<$pos; $j++) {
                $result .= $indentStr;
            }
        }
        // Add the character to the result string.
        $result .= $char;
        // If the last character was the beginning of an element, 
        // output a new line and indent the next line.
        if (($char == ',' || $char == '{' || $char == '[') && $outOfQuotes) {
            $result .= $newLine;
            if ($char == '{' || $char == '[') {
                $pos ++;
            }
            for ($j = 0; $j < $pos; $j++) {
                $result .= $indentStr;
            }
        }
        $prevChar = $char;
    }
    return $result;
}

// notice lack of ending php tag because we're doing it like the cool kids
