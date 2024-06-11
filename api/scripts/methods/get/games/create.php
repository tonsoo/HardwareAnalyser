<?php

try{
    $Jogo = Jogo::FromArray($Input);
} catch (MissingInputException $e){
    set_response(HTTP_BAD_REQUEST, 'missing-input', [
        'error' => $e->getMessage()
    ]);
}

$JogoArray = $Jogo->ToArray();

$db = DBConn::Instance();

$JogoDb = $db->Read('jogo', 'WHERE Id=:Id OR Nome=:Nome', $JogoArray);
if($JogoDb){
    $db->Update('jogo', 'WHERE Id=:Id', $JogoDb[0], $JogoArray);
} else {
    $db->Create('jogo', $JogoArray);
}

set_response(HTTP_OK, 'OK', [
    'jogodb' => $JogoDb[0],
    'jogo' => $JogoArray,
    'status' => $JogoDb ? 'exists' : 'new'
]);
