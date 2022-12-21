# A aplicação

A aplicação possui 4 passos principais:

- Análise dos argumentos com os quais a aplicação foi lançada
- Estabelecimento da primeira ligação FTP para efetuar login e establecer o modo passivo
- Estabelecimento da segunda ligação FTP para abrir um canal para receber o ficheiro
- Descarregar o ficheiro

Análise dos argumentos:



A aplicação foi concebida para utilizar 2 conexões FTP, assim como foi feito na primeira aula.

- A primeira conexão é realizada na porta 21, no link fornecido em argv[1] de modo a enviar os comandos necessários para efetuar o login. 

- A segunda conexão é establecida assim que na primeira conexão o modo passivo seja permitido e seja retornada a porta para transferir o ficheiro.