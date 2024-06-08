<?php

try{
    $Processador = Processador::FromArray($Input);
} catch(MissingInputException $e){
    set_response(HTTP_BAD_REQUEST, 'missing-input', [
        'error' => $e->getMessage()
    ]);
}

$procArray = $Processador->ToArray();

$db = DBConn::Instance();

$query = 'WHERE (Nome=:Nome AND Fabricante=:Fabricante OR Modelo=:Modelo) OR (Clock=:Clock AND ClockTurbo=:ClockTurbo AND Nucleos=:Nucleos AND Threads=:Threads)';
$db_Processador = $db->Read('processador', $query, $procArray);

if(!$db_Processador){
    $db->Create('processador', $procArray);
}

set_response(HTTP_OK, 'OK', [
    'cpu' => $procArray,
    'status' => $db_Processador ? 'exists' : 'new'
]);