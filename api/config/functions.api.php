<?php

function set_response(int $code, string $message, array $response = []) : void {

    http_response_code($code);

    header('Content-type: application/json');

    die(json_encode([
        'code' => $code,
        'text' => $message,
        'response' => $response
    ]));
}

function get_url_array(string $url) : array {

    $url = parse_url($url)['path'] ?? '';

    if(substr($url, 0, 1) == '/'){
        $url = substr($url, 1);
    }

    if(substr($url, strlen($url) - 1) == '/'){
        $url = substr($url, 0, strlen($url) - 1);
    }

    if(!$url){
        return [];
    }

    return explode('/', $url);
}
