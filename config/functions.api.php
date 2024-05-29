<?php

function set_response(int $code, string $message, array $response = []) : void {

    http_response_code($code);

    echo json_encode([
        'code' => $code,
        'text' => $message,
        'response' => $response
    ]);

    die();
}