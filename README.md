# Cisternas nas Escolas IoT

Aplicação de Internet das coisas (IoT) na melhoria do Projeto Cisternas nas Escolas


## Projeto Cisternas nas Escolas
A ação Cisternas nas Escolas tem como objetivo construir sistemas de captação de água de chuva em escolas da rede pública de São Paulo e realizar atividades educacionais com base no projeto de engenharia para desenvolver habilidades de correlação e raciocínio lógico.

Em 2017, foram implementados 2 projetos técnico-educacionais de captação de água de chuva, na Escola Municipal de Ensino Fundamental Olavo Pezzotti e na Escola Municipal de Ensino Infantil Alberto de Oliveira. A iniciativa foi contemplada pelo edital da Associação Amigos da Poli, que financiou ambos os projetos ao longo do ano.

## Cisternas nas Escolas IoT - Descrição 

O projeto visa medir o volume de água em cisternas que captam água da chuva em escolas públicas de São Paulo.
A primeira escola onde o projeto deve ser implantando é a escola EMEF Olavo Pezotti que fica na Rua Fradique Coutinho, 2200.
A informação de nível da cisterna deve ser enviada a um servidor web e estar acessível somente para a gerência da escola.
Periodicamente as moças da limpeza tem que adicionar cloro na cisterna, mas muitas vezes elas esquecem de fazer isso e acumula uma espécie de musgo no interior da cisterna. Como complemento ao sensor de nível de água seria colocado um pequeno recipiente dentro da cisterna que jogasse pastilhas de cloro periodicamente na cisterna.

Público alvo: 
- Gerência das escolas, para gerenciar o uso da água
- Alunos, podem desenvolver interesse por ciência e engenharia a partir de projetos como esse
- Governo do estado de Sâo Paulo, pode monitorar o uso de recursos hídricos que as escolas fazem


## Cisternas nas Escolas IoT - Descrição técnica

Sensores: Quais sensores serão utilizados, onde eles poderão ser instalados, e a razão dessas escolhas.

Descrição técnica dos sensores: Apresentar os dados técnicos incluindo as características apresentadas no curso.


## Cisternas nas Escolas IoT - Descrição da arquitetura

Os dados serão coletados por um sensor acoplado à cisterna e transmitidos por um dispositivo IoT para a plataforma PubNub (https://www.pubnub.com) que fará a distribuição dos dados obtidos para os diversos dispositivos interessados (servidor de banco de dados da aplicação, usuários da interface gráfica do servidor, etc.). Após armazenados os dados no banco de dados eles estarão disponíveis para uso em predições de volume de água na cisterna, avaliação do histórico de uso, e outras aplicações. Com base nessas informações um usuário com permições de diretor / gestor poderá intervir na atividade da cisterna solicitando que seja despejada uma quantidade de cloro para limpeza da água coletada. Outras informações como previsão meteorológica também poderão ser incluídas na tomada de decisão de como melhor utilizar a água.
