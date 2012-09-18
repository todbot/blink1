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
function writeEvent($event)
{
    global $eventDir;
    global $log;

    $retstr = "";

    $blink1_id = $event['blink1_id'];
    $fname = "$eventDir/" . $blink1_id; // . ".txt";

    if (!$handle = fopen($fname, 'w')) {
        $log->error("Cannot open file ($filename)");
        $retstr = "cannot open event file for '$blink1_id'";
        return;
    }

    $eventstr = json_pretty( json_encode($event) );

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
    
    return TRUE;
}

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


// -------------------------------------------

//
$app->get('/', function () {
        echo "{\"result\" : \"so the index then\"}";
    });

//
$app->get('/hello/:name', function ($name) {
        echo "Hello, $name";
    });

//
$app->get('/blink1/sendevent', function() use( &$req ) { 
        $blink1_id  = $req->get('blink1_id');
        $name       = $req->get('name');
        $source     = $req->get('source');

        if( !isValidBlink1Id( $blink1_id ) ) { 
            echo "{\"status\":\"invalid id\"}"; //FIXME: 
            return;
        }

        if( empty($name) ) { $name = 'default'; } 
        if( empty($source) ) { $source = 'default'; } 

        $event['blink1_id'] = $blink1_id;
        $event['name']      = $name;
        $event['source']    = $source;
        $event['date']      = time();

        $result = writeEvent( $event );

        $event['status'] = "success: $result";
 
        $str = json_encode($event);
        echo json_pretty($str);

    });

$app->run();
