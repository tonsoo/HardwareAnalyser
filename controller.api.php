<?php

ini_set('display_errors', true);

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

$request_url = get_url_array($_SERVER['REQUEST_URI']);
$php_self = get_url_array($_SERVER['PHP_SELF']);

if(!$request_url){
    set_response(HTTP_BAD_REQUEST, 'no-api-requested');
}

$slice_index = 0;
for($i = 0; $i < count($request_url) && $i < count($php_self); $i++){
    if($request_url[$i] != $php_self[$i]){
        $slice_index = $i;
        break;
    }
}

$request_url = array_slice($request_url, $slice_index);

$render_script = '';
$previous_script = '';
for($i = 0; $i < count($request_url); $i++){
    $url_piece = $request_url[$i];
    $script = "{$previous_script}/{$url_piece}.php";
    if(file_exists(PATH_METHODS.$script) && is_file(PATH_METHODS.$script)){
        $render_script = PATH_METHODS.$script;
        break;
    }

    $previous_script .= "/{$url_piece}";
}

if(!$render_script || !file_exists($render_script) || !is_file($render_script)){
    set_response(HTTP_NOT_FOUND, 'not-found');
}

require $render_script;
