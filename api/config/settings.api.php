<?php

require 'classes/autoload.inc.php';
require 'functions.api.php';
require 'paths.api.php';
require 'responses.api.php';

define('ALLOWED_METHODS', [
    'get' => INPUT_GET,
    'post' => INPUT_POST,
    'put' => INPUT_POST,
    'delete' => INPUT_POST
]);

// Default initialization of Database
DBConn::Instance()->Connect('root', 'usbw', 'sera_que_roda');
