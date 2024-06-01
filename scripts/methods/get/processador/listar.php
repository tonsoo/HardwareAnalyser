<?php

$proc1 = new Processador(1, 'i3 10100f', 'Intel', 4, 8, 3600, 4100, 8);
$proc2 = new Processador(2, 'amd athlon 3000g', 'AMD', 2, 4, 3200, 3200, 2);

set_response(HTTP_OK, 'OK', [
    'processors' => [
        $proc1->ToArray(),
        $proc2->ToArray()
    ]
]);