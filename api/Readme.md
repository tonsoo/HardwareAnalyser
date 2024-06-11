# Api de controle de dados

## Metodos do Processador:

### GET /processador/cadastrar
Ira realizar o cadastro de um processador ao banco de dados.

Id: Numero do id do processador no branco de dados. Opicional.
Nome: Nome do processador, ex: "i3 10100f". Obrigatorio.
Fabricante: Nome do fabricante do processador. Obrigatorio.
Nucleos: Quantidade de nucleos do processador. Obrigatorio.
Threads: Quantidade de threads do processador. Obrigatorio.
Clock: Velocidade do clock em MHz. Obrigatorio
ClockTurbo: Velocidade maxima do clock apos ativação do modo turbo ou utilizando overclock. Obrigatorio.
Cache: Memoria cache do processador em Mb. Obrigatorio.

### GET /processador/listar
Fara a lista de todos os processadores cadastrados.

Não há necessidade de encaminhar parametros.


### GET /processador/score
Retorna os dados de um processador junto de sua nota.

Id: Numero do id do processador. Obrigatorio.

## Metodos dos jogos:

### GET /games/create
Faz a inserção de um jogo ao banco de dados.

Id: Numero do id do jogo. Opicional.
Nome: Nome do jogo. Obrigatorio.
Lancamento: Data de lançamento do jogo no formato YYYY-mm-dd. Obrigatorio.
Descricao: A descrição do jogo. Obrigatorio.
LinkDownload: Link para a pagina de download ou pagina oficial do jogo. Obrigatorio.