<?php

require 'config/settings.api.php';

$method_used = strtolower($_SERVER['REQUEST_METHOD']);
if(!in_array($method_used, ALLOWED_METHODS)){
    set_response(HTTP_METHOD_NOT_ALLOWED, 'method-not-allowed');
}

$input_filter = null;
switch($method_used){
    case 'get':
        $input_filter = INPUT_GET;
        break;

    case 'post':
        $input_filter = INPUT_POST;
        break;
}

if($input_filter === null){
    set_response(HTTP_METHOD_NOT_ALLOWED, 'method-not-allowed');
}

$Input = filter_input_array($input_filter, FILTER_DEFAULT);

$request_url = $_SERVER['REQUEST_URI'];
echo $_SERVER['PHP_SELF'];