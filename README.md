# MAC6929-Projeto
Projeto de Internet das Coisas (IoT)


# Título do projeto: Mapa Sonoro Urbano

Usar sensores de instensidade sonora para monitorar o ruído em uma localidade. Cada pessoa poderia ter consigo um sensor de intensidade sonora que captaria dados do ambiente de tempos em tempos. Os dados seriam relacionados com a posição do usuário. Com vários usuários usando esses sensores espalhados por uma cidade seria possível criar um mapa sonoro para um dado município. As informações desse mapa estariam disponívis para os usuários para que possam decidir frequentar lugares ounde a poluição sonora seja menor.

Público alvo: Deficientes auditivos (todos os níveis). Pessoas que gostam de lugares mais tranquilos.

# Descrição técnica

Poderiam ser usado o módulo detector de som LM393 para captar a intensidade sonora.
Para manter o controle da localiação do usuário poderia-se usar um módulo Gps Ublox/u-blox.
Também seria necessário um esp8266 para em algum momento conectar com a Internet e enviar os dados coletados.
Outra opção seria tentar inverir a localização do usuário a partir o seu IP e da rede ao qual ele está conectado.


# Descrição da arquitetura

A coleta de dados pode ser feita periódicamente e armazenada temporariamente na memória não volátil do microcontrolador enquanto o usuário não estiver conectado a Internet. Ao conectar a Internet os dados já armazenados seriam transmitidos para um servidor e os novos seriam transmitidos no instante da coleta, sem serem salvos na memória do microcontrolador.
Os dados recebidos pelo servidor poderiam ficar acessíveis aos usuários por meio de uma página de Internet e/ou web service.
