<?php

$procId = $request_url[0];

$db = DBConn::Instance();
$Processador = $db->Read('processador', 'WHERE Id=:Id', [
    'Id' => $procId
]);

if(!$Processador){
    set_response(HTTP_NOT_FOUND, 'not-found');
}

$Processador = Processador::FromArray($Processador[0]);

set_response(HTTP_OK, 'OK', [
    'cpu' => $Processador->ToArray(),
    'score' => $Processador->Nota()
]);
