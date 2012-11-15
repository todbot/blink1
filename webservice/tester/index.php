<?php
/*
 *
 */

error_reporting( E_ALL ); // yeah we're going to be as strict as we can 

$slimpath = '..';
set_include_path(get_include_path() . PATH_SEPARATOR . $slimpath);

require 'Slim/Slim.php';
require 'Slim/View.php';
require 'Slim/Extras/Views/Twig.php';

use Slim\Slim;
use Slim\Extras\Views\Twig;

Twig::$twigDirectory = '../Slim/twig/lib/Twig';
Twig::$twigOptions = array("debug" => true);

\Slim\Slim::registerAutoloader();

$app = new Slim( array(
                       'debug' => true,
                       'view' => new Twig,
                       'templates.path' => '.',
                       ));

//
$app->get('/', function() use( $app ) { 

        $blink1_id = $app->request()->get('blink1_id');
        $source    = $app->request()->get('source');
        $name      = $app->request()->get('name');

        $app->render('template0.html', array(
                                             'blink1_id' => $blink1_id,
                                             'source' => $source,
                                             'name' => $name,
                                             )
                     );

   });

// run it!
$app->run();

