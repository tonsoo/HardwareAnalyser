<?php

$db = DBConn::Instance();

$Jogo = $db->Read('jogo', 'WHERE nome=:nome', $Input);
if($Jogo){
    $db->Update('jogo', 'WHERE id=:id', $Jogo['id'], $Input);
} else {
    $db->Create('jogo', $Input);
}

set_response(HTTP_OK, 'OK', [
    'jogo' => [],
    'status' => $Jogo ? 'exists' : 'new'
]);
