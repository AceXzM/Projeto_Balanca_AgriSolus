**PROJETO DE ENGENHARIA REVERSA ESTAÇÃO DE MONITORAMENTO INTELIGENTE**

**SOBRE O PRODUTO**
O que é uma estação de monitoramente inteligente?
É um produto utilizado em aviários para monitoramento de parâmetros como peso, umidade, CO2 e temperatura, todos estes sendo importantissímos no processo de criação de frangos.
Casos estes parâmetros não sejam bem monitorados e controlados com os devidos equipamentos com precisão, podem ocasionar na morte dos mesmos.

**ASPECTOS TÉCNICOS DO PRODUTO**
Duas placas eletrônicas, sendo uma para alimentação contendo um transformador de 110-220V/24V, juntamente com uma ponte diodos que servem para alimentar a placa principal.
A placa principal contém dois microcontroladores, sendo o primeiro um **STM32G070**, responsável por captar dados dos sensores como um sensor **IR de CO2 e Temperatura**, juntamente com um de **umidade** e uma **célula de carga de 25kg**.
Entre estes microcontroladores temos um comunicação por meio do **protocolo UART** (compartilhamento de GND's), dos quais os mesmos trocam dados dos sensores, e pelo **protocolo I2C**
o STM32 envia estes dados para uma tela **OLED**. O outro microcontrolador, responsável por receber os dados do STM32, fazer um pré-tratamento dos dados, deixá-los em formato **JSON** para enviá-los por meio de um pacote com todos estes dados, e enviar para um servidor BROKER do protocolo MQTT, que o mesmo se torna um client publisher do protocolo.
Após isso temos um "Microserviço" em **node.js**, em **TypeScript** que se torna um client subscriber do protocolo, assinando no tópico **"sensores"** do qual o publisher está publicando os dados em JSON. Após isso o microserviço envia estes dados para um Firestore Database, o qual foi escolhido por ser gratuito durante um período e apenas para uma demonstração breve do sistema funcionando, porém, pensando em escalabilidade e outros aspectos um ideal seria o AWS. 
Após armazenado o dado está disponível para ser captado por uma **API**, da arquitetura **REST**, que espera a requisição de um aplicativo em **FLUTTER** e entrega o dado.

**FLUXOGRAMA BÁSICO DO PROJETO**
<img width="189" height="249" alt="image" src="https://github.com/user-attachments/assets/b899a044-33de-44c6-997a-d9dbebd007ae" />

**COMO USAR OS CÓDIGOS?**
Softwares utilizados:
Arduino IDE - para ESP32.
Eclipse Mosquitto - para Protocolo MQTT.
VsCode - para o Microserviço em Node.js, API e Servidor.
Firebase - para DB em nuvem.

Todo passo a passo de como compilar e debbugar o código em node.js está disponível na branch "BACKEND".
