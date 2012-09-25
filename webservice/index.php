<?php
/*
 *
 */

require 'Slim/Slim.php';
\Slim\Slim::registerAutoloader();
$app = new \Slim\Slim();
$req = $app->request();
$log = $app->getLog();
$app->contentType('application/json;charset=utf-8');

#$log->setEnabled(true);


$eventDir = getcwd() . "/events";

//
function writeEvent($blink1_id,$event)
{
    global $eventDir;
    global $log;

    $retstr = "";

    //$blink1_id = $event['blink1_id'];
    $fname = "$eventDir/" . $blink1_id; // . ".txt";

    if (!$handle = fopen($fname, 'w')) {
        $log->error("Cannot open file ($filename)");
        $retstr = "cannot open event file for '$blink1_id'";
        return;
    }

    $eventstr = json_pretty( json_encode($event) ) . "\n";

    // Write $somecontent to our opened file.
    if (fwrite($handle, $eventstr) === FALSE) {
        $log->error("Cannot write file ($filename)");
        $retstr = "Cannot save event for '$blink1_id'";
        return;
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
    $str = json_encode($data);
    $str =  json_pretty($str) . "\n";
    echo $str;
}

// -------------------------------------------

//
$app->get('/', function () {
        $str = <<<EOT
blink1 api service.  valid paths are /blink1/sendevent
EOT;

        $event['result'] = $str;
        echo json_pretty( json_encode($event) );

    });

//
$app->get('/hello/:name', function ($name) {
        echo "Hello, $name";
    });

//
$app->get('/sendevent/:blink1_id', function($blink1_id) use( &$req ) { 
        #$blink1_id  = $req->get('blink1_id');
        $name       = $req->get('name');
        $source     = $req->get('source');

        if( !isValidBlink1Id( $blink1_id ) ) { 
            send_json_response( "invalid blink1_id", NULL);
            return;
        }

        if( empty($name) ) { $name = 'default'; } 
        if( empty($source) ) { $source = 'default'; } 

        $event['blink1_id'] = $blink1_id;
        $event['name']      = $name;
        $event['source']    = $source;
        $event['date']      = time();

        $result = writeEvent( $blink1_id, $event );

        send_json_response( "success: $result", $events);

    });

//
$app->post('/sendevents/:blink1_id', function($blink1_id) use( &$req ) { 
        $post = $req->post();
        $body = $req->getBody();
        $jsondata = json_decode($body);
        $status_str = "";

        if( !isValidBlink1Id( $blink1_id ) ) { 
            $status_str = "invalid blink1_id";
            send_json_response($status_str, NULL);
            return;
        }

        $events = $jsondata->{'events'};
        if( empty($events) ) { 
            $status_str = "no 'events' datastruct found in POST body";
            send_json_response($status_str, NULL);
            return;
        }

        foreach ( $events as $event ) {
            $bl_id = $event->{'blink1_id'};
            $date = $event->{'date'};
            if( empty($date) ) {
                $event->{'date'} = time();
            }
                               
            echo "blink1_id = $bl_id\n";
            if( !isValidBlink1Id( $bl_id ) ) { 
                #echo "{\"status\":\"invalid id\"}"; //FIXME: 
                next;
            }
        }

        $result = writeEvent($blink1_id, $events);

        $status_str = "success: $result";
 
        send_json_response($status_str, $events);

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
