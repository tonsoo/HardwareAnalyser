<?php

$db = DBConn::Instance();
$data = $db->Read('processador');

set_response(HTTP_OK, 'OK', [
    'processors' => $data
]);