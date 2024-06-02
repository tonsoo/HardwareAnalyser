<?php

ini_set('display_errors', true);

require 'config/settings.api.php';

$method_used = strtolower($_SERVER['REQUEST_METHOD']);
$input_filter = ALLOWED_METHODS[$method_used] ?? null;

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
    if($request_url[$i] == $php_self[$i]){
        continue;
    }

    $slice_index = $i;
    break;
}

$request_url = array_slice($request_url, $slice_index);

$slice_index = 0;
$render_script = '';
$previous_script = '';
for($i = 0; $i < count($request_url); $i++){
    $previous_script .= "/{$request_url[$i]}";
    $script = PATH_METHODS."{$method_used}{$previous_script}.php";

    if(!file_exists($script) || !is_file($script)){
        continue;
    }

    if(count(explode('/', $script)) > count(explode('/', $render_script))){
        $render_script = $script;
        $slice_index = $i;
    }
}

if(!$render_script || !file_exists($render_script) || !is_file($render_script)){
    set_response(HTTP_NOT_FOUND, 'not-found');
}

$request_url = array_slice($request_url, $slice_index + 1);

unset($previous_script);
unset($script);
unset($method_used);

require $render_script;

set_response(HTTP_OK, 'OK');
