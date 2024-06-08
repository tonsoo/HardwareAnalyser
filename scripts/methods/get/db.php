<?php

$db = DBConn::Instance();

// $db->Create('tipo_requisito', [
//     'tipo' => 'Novo tipo '.time().rand(0, 1000)
// ]);

// $db->Update('tipo_requisito', 'WHERE id=:id', [
//     'tipo' => 'Editado hahaha'
// ], [
//     'id' => 8
// ]);

// $db->Delete('tipo_requisito', 'WHERE id=:1 OR id=:2', [
//     '1' => 7,
//     '2' => 8
// ]);

$data = $db->Read('tipo_requisito');

set_response(HTTP_OK, 'OK', $data);