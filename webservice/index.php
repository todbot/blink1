<?php
/*
 * Webservice API for blink(1), for IFTTT
 *
 * 2012, ThingM Corporation, Tod E. Kurt, http://thingm.com/
 *
 */

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
        send_json_response("url not found",NULL);
});

//$log->setEnabled(true);  // for debugging


$eventDir = getcwd() . "/events";

//
function writeEvents($blink1_id,$events)
{
    global $eventDir;
    global $log;

    $retstr = "";

    //$blink1_id = $event['blink1_id'];
    $fname = "$eventDir/" . $blink1_id; // . ".txt";

    if (!$handle = fopen($fname, 'w')) {
        $log->error("Cannot open file ($filename)");
        $retstr = "cannot open event file for '$blink1_id'";
        return $retstr;
    }

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

//
function isValidBlink1Id($blink1_id)
{
    if( empty($blink1_id) ) {
        return FALSE;        
    }
    if( $blink1_id == 0 ) {
        return FALSE;
    }
    
    return TRUE;
}

function send_json_response($status_str, $data)
{
    $data['status'] = $status_str;
    $str = json_encode($data,true);
    $str = str_replace("\/","/",$str); 
    $str =  json_pretty($str) . "\n";
    echo $str;
}

// -------------------------------------------

//
$app->get('/', function () {
        $str = <<<EOT
Welcome to the blink1 api service.  valid paths are GETS to /blink1/sendevent and POSTs to /blink1/sendevents.  Details are in the 'web-api.txt' document.
EOT;
        send_json_response($str, NULL);
    });

//
$app->get('/hello/:name', function ($name) {
        echo "Hello, $name";
    });

//
/*
$app->get('/events/:blink1_id', function($blink1_id) use( &$req,$app ) { 
        echo "bah!";
        $app->render("events/$blink1_id"); // FIXME:
    });
*/
//
$app->get('/sendevent/:blink1_id', function($blink1_id) use( &$req ) { 
        //blink1_id  = $req->get('blink1_id');
        $name       = $req->get('name');
        $source     = $req->get('source');

        if( !isValidBlink1Id( $blink1_id ) ) { 
            send_json_response( "invalid blink1_id", NULL);
            return;
        }
        //echo "blink1_id: '$blink1_id'";

        if( empty($name) ) { $name = 'default'; } 
        if( empty($source) ) { $source = 'default'; } 
        
        $event['blink1_id'] = $blink1_id;
        $event['name']      = $name;
        $event['source']    = $source;
        $event['date']      = "".time(); // FIXME: hack convert to $result

        $events['events'][] = $event; // push onto events list
        $str = writeEvents( $blink1_id, $events );

        send_json_response( "success: $str", $events);

    });

//
$app->post('/sendevents/:blink1_id', function($blink1_id) use( &$req ) { 
        $post = $req->post();
        $body = $req->getBody();
        $jsondata = json_decode($body, false);
        $status_str = "";

        if( !isValidBlink1Id( $blink1_id ) ) { 
            $status_str = "invalid blink1_id";
            send_json_response($status_str, NULL);
            return;
        }
        
        $events = $jsondata->{'events'};
        //$events = $jsondata{'events'};
        if( empty($events) ) { 
            $status_str = "no 'events' datastruct found in POST body";
            send_json_response($status_str, NULL);
            return;
        }

        $event_count = 0;
        foreach ( $events as $event ) {
            $bl_id = $event->{'blink1_id'};
            $date = $event->{'date'};
            if( empty($date) ) {
                $event->{'date'} = "".time(); // FIXME: convert to string
                //$event{'date'} = "".time(); // FIXME: convert to string
            }
                               
            if( !isValidBlink1Id( $bl_id ) ) { 
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

        return true;
    });

// run it!
$app->run();



// ------------------------------------------------------------------------
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
