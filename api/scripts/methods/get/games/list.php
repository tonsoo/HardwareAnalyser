<?php

$db = DBConn::Instance();

set_response(HTTP_OK, 'OK', [
    'jogos' => $db->Read('jogo')
]);