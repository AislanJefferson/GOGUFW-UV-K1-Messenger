# Manual do Usuário — GOGUFW 3.6.2m

Firmware customizado para **Quansheng UV-K1 / UV-K5 V3** (MCU PY32F071 + RF BK4829), baseado no **F4HWN 6.3.0**.
Este manual descreve o build **Fusion** (o preset oficial) do código atual, versão `v3.6.2m`.

> [!CAUTION]
> Instale somente em rádios **UV-K1 / UV-K5 V3** (PY32F071 + BK4829). **Não** instale em UV-K5 V1/V2 ou em outros rádios com BK4819.
> Faça backup com o CHIRP antes de atualizar e mantenha uma imagem de recuperação DFU à mão.

---

## Sumário

1. [Visão geral](#1-visão-geral)
2. [Instalação e atualização](#2-instalação-e-atualização)
3. [Conceitos básicos](#3-conceitos-básicos)
4. [Referência rápida de atalhos](#4-referência-rápida-de-atalhos)
5. [Tela principal (VFO / MR)](#5-tela-principal-vfo--mr)
6. [Menu](#6-menu)
7. [Ações das teclas laterais](#7-ações-das-teclas-laterais)
8. [Scan (varredura)](#8-scan-varredura)
9. [Messenger](#9-messenger)
10. [HEARD, Range Check e AUTO CHECK](#10-heard-range-check-e-auto-check)
11. [Criptografia: CryKey](#11-criptografia-crykey)
12. [Scrambler de voz (fixo e HOP)](#12-scrambler-de-voz-fixo-e-hop)
13. [Largura de banda: W, N e N+](#13-largura-de-banda-w-n-e-n)
14. [CALLTX (melodias de chamada)](#14-calltx-melodias-de-chamada)
15. [Rádio FM comercial](#15-rádio-fm-comercial)
16. [Search Frequency e Search Tone](#16-search-frequency-e-search-tone)
17. [Spectrum](#17-spectrum)
18. [Dual Watch e Full Watch](#18-dual-watch-e-full-watch)
19. [Apps overlay](#19-apps-overlay)
20. [Multiboot](#20-multiboot)
21. [F Lock e menu oculto (Service)](#21-f-lock-e-menu-oculto-service)
22. [Políticas por canal e ícones](#22-políticas-por-canal-e-ícones)
23. [Editor de texto](#23-editor-de-texto)
24. [Bloqueio de teclado e navegação UV-K1 × UV-K5](#24-bloqueio-de-teclado-e-navegação-uv-k1--uv-k5)
25. [CHIRP](#25-chirp)
26. [Dicas e melhores práticas](#26-dicas-e-melhores-práticas)
27. [Otimizações](#27-otimizações)
28. [Solução de problemas](#28-solução-de-problemas)
29. [Compilar a partir do código](#29-compilar-a-partir-do-código)

---

## 1. Visão geral

O GOGUFW mantém o conjunto de recursos de rádio do F4HWN e adiciona:

| Recurso | Resumo |
|---|---|
| **Messenger** | Mensagens de texto rádio-a-rádio por FSK, com ACK, retentativa e rascunhos |
| **HEARD / Range Check** | Lista das estações ouvidas e teste de alcance com várias estações (PING/PONG) |
| **AUTO CHECK** | Teste de alcance automático periódico em segundo plano |
| **Criptografia (CryKey)** | Texto do Messenger cifrado (XTEA-CTR) com frase-senha compartilhada |
| **Scrambler HOP** | Inversão de voz com portadora que salta a cada 500 ms |
| **CALLTX** | Cinco melodias de chamada transmitidas pelo rádio |
| **FM comercial** | 48 memórias nomeadas, 4 bandas e Live RSSI (via apps) |
| **Search** | Busca de frequência e de tom (CTCSS/DCS) com gravação em canal |
| **Spectrum** | Analisador de espectro em VFO ou em canais de memória |
| **Full Watch** | Escuta de até 4 canais (A, B e 2 prioritários) |
| **Apps overlay** | 48 slots para aplicativos instaláveis (APRS, SSTV, jogos etc.) |
| **Multiboot** | Até 5 firmwares no rádio, com bancos de configuração separados |
| **F Lock BRASIL** | TX restrito por padrão aos canais de uso geral da Anatel |

---

## 2. Instalação e atualização

### 2.1 Arquivos

| Arquivo | Uso |
|---|---|
| `GOGUFW-vX.Y.Z.bin` | Firmware normal |
| `GOGUFW-vX.Y.Z-multiboot.bin` | Firmware no formato multiboot do F4HWN |
| `Gogufw_X.Y.Z_chirp_module.py` | Módulo CHIRP da mesma versão |
| `FMVFO.app` + `FMMemory.app` | Rádio FM (os **dois** são obrigatórios) |
| `Search.app` | Necessário para F+4 e F+\* |
| `Apps.zip` | Pacote de apps compatíveis |

### 2.2 Passo a passo

1. Faça backup do rádio com o CHIRP (o módulo antigo serve para o backup).
2. Grave o firmware pelo método de sua preferência (UV Studio, k5prog, DFU etc.).
3. Ligue o rádio normalmente e instale os `.app` pela página **Apps** do UV Studio (Armel).
4. No CHIRP, carregue o **módulo da mesma versão** do firmware (seção 25).

> [!IMPORTANT]
> Apps feitos para a antiga API experimental GOGUFW 4–9 **não funcionam** e são rejeitados de propósito. Use apps da API 3 oficial do F4HWN 6.3.0 (os publicados a partir do GOGUFW 3.3.1).

### 2.3 Primeiro boot após atualizar

- Na primeira vez, o multiboot cria um backup do firmware principal no slot 0 (`Init Main`). **Não desligue o rádio durante essa etapa.**
- Quando a versão muda, o firmware zera KEY_LOCK, MENU_LOCK, SET_KEY e SET_INV.
- O rádio liga direto, sem tela de boas-vindas, bipes ou atraso (a opção `POnMsg` foi removida).

---

## 3. Conceitos básicos

### 3.1 Teclas

| Tecla | Nome neste manual |
|---|---|
| Teclado numérico | **0–9** |
| Asterisco | **\*** (STAR) |
| Função | **F** |
| Menu | **MENU** |
| Setas | **UP / DOWN** |
| Sair | **EXIT** |
| Botão lateral de cima | **SIDE1** (F1) |
| Botão lateral de baixo | **SIDE2** (F2) |
| Transmitir | **PTT** |

- **Curto** = apertar e soltar. **Segurar** = manter pressionado por mais de 400 ms.
- **F + tecla** = aperte F (aparece o ícone F), solte, e aperte a tecla. O modo F expira em 8 s.
- UP/DOWN repetem a cada 80 ms enquanto seguros.

### 3.2 Modos VFO e MR

- **VFO**: você digita/sintoniza qualquer frequência.
- **MR** (memória): você navega pelos 1024 canais gravados.
- **F+3** alterna entre VFO e MR. **F+2** alterna entre o VFO A e o B.

### 3.3 Barra de status (topo da tela)

| Indicador | Significado |
|---|---|
| **F** | Modo F armado |
| Cadeado | Teclado bloqueado |
| Envelope | Há mensagem não lida no Inbox |
| Elo de corrente | Resultado do AUTO CHECK (fixo = PONG recebido; piscando = sem resposta) |
| VOX | VOX ligado (tem prioridade sobre o ícone do AUTO CHECK) |
| `HOLD` | O Messenger está segurando o RX num canal (ACK/PONG em andamento) |
| `FWR` / `DWR` | Full Watch / Dual Watch ativo |
| `MENU` ou nome da categoria | Você está no menu |
| Lista de scan (`ALL`, nome, `+`) | Lista ativa durante a varredura (`+` = prioridade ligada) |
| Bateria | Nível, tensão ou porcentagem (conforme `BatTxt`) |

---

## 4. Referência rápida de atalhos

### 4.1 Tela principal

| Atalho | Função |
|---|---|
| **F + MENU** | Abre o **Messenger** |
| **F + 0** | **Rádio FM** (precisa de `FMVFO.app` e `FMMemory.app`) |
| **F + 1** | VFO: próxima banda · MR: copia o canal para o VFO |
| **F + 2** | Alterna VFO A / B (com Full Watch: A → B → C → D) |
| **F + 3** | Alterna VFO / MR |
| **F + 4** | **Search Frequency** (precisa de `Search.app`) |
| **F + 5** | **Spectrum** |
| **F + 6** | Potência de TX (LOW1 → … → HIGH) |
| **F + 7** | **Lista de apps** |
| **F + 8** | Luz de fundo: sempre acesa → sempre apagada → configuração salva |
| **F + 9** | **CALLTX** (transmite a melodia selecionada) |
| **F + \*** | **Search Tone** (precisa de `Search.app`) |
| **F + UP/DOWN** | Squelch +1 / −1 (durante scan: troca a lista) |
| **F + SIDE1 / SIDE2** | Aumenta / diminui o passo de frequência |
| **Segurar F** | Bloqueia / desbloqueia o teclado |
| **Segurar 0–4, 6** | Mesmo efeito de F + tecla |
| **Segurar 5** | MR: muda a lista de scan do canal · VFO: liga/desliga a faixa de scan entre VFO A e B |
| **Segurar 7** | Liga / desliga o **VOX** |
| **Segurar 8** | **Reverse** (troca as frequências de RX e TX) |
| **Segurar 9** | Vai para o canal **1 Call** |
| **Segurar \*** | Inicia o **scan** |
| **\* curto** | Modo de digitação DTMF |
| **Segurar MENU** | Ação `M Long` (no scan: exclui o canal/frequência) |
| **Segurar EXIT** | Restaura o VFO, cancela F/DTMF/digitação e apaga a faixa de scan |

> [!NOTE]
> O HEARD **não tem atalho de teclado** no build atual: F+7 abre a lista de apps. Para abrir o HEARD, atribua a ação **HEARD** a uma tecla lateral ou ao `M Long` (seção 7).

### 4.2 Ao ligar o rádio

| Segurar ao ligar | Resultado |
|---|---|
| **MENU** | Seletor do Multiboot |
| **PTT + SIDE1** | Menu oculto (Service) liberado, abrindo em **F Lock** |

---

## 5. Tela principal (VFO / MR)

### 5.1 Digitando frequência ou canal

- **Em VFO**: digite 6 dígitos em MHz (7 dígitos acima de 1 GHz). Exemplo: `1 4 5 5 0 0` = 145,500 MHz. Cada dígito já aplica o valor parcial; a frequência é ajustada à banda válida e ao passo.
- **Em MR**: digite até 4 dígitos do canal. Com 4 dígitos confirma na hora; senão confirma em cerca de 2 s (ou com **MENU**).
- **EXIT** apaga o último dígito; com todos apagados, restaura o VFO anterior.
- **Frequência digitada entra em simplex limpo**: ao digitar uma frequência no VFO, CTCSS/DCS (RX e TX), direção e valor do offset e Reverse voltam para OFF. Potência, largura de banda, passo, modulação e scrambler são mantidos. Para usar uma repetidora no VFO, configure `TxODir`, `TxOffs` e os tons **depois** de digitar a frequência, ou copie um canal gravado com **F+1**. Cancelar com EXIT restaura os valores anteriores.
- **F+3** de MR para VFO volta ao último estado salvo do VFO (cada banda guarda os próprios parâmetros); ele não copia o canal de memória.

### 5.2 Teclas sem F

| Tecla | Função |
|---|---|
| **UP / DOWN** | VFO: um passo · MR: próximo canal válido · Scan: muda a direção |
| **MENU** | Confirma a digitação · sem digitação abre o menu · no scan, para no sinal |
| **EXIT** | Apaga dígito · no scan, para e volta ao ponto de partida |
| **PTT** | Transmite · no scan, só para · no modo DTMF, envia a sequência |
| **SIDE1 / SIDE2** | Ações configuradas (padrão: SIDE1 = MONITOR, SIDE2 = SCAN) |

### 5.3 Modo DTMF (após **\*** curto)

- Teclas: 0–9, MENU = A, UP = B, DOWN = C, EXIT = D, \* = \*, F = #.
- **SIDE1** apaga um caractere (vazio = sai).
- **PTT** envia. Sai com **EXIT segurado** ou após 8 s sem tecla.
- Não funciona durante scan, em Full Watch ou com faixa de scan ativa.

### 5.4 Durante a transmissão

- Os números e demais teclas enviam o **tom DTMF** correspondente.
- **SIDE2** segurado envia **tom de 1750 Hz** (abertura de repetidora).

### 5.5 PTT ONEPUSH

Com `SetPTT = ONEPUSH`, o primeiro toque no PTT começa a transmitir e o segundo para. A ação **PTT MODE** alterna entre CLASSIC e ONEPUSH só na sessão atual (não salva).

---

## 6. Menu

### 6.1 Navegação

- **MENU** (na tela principal) abre a **tela de categorias**:
  Channels · Scan · Keys · Power · Display · Timers · Audio · Radio · DTMF · Msg · (Service) · All.
- **UP/DOWN** escolhe; **MENU** abre a categoria (ela lembra a última posição); **EXIT** volta.
- **All** é a lista completa na ordem e numeração originais. Digitar um número na tela de categorias vai direto ao item de mesmo número em **All**.
- Dentro de um item: **UP/DOWN** muda o valor, números digitam o valor direto, **MENU** confirma, **EXIT** cancela.
- Itens que apagam ou gravam (Reset, ChSave, ChDele, SetCfg) pedem confirmação com **MENU** de novo.
- Em `RxCTCS` / `RxDCS`, a tecla **\*** procura o tom/código do sinal recebido (só em FM).
- **PTT** funciona dentro do menu (transmite sem sair).
- **Service** só aparece depois de um boot com PTT+SIDE1 (seção 21).

### 6.2 Lista completa de itens (categoria All)

| # | Item | Categoria | | # | Item | Categoria |
|---|---|---|---|---|---|---|
| 1 | Step | Channels | | 43 | RP STE | Radio |
| 2 | Power | Channels | | 44 | 1 Call | Keys |
| 3 | RxDCS | Channels | | 45 | UPCode | DTMF |
| 4 | RxCTCS | Channels | | 46 | DWCode | DTMF |
| 5 | TxDCS | Channels | | 47 | PTT ID | Channels |
| 6 | TxCTCS | Channels | | 48 | D ST | DTMF |
| 7 | TxODir | Channels | | 49 | D Prel | DTMF |
| 8 | TxOffs | Channels | | 50 | VOX | Radio |
| 9 | W/N | Channels | | 51 | RxMode | Radio |
| 10 | Scramb | Channels | | 52 | Sql | Radio |
| 11 | BusyCL | Channels | | 53 | SetPwr | Channels |
| 12 | Compnd | Channels | | 54 | SetPTT | Keys |
| 13 | Mode | Channels | | 55 | SetTOT | Timers |
| 14 | TXLock | Channels | | 56 | SetEOT | Timers |
| 15 | ChList | Channels | | 57 | SetCtr | Display |
| 16 | ChSave | Channels | | 58 | SetInv | Display |
| 17 | ChDele | Channels | | 59 | SetLck | Keys |
| 18 | ChName | Channels | | 60 | SetMet | Display |
| 19 | ScList | Scan | | 61 | SetGUI | Display |
| 20 | ScPri | Scan | | 62 | SetRxA | Audio |
| 21 | PriCh1 | Scan | | 63 | SetRxG | Audio |
| 22 | PriCh2 | Scan | | 64 | SetTmr | Timers |
| 23 | ScnRev | Scan | | 65 | SetOff | Power |
| 24 | F1Shrt | Keys | | 66 | SetNFM | Channels |
| 25 | F1Long | Keys | | 67 | SetVol | Audio |
| 26 | F2Shrt | Keys | | 68 | SetScn | Scan |
| 27 | F2Long | Keys | | 69 | SetCfg | Channels |
| 28 | M Long | Keys | | 70 | CllTon | Audio |
| 29 | KeyLck | Keys | | 71 | CllVol | Audio |
| 30 | TxTOut | Timers | | 72 | MsgRx | Msg |
| 31 | BatSav | Power | | 73 | MsgAck | Msg |
| 32 | BatTxt | Power | | 74 | MsgBep | Msg |
| 33 | Mic | Audio | | 75 | MsgLed | Msg |
| 34 | MicBar | Audio | | 76 | CllSgn | Msg |
| 35 | ChDisp | Display | | 77 | CryKey | Msg |
| 36 | BLTime | Display | | 78 | RngRsp | Msg |
| 37 | BLMin | Display | | 79 | F Lock | Service |
| 38 | BLMax | Display | | 80 | 350 En | Service |
| 39 | BLTxRx | Display | | 81 | BatCal | Service |
| 40 | Beep | Audio | | 82 | BatTyp | Service |
| 41 | Roger | Radio | | 83 | SetNav | Service |
| 42 | STE | Radio | | 84 | Reset | Service |

> [!NOTE]
> A categoria **Power** trata de bateria e modo sono. A **potência de TX** (item `Power`) fica em **Channels**.

### 6.3 Channels

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **Step** | 0.01 a 500 kHz (24 passos) | 12.5 kHz | Passo de sintonia |
| **Power** | USER, LOW 1 (<20 mW), LOW 2 (125 mW), LOW 3 (250 mW), LOW 4 (500 mW), LOW 5 (1 W), MID (2 W), HIGH (5 W) | LOW 1 | Potência de TX do canal. USER usa o valor de `SetPwr` |
| **SetPwr** | LOW 1 a HIGH | LOW 1 | Potência usada quando o canal está em USER |
| **RxDCS / TxDCS** | OFF, D023N–D754N, D023I–D754I | OFF | Código DCS. O selo `NNN/MM` mostra a posição na lista completa / na lista homologada (`--` = não homologado) |
| **RxCTCS / TxCTCS** | OFF, 67.0–254.1 Hz (50 tons) | OFF | Tom CTCSS. **\*** no RxCTCS/RxDCS busca o tom recebido |
| **TxODir** | OFF, +, − | OFF | Direção do offset de repetidora |
| **TxOffs** | 0 a 999.99999 MHz | — | Valor do offset (6 dígitos ou UP/DOWN) |
| **W/N** | WIDE, NARROW | NARROW | Largura de banda (seção 13) |
| **SetNFM** | NARROW (12,5 kHz), NARROWER (6,25 kHz = N+) | NARROW | O que "Narrow" significa em todos os canais |
| **Scramb** | OFF, 2600–3500 Hz (passos de 100 Hz), HOP | OFF | Scrambler de voz do canal (seção 12) |
| **BusyCL** | OFF, ON | OFF | Impede TX com o canal ocupado |
| **Compnd** | OFF, TX, RX, TX/RX | OFF | Compressor/expansor de áudio |
| **Mode** | FM, AM, USB | FM | Modulação |
| **TXLock** | OFF, ON | ON | ON = respeita o F Lock. **OFF libera TX de voz** neste canal mesmo fora do plano (não vale para o Messenger) |
| **PTT ID** | OFF, UP CODE, DOWN CODE, UP+DOWN CODE, APOLLO QUINDAR | OFF | DTMF ao apertar/soltar o PTT |
| **ChList** | OFF, 01–24, ALL | — | Lista de scan a que o canal pertence |
| **ChSave** | Canal 1–1024 | — | Grava o VFO num canal (pede `SURE?`) |
| **ChDele** | Canal 1–1024 | — | Apaga um canal (pede `SURE?`) |
| **ChName** | Até 10 caracteres | — | Nome do canal (editor da seção 23) |
| **SetCfg** | CFG M, CFG 1–4 | Banco atual | Banco de configuração do multiboot (reinicia o rádio) |

### 6.4 Scan

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **ScList** | 01–24, ALL | 1 | Lista de scan padrão |
| **ScPri** | OFF, ON | ON | Scan de prioridade |
| **PriCh1 / PriCh2** | Canal 1–1024, None | — | Canais prioritários (também usados como C/D no Full Watch) |
| **ScnRev** | STOP · CARRIER 0,25–20 s · TIMEOUT 5 s–2 min | CARRIER 3,5 s | Como o scan retoma após parar num sinal |
| **SetScn** | NORMAL, FAST | NORMAL | Velocidade do scan |

### 6.5 Keys

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **F1Shrt / F1Long** | Ações (seção 7) | MONITOR / NONE | SIDE1 curto / segurado |
| **F2Shrt / F2Long** | Ações (seção 7) | SCAN / NONE | SIDE2 curto / segurado |
| **M Long** | Ações (seção 7) | NONE | MENU segurado |
| **KeyLck** | OFF, 15 s a 10 min | OFF | Bloqueio automático do teclado |
| **SetLck** | KEYS, KEYS+PTT | KEYS | O que o bloqueio trava |
| **SetPTT** | CLASSIC, ONEPUSH | CLASSIC | Modo do PTT |
| **1 Call** | Canal 1–1024 | — | Canal acessado com 9 segurado |

### 6.6 Power (bateria e sono)

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **BatSav** | OFF, 1:1 a 1:5 | 1:4 | Economia de bateria no RX |
| **BatTxt** | NONE, VOLTAGE, PERCENT | PERCENT | Texto da bateria na barra de status |
| **SetOff** | OFF, 1 min a 2 h | 1 h | Modo sono após inatividade: tela e luz apagam e o LED vermelho pisca. O rádio **não** desliga |

### 6.7 Display

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **ChDisp** | FREQ, CHANNEL NUMBER, NAME, NAME+FREQ | FREQ | Como os canais aparecem |
| **BLTime** | OFF, 5 s a 5 min, ON | 1 min | Tempo da luz de fundo |
| **BLMin** | 0–9 | 0 | Brilho mínimo |
| **BLMax** | 1–10 | 10 | Brilho máximo |
| **BLTxRx** | OFF, TX, RX, TX/RX | TX/RX | Acende a luz em TX/RX |
| **SetCtr** | 1–15 | — | Contraste do LCD (prévia ao vivo) |
| **SetInv** | OFF, ON | OFF | Inverte as cores do LCD |
| **SetMet** | TINY, CLASSIC | — | Estilo do S-meter |
| **SetGUI** | TINY, CLASSIC | — | Estilo da tela principal |

### 6.8 Timers

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **TxTOut** | 30 s a 15 min | 1 min | Limite de tempo de transmissão |
| **SetTOT** | OFF, SOUND, VISUAL, ALL | — | Aviso de fim do tempo de TX |
| **SetEOT** | OFF, SOUND, VISUAL, ALL | — | Aviso de fim de recepção (VISUAL pisca o LED verde) |
| **SetTmr** | OFF, ON | — | Cronômetro de RX/TX na barra de status |

### 6.9 Audio

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **Mic** | +1.5 a +31.5 dB (9 níveis) | +16.0 dB | Ganho do microfone |
| **MicBar** | OFF, ON | ON | Barra de áudio do microfone no TX |
| **Beep** | OFF, ON | ON | Bipe das teclas |
| **SetVol** | OFF, 01–63 | 58 | Volume de RX |
| **SetRxA** | FM: FLAT, CLEAN, MID, BOOST, MAX · AM: SHARP, STOCK, OPEN · USB | FLAT / SHARP | Perfil de áudio de RX |
| **SetRxG** | OFF, ON | ON | Gate de áudio de RX do BK4829. OFF deixa passar modulação fraca, com mais chiado entre palavras. Com o compander de RX ligado fica sempre ON (`ON (COMPND)`) |
| **CllTon** | TONE1–TONE5 | TONE1 | Melodia do CALLTX (prévia ao navegar) |
| **CllVol** | LOW, HIGH | HIGH | Nível do gerador de tom do CALLTX |

### 6.10 Radio

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **Sql** | 0–9 | 1 | Nível de squelch |
| **STE** | OFF, ON | ON | Elimina o ruído de cauda do squelch |
| **RP STE** | OFF, 1–10 × 100 ms | OFF | Elimina a cauda da repetidora |
| **Roger** | OFF, ROGER, MDC | OFF | Sinal de fim de TX (prévia ao navegar) |
| **VOX** | OFF, 1–10 | OFF | Transmissão ativada por voz |
| **RxMode** | MAIN ONLY, DUAL RX RESPOND, CROSS BAND, MAIN TX DUAL RX, FULL RX RESPOND, MAIN TX FULL RX | DUAL RX RESPOND | Modo de recepção (seção 18) |

### 6.11 DTMF

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **UPCode / DWCode** | Só leitura no rádio | 12345 / 54321 | Códigos enviados no início/fim do TX (edite pelo CHIRP) |
| **D ST** | OFF, ON | ON | Ouvir os tons DTMF enviados |
| **D Prel** | 30–990 ms | 300 ms | Pré-carga antes do DTMF |

### 6.12 Msg (Messenger)

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **MsgRx** | OFF, ON | ON | Recepção de mensagens (também necessária para ACK, PONG e HOP no RX) |
| **MsgAck** | OFF, ON | **OFF** | Envia e espera confirmação de entrega |
| **MsgBep** | OFF, ON | ON | Alerta de nova mensagem, de ACK e do Range Check |
| **MsgLed** | OFF, GREEN, YELLOW | GREEN | LED de mensagem não lida |
| **CllSgn** | Até 6 caracteres | UVK1 | Indicativo da estação (também usado pelos apps APRS) |
| **CryKey** | Até 10 caracteres | vazio (`--`) | Frase-senha do HOP e do Messenger cifrado (seção 11) |
| **RngRsp** | OFF, ON | ON | Responde automaticamente aos PINGs (PONG) |

### 6.13 Service (oculto)

Veja a seção 21.

---

## 7. Ações das teclas laterais

Configure em **Keys → F1Shrt, F1Long, F2Shrt, F2Long, M Long** (ou pelo CHIRP).

| Ação | O que faz |
|---|---|
| **NONE** | Nada |
| **FLASH LIGHT** | Liga/desliga a lanterna |
| **POWER** | Avança a potência (LOW1 → HIGH) e salva no canal |
| **MONITOR** | Abre/fecha o squelch |
| **SCAN** | Inicia o scan · durante o scan de memória, troca de lista |
| **VOX** | Liga/desliga o VOX |
| **FM RADIO** | Abre o rádio FM (igual a F+0) |
| **LOCK KEYPAD** | Bloqueia/desbloqueia o teclado |
| **VFO A VFO B** | Alterna VFO A / B |
| **VFO MEM** | Alterna VFO / MR |
| **MODE** | Avança a modulação FM → AM → USB |
| **RX MODE** | Percorre os 6 modos de RxMode |
| **MAIN ONLY** | Liga/desliga "só principal" (guarda e restaura Dual Watch/Cross Band) |
| **PTT MODE** | Alterna CLASSIC/ONEPUSH na sessão (não transmite e não salva) |
| **WIDE NARROW** | Alterna a banda do VFO atual, temporariamente |
| **MUTE** | Silencia/libera o áudio de RX |
| **RxA** | Avança o perfil de áudio de RX |
| **MESSENGER** | Abre o Messenger (de novo, na tela inicial do Messenger, volta ao rádio) |
| **HEARD** | Abre o HEARD (de novo, volta ao rádio) |
| **CALLTX** | Transmite a melodia de chamada (igual a F+9) |
| **SCRAMBLE** | Liga/desliga o scrambler do canal atual. Ao religar, restaura o último tipo usado; após ligar o rádio, o tipo padrão é **HOP** |

---

## 8. Scan (varredura)

### 8.1 Iniciar e parar

- **Iniciar**: segure **\*** ou use uma tecla lateral com a ação **SCAN**.
- **Faixa entre VFOs**: em VFO, segure **5** para ligar a faixa de scan entre as frequências do VFO A e do B. O scan passa a varrer só essa faixa. **EXIT segurado** apaga a faixa.

| Tecla | Resultado |
|---|---|
| **MENU** curto ou **PTT** | Para e fica no sinal encontrado |
| **EXIT** curto | Para e volta ao ponto de partida |
| **EXIT** segurado | Para e apaga a faixa de scan |

### 8.2 Durante o scan

- **UP/DOWN**: muda a direção e pula para o próximo.
- **Trocar a lista**: digite dois números (`00` = todas, `01`–`24`) ou use **F+UP/DOWN**. A barra de status mostra a lista ativa.
- **Excluir**: com o scan parado num sinal, **segure MENU**. O canal/frequência sai da varredura e o scan continua. Exclusões valem até desligar o rádio.
- **F+1** com scan pausado copia o canal para o VFO.

### 8.3 Indicadores

- **Sparkline de RSSI**: gráfico dos últimos passos na linha do VFO.

### 8.4 Listas de canais

- Em MR, **segure 5** para avançar a lista de scan do canal atual (ou tirar a exclusão).
- `ChList` no menu faz o mesmo. `ScList` define a lista padrão.

---

## 9. Messenger

Mensagens curtas de texto entre rádios GOGUFW, por FSK. Funciona em segundo plano: você recebe mensagens em qualquer tela.

### 9.1 Abrir

- **F + MENU**, ou a ação **MESSENGER** numa tecla lateral (que também fecha).

### 9.2 Telas e teclas

| Tela | UP/DOWN | MENU | F | EXIT |
|---|---|---|---|---|
| **Início** (INBOX / COMPOSE / SENT / DRAFTS) | Move | Abre | — | Volta ao rádio |
| **INBOX / SENT** | Move | Lê | Apaga | Volta |
| **DRAFTS** | Move | Edita | — | Volta |
| **Leitura** | Mensagem anterior/próxima | INBOX: responde (`RE: `) · SENT: reenvia | Apaga | Volta à lista |
| **COMPOSE** | Move o cursor | **Envia** (num rascunho: salva e envia) | Apaga caractere | Volta sem enviar |

- O rodapé mostra `TO:` com o nome do canal ou a frequência de TX: é para onde a mensagem vai.
- Pelo código, o **PTT não transmite voz** nas telas do Messenger. Saia para a tela principal para falar.

### 9.3 Limites

| Item | Limite |
|---|---|
| Texto | 36 caracteres (vazio vira `EMPTY`) |
| Inbox | 16 mensagens |
| Sent | 8 mensagens |
| Drafts | 8 rascunhos editáveis |
| Indicativos mostrados por ACK | 3 |

Rascunhos de fábrica: `OK`, `NEED HELP`, `WHERE ARE YOU?`, `ON THE WAY`, `ARRIVED SAFE`, `CALL ME`, `NEGATIVE`, `BATTERY LOW`.

> [!WARNING]
> **Inbox, Sent e HEARD ficam só na RAM.** Elas se perdem ao desligar, reiniciar, trocar de slot do multiboot ou mudar o `SetCfg`. Rascunhos e configurações ficam salvos.

### 9.4 Como o envio funciona

1. O rádio verifica se o TX é permitido (seção 9.6).
2. Envia um quadro **WAKE** invisível e aguarda 120 ms, para acordar rádios em economia de bateria ou Dual Watch.
3. Envia a mensagem (cerca de 1,2 s no ar) e grava em **Sent**.
4. **Com `MsgAck = ON`**: espera ACK por 6 s. Sem resposta, faz **1 retentativa** e espera mais 6 s. Se o canal estiver ocupado, a retentativa é adiada em passos de 0,5 s.
5. No primeiro ACK, toca um bipe curto e continua ouvindo por mais 2,5 s para coletar até 3 indicativos.

O FSK sempre sai em **Narrow** (ou N+, com `SetNFM = NARROWER`), mesmo num canal Wide.

**Marcadores em SENT:**

| Marcador | Significado |
|---|---|
| `+` | Entregue (ACK recebido) |
| `x` | Falhou (sem ACK após a retentativa) |
| `?` | Pendente ou sem pedido de ACK |

A tela de leitura mostra `ACK: ID1 ID2 ID3` com quem confirmou.

> [!TIP]
> Para ter confirmação de entrega, deixe **MsgAck = ON nos dois rádios**: quem envia espera o ACK, e quem recebe só responde se o seu próprio MsgAck estiver ligado.

### 9.5 Como a recepção funciona

- Requer **MsgRx = ON** e **VOX = OFF**.
- O áudio fica abafado durante o quadro FSK.
- Mensagens duplicadas (mesmo remetente e ID) não entram de novo no Inbox, mas o ACK é reenviado.
- O ACK sai com atraso aleatório de 0,8 a 3 s, para evitar colisão entre vários rádios.
- **Alertas** (controlados por `MsgBep`):
  - nova mensagem: quatro notas de 932 Hz em dois pares, estilo "Nokia";
  - ACK: bipe curto de confirmação.
- **LED** (`MsgLed`): pisca em dupla a cada 3 s enquanto houver mensagem não lida.
- **Envelope** na barra de status enquanto houver mensagem não lida.
- Enquanto um app overlay está aberto, ACK, retentativa, alertas e LED do Messenger podem ficar pausados (comportamento observado no código, ainda não testado no rádio).

### 9.6 TX BLOCKED

Antes de cada mensagem, retentativa, ACK, PING ou PONG, o rádio verifica as condições abaixo. Se alguma falhar, aparece o aviso **TX BLOCKED** com o motivo:

| Motivo exibido | Causa | Como resolver |
|---|---|---|
| `NO FSK SELECTED` | O canal tem a política **No FSK TX** | Use outro canal ou mude a política no CHIRP |
| `TX FREQ BLOCKED` | Frequência fora do F Lock ou da faixa de RX | Use uma frequência permitida (seção 21) |
| `FM MODE REQUIRED` | Modulação AM ou USB | Mude `Mode` para FM |
| `CONFIG IN PROGRESS` | O CHIRP está conectado/programando | Aguarde terminar |
| `BATTERY BLOCK` | Bateria fraca (ou tensão excessiva) | Carregue a bateria |
| `FSK NOT ALLOWED` | Outros casos | — |

> [!NOTE]
> `TXLock = OFF` libera a **voz** fora do plano F Lock, mas **não** libera o Messenger.

### 9.7 Com Dual Watch / Full Watch

- O envio usa o canal exibido (A, B, C ou D).
- ACK e PONG saem **exatamente no canal que recebeu**, inclusive canais prioritários C/D.
- Durante a troca de mensagens, a alternância do Dual/Full Watch e a economia de bateria ficam suspensas; a barra mostra `HOLD`.

---

## 10. HEARD, Range Check e AUTO CHECK

### 10.1 HEARD

Lista as últimas 6 estações ouvidas pelo Messenger. Cada linha mostra:

- indicativo e tensão da bateria (quando disponível);
- barra e valor numérico de RSSI;
- tipo do pacote: `MSG`, `ACK`, `PNG` (PING) ou `PON` (PONG);
- idade.

**Abrir**: atribua a ação **HEARD** a uma tecla lateral ou ao `M Long` (a ação também fecha).

| Tecla | Função |
|---|---|
| **UP/DOWN** | Rola a lista |
| **MENU** | Abre o menu: **SEND PING** / **AUTO CHECK** |
| **EXIT** | Volta ao rádio |

### 10.2 Range Check (SEND PING)

1. No HEARD, **MENU → SEND PING**.
2. O rádio envia o PING e escuta por **12 s**.
3. Cada estação com `RngRsp = ON` responde com um PONG em um momento aleatório entre 3 e 9 s, levando a tensão da própria bateria.
4. As estações que responderam sobem para o topo da lista com tensão e RSSI.

### 10.3 AUTO CHECK

- No HEARD, **MENU → AUTO CHECK**: escolha **OFF, 30 s, 1 min, 5 min ou 10 min**.
- O primeiro PING sai na hora. Depois repete no intervalo escolhido, em qualquer tela normal do firmware.
- Fica adiado enquanto há voz em TX/RX, scan ou busca FM.
- **Resultado na barra de status** (no lugar do ícone de VOX):
  - elo **fixo**: recebeu PONG;
  - elo **piscando**: ninguém respondeu nos 12 s, e toca um tom descendente discreto (450 → 320 Hz).
- Um PONG recebido atualiza o HEARD sem bipe.
- Com VOX ligado, o ícone do VOX tem prioridade e esconde o do AUTO CHECK.

---

## 11. Criptografia: CryKey

### 11.1 Definir a frase-senha

1. Abra **MENU → Msg → CryKey** e pressione **MENU**.
2. Na tela **CRYKEY**, digite a frase-senha (até **10 caracteres**, maiúsculas e minúsculas são diferentes) com o editor da seção 23.
3. **MENU** salva. **EXIT** sai sem salvar. Apagar todo o texto e salvar volta à chave padrão.

A chave vale assim que é salva. Ela fica no setor do Messenger, junto do `CllSgn`: no multiboot, cada banco de configuração (seção 20.1) tem a sua. Só grava na Flash quando o texto muda.

> [!IMPORTANT]
> O `Crypto.app` não é mais usado. Ao atualizar de uma versão que usava o app, digite a CryKey de novo no menu.

### 11.2 O que é cifrado

Com uma CryKey definida, **todo** texto do Messenger sai cifrado (XTEA-CTR, salt aleatório por envio).

| Cifrado | Em claro |
|---|---|
| Conteúdo da mensagem (36 bytes) | Indicativo do remetente, ID, flags |
| | Tamanho do texto |
| | ACK, PING, PONG (com tensão da bateria) e WAKE |

- Não há autenticação (MAC): a integridade depende do CRC e de uma verificação de chave de 16 bits.
- Inbox e Sent locais guardam o texto em claro.

### 11.3 Comportamento entre rádios

| Situação | Resultado |
|---|---|
| Mesma CryKey nos dois rádios | Mensagem chega normalmente |
| CryKey diferente ou ausente no receptor | Descartada em silêncio (sem Inbox e sem ACK); o remetente vê `x` |
| Receptor com firmware antigo | Rejeita o pacote cifrado |
| Rádio com CryKey recebendo texto em claro | Recebe normalmente |
| Frase vazia | Chave padrão comum a todos: HOP funciona entre todos e o texto vai **em claro** |

---

## 12. Scrambler de voz (fixo e HOP)

### 12.1 Ativar

- **Menu**: `Channels → Scramb` (por canal).
- **Tecla lateral**: ação **SCRAMBLE** (liga/desliga e lembra o último tipo; padrão HOP).
- **CHIRP**: coluna `Scrambler`.
- Quando ligado, aparece um ícone de **onda dupla** na linha do VFO.

### 12.2 Scrambler fixo (2600–3500 Hz)

Inversão de espectro com portadora fixa. Os dois rádios precisam usar a mesma frequência. Ao navegar pelas opções no menu, a portadora já é aplicada como prévia.

### 12.3 HOP

O HOP troca a portadora de inversão a cada **500 ms**, numa sequência derivada da CryKey.

> [!IMPORTANT]
> **HOP não é criptografia.** Ele só impede a decodificação rápida com um scrambler de portadora fixa.

**Como funciona:**

- A cada PTT, o rádio envia um curto **burst FSK** (cerca de 0,25 s) com a semente. O microfone fica mudo nesse intervalo: **espere um instante depois de apertar o PTT antes de falar**.
- A portadora salta entre 16 valores:
  - N / N+: 2600–3100 Hz;
  - Wide: 2900–3400 Hz.
- A cada **8 s** de TX, um novo burst ressincroniza os receptores. O microfone fica mudo cerca de 0,3 s em cada um.
- No RX, um leve "tique" a cada 0,5 s é normal (o áudio é silenciado ±8 ms em cada troca).
- O receptor tolera desvanecimentos: a sessão só termina 2 s após o último sinal.

**Requisitos no receptor:**

- canal de RX também em **Scramb = HOP**;
- **MsgRx = ON** e **VOX = OFF**;
- mesma CryKey (ou nenhuma nos dois);
- não estar em scan.

**Se o receptor perder o burst** (entrou no meio do câmbio ou chave diferente), ouve áudio embaralhado até o próximo ressincronismo, em até 8 s.

**Outros detalhes:**

- O burst sai mesmo em canais com No FSK TX.
- Um TX HOP sem sessão usa uma portadora fixa derivada da chave, então nunca sai em claro.
- O CALLTX sempre sai **sem** scrambler.

---

## 13. Largura de banda: W, N e N+

| Modo | Canal | Filtro de RX | Filtro de RX (sinal fraco) | Áudio TX |
|---|---|---|---|---|
| **Wide (W)** | 25 kHz | 7,0 kHz | 4,0 kHz | 3,0 kHz |
| **Narrow (N)** | 12,5 kHz | 4,0 kHz | 2,0 kHz | 2,5 kHz |
| **Narrower (N+)** | 6,25 kHz | 3,0 kHz | 2,0 kHz | 2,5 kHz |

- **W/N** define a banda de cada canal. **SetNFM** define se "Narrow" significa N (12,5 kHz) ou N+ (6,25 kHz) em todos os canais.
- Na tela: `W` / `N` / `N+` (GUI CLASSIC) ou `WIDE` / `NAR` / `NAR+` (GUI TINY).
- **Padrões**: canais e VFOs novos em FM ficam em **Narrow**; AM fica em Wide. O Reset deixa `SetNFM = NARROW`.
- N e N+ usam o mesmo filtro para sinal fraco, então **N+ não aumenta o alcance** e reduz o desvio de TX.

---

## 14. CALLTX (melodias de chamada)

- **F + 9** (ou a ação **CALLTX**) transmite a melodia escolhida por pelo menos 3 s e termina a frase sem cortar a última nota.
- Não funciona durante RX ou TX (bipe duplo).
- Sai sem Roger/MDC e sem scrambler.
- **Configuração** (Audio): `CllTon` escolhe TONE1–TONE5 (com prévia), `CllVol` escolhe LOW ou HIGH.

| Melodia | Notas |
|---|---|
| TONE1 | Alternância 1180 / 1580 Hz |
| TONE2 | 1800 / 1350 Hz |
| TONE3 | Escala subindo de 820 a 1760 Hz |
| TONE4 | 1850 / 1450 / 1050 Hz |
| TONE5 | 980 Hz seguido de 1650 Hz duplo |

> [!NOTE]
> O CALLTX é uma transmissão real: respeita F Lock e TXLock como a voz.

---

## 15. Rádio FM comercial

Requer **os dois** apps `FMVFO.app` e `FMMemory.app`. Sem eles, F+0 dá bipe duplo.

### 15.1 Abrir e fechar

- **F + 0** abre o último modo usado (VFO ou Memória).
- **EXIT** ou **F + 0** fecham e voltam exatamente à tela e ao modo VFO/MR de onde você saiu.

### 15.2 Teclas

| Tecla | Função |
|---|---|
| **0–9** | Digita a frequência (VFO) ou o canal com 2 dígitos (Memória) |
| **UP/DOWN** | Sintonia ou canal |
| **\*** | Scan manual (UP/DOWN continua em qualquer direção) |
| **MENU** | Menu FM: Live RSSI e Save (VFO) ou Name/Delete (Memória) |
| **EXIT** | Apaga dígito / fecha menu / para o scan / sai |
| **F+1** ou segurar **1** | Troca a banda (4 bandas configuráveis) |
| **F+3** ou segurar **3** | Alterna FM VFO ↔ FM Memória |
| **F+\*** ou segurar **\*** | Scan automático (confirme com MENU ou \*) |
| **F+0** ou segurar **0** | Sai |

### 15.3 Recursos

- 48 memórias com nome (renomear e apagar pelo menu, editor da seção 23).
- **LIVE RSSI**: medidor de sinal atualizado continuamente.
- Se o canal de rádio selecionado receber sinal, o FM cede a tela e o áudio. Cinco segundos após o fim da recepção, o FM volta no mesmo modo, banda e frequência.

---

## 16. Search Frequency e Search Tone

Requer o `Search.app`.

- **F + 4**: busca a frequência de um transmissor próximo.
- **F + \***: identifica o tom CTCSS (50 tons) ou código DCS (104 códigos) do sinal.
- Os dois partem do canal exibido (inclusive C/D no Full Watch).

| Tecla | Função |
|---|---|
| **\*** | Reinicia a busca |
| **MENU** (com resultado) | Abre **SAVE** |
| **EXIT** ou **PTT** | Sai |

**Salvar o resultado:**

1. **MENU / SAVE**.
2. Escolha o canal com **UP/DOWN** ou 4 dígitos. Vazios aparecem como `CH-xxxx`; ocupados mostram número e nome.
3. **MENU** e confirme `SAVE?` com **MENU** de novo. O canal abre direto em MR.

Canais criados pela busca mantêm a potência que estava em uso, ficam em **NARROW** e, na banda aérea, em **AM**.

---

## 17. Spectrum

**F + 5** abre o analisador.

- **Em MR**: varre os canais da lista de scan ativa (**ScList** ou a última lista escolhida no scan) e mostra nome, frequência, modulação e banda de escuta do canal selecionado.
  - Canais excluídos (MENU segurado) ficam de fora, como no scan de memória.
  - **ALL** pega os canais que estão em alguma lista. Canais com `ChList OFF` ficam de fora.
  - Com a prioridade ligada (`+`), os 2 canais prioritários ficam de fora de uma lista 01–24, mas aparecem em ALL.
  - Se a lista não tiver nenhum canal válido, o Spectrum varre todos os canais válidos e mostra `ALL`.
  - No canto superior esquerdo aparecem a lista e o número de canais varridos, por exemplo `ALL 42`, `RPT 12` ou `03 7`.
  - Embaixo, `M:1` e `M:<n>` são a primeira e a última posição dentro da lista, não o número do canal. Uma lista com os canais 150–154 mostra `M:1` e `M:5`.
- **Em VFO**: varre uma faixa contínua.
  - Durante a escuta, a barra de status mostra o nome do canal MR que tem exatamente a frequência do pico. Varrendo, ou sem canal nessa frequência, a barra fica sem nome.
- **Nos dois modos**: a frequência, a modulação e a banda de escuta do pico ficam na linha de cima do gráfico, com a mesma fonte. Durante a escuta, a modulação e a banda mostradas são as que estão em uso.
  - Em MR, a seta embaixo marca o canal mostrado e fica nele até a varredura achar outro pico.
  - Em VFO, a seta marca o passo do pico atual e volta para a borda esquerda enquanto não há pico. A frequência mostrada continua a do último pico.

### 17.1 Teclas no gráfico

| Tecla | Função |
|---|---|
| **1 / 7** | Passo de varredura + / − |
| **2 / 8** | Passo de deslocamento + / − |
| **3 / 9** | Topo da escala +5 / −5 dB |
| **\* / F** | Nível de disparo sobe / desce |
| **4** | VFO: número de pontos 128 / 64 / 32 / 16 · MR: próxima lista de scan |
| **5** | Digitar uma frequência (\* = ponto decimal, MENU confirma) |
| **0** | Muda a modulação (só em VFO) |
| **6** | Largura de banda de escuta 25k / 12,5k / 6,25k, aplicada na hora (em MR, só escutando: no gráfico vale até o sinal cair, após o PTT até o EXIT; o canal não muda) |
| **UP/DOWN** | Desloca a faixa / retoma a varredura |
| **SIDE1** | Ignora o pico atual (o canal ou passo que está tocando) até sair do Spectrum |
| **SIDE2** | Luz no máximo / mínimo |
| **Segurar MENU** | Volta tudo ao padrão |
| **PTT** | Para no pico e escuta |
| **EXIT** | Sai. Em VFO salva o passo de varredura, o número de pontos, o topo da escala e o nível de disparo; em MR não grava nada |

Em MR, as teclas 1, 7, 2, 8, 5 e 0 ficam inativas: o gráfico mostra a lista inteira, sem uma faixa para deslocar ou digitar, e cada canal usa a própria modulação.

**Ignorar um canal (SIDE1):** funciona como excluir no scan, mas só vale enquanto o Spectrum está aberto e não grava nada na flash. O canal ou passo some do gráfico (a coluna fica vazia), deixa de ser medido, e a varredura continua.
- Sem pico ainda, por exemplo logo depois de outra exclusão, o SIDE1 não faz nada. Espere a varredura achar o próximo sinal.
- Com mais de 128 canais ou passos, a coluna continua desenhada (ela é dividida com canais vizinhos), e só as 15 últimas exclusões ficam valendo.
- As exclusões são apagadas ao sair, ao segurar MENU, ao digitar uma frequência (tecla 5) e, em MR, ao trocar de lista com a tecla 4.

A lista trocada com a tecla 4 vale também para o scan de memória depois de sair. Ela não é gravada na hora: fica na memória do rádio e só vai para a flash quando outra configuração for salva, como acontece ao trocar a lista durante o scan.

### 17.2 Teclas na escuta

| Tecla | Função |
|---|---|
| **UP/DOWN** | Ajuste fino (ou ajuste do ganho, com o menu de ganho aberto) |
| **MENU** | Percorre os ganhos LNAs → LNA → PGA |
| **SIDE1** | Monitor |
| **0** | Muda a modulação da escuta (só em VFO; a escuta continua abrindo com o mesmo nível de disparo) |
| **6** | Largura de banda de escuta (em MR vale até o EXIT; o canal não muda) |
| **EXIT** | Fecha o menu de ganho / volta ao gráfico |
| **PTT** de novo | Leva o pico para o canal MR ou para o VFO ativo e sai, **sem transmitir** |

Em MR, a escuta usa a modulação e a banda do canal, e continua com elas quando o sinal cai e volta.

Ao sair com o segundo PTT:

- **A frequência já está salva num canal MR** (o mais próximo, até meio passo de varredura e no máximo 6,25 kHz): o rádio vai para o modo MR nesse canal, com as configurações do canal.
- **Não há canal**: o rádio vai para o VFO como se a frequência tivesse sido digitada, mas com a largura de banda da escuta do Spectrum: 25k vira **W**, 12,5k vira **N** e 6,25k vira **N+**. Como N+ é a opção global **SetNFM**, ela é ligada ou desligada para bater com o Spectrum, o que vale para todos os canais Narrow. Modulação, passo e potência são os do VFO. Tons, offset e reverse são zerados. A frequência é a do pico, sem arredondar para o passo do VFO.

A largura de banda de escuta começa igual à do VFO ou canal ativo ao apertar F+5 (W = 25k, N = 12,5k, N com SetNFM = 6,25k), e o filtro é o mesmo que o rádio usa nesses modos.

---

## 18. Dual Watch e Full Watch

Configure em **Radio → RxMode** (ou com a ação **RX MODE**):

| Modo | Comportamento |
|---|---|
| **MAIN ONLY** | Só o VFO principal |
| **DUAL RX RESPOND** | Escuta A e B; responde no que recebeu |
| **CROSS BAND** | Recebe num VFO e transmite no outro |
| **MAIN TX DUAL RX** | Escuta A e B; transmite sempre no principal |
| **FULL RX RESPOND** | Escuta A, B, PriCh1 (C) e PriCh2 (D); responde no que recebeu |
| **MAIN TX FULL RX** | Escuta A, B, C e D; transmite no principal |

- No Full Watch, **F+2** percorre A → B → C → D (entradas inválidas ou repetidas são puladas). Depois de uma escolha manual, o alvo exibido vira o alvo do PTT.
- **F+3** num canal C/D alterna o modo VFO/MR desse canal prioritário sem mexer no A/B.
- Edições no menu valem para o canal exibido (inclusive C/D).
- Messenger, Search, Spectrum, CALLTX e PTT seguem o canal exibido.

---

## 19. Apps overlay

O rádio tem **48 slots** na Flash externa. Instale os `.app` pela página **Apps** do UV Studio com o rádio ligado normalmente.

### 19.1 Lista de apps

- **F + 7** abre a lista (só os slots ocupados, 4 por tela).
- **UP/DOWN** move (segure para rolar rápido), **\*** alterna entre tamanho e versão, **MENU** abre, **EXIT** sai.
- `NO APPS` = nada instalado. `APP ERROR` = falha ao abrir (qualquer tecla fecha).

### 19.2 Apps disponíveis no código

| App | Função |
|---|---|
| **FMVFO / FMMemory** | Rádio FM (seção 15) |
| **Search** | Busca de frequência e tom (seção 16) |
| **SystemInfo** | Informações do sistema, bateria, armazenamento e pilha |
| **APRS RX** | Decodifica APRS (AX.25/AFSK 1200, Mic-E) |
| **APRS TX** | Beacon de posição APRS |
| **APRS Edit** | Editor do perfil APRS (indicativo vazio = usa o `CllSgn`) |
| **APRS DIGI** | Digipeater WIDE1-1 / WIDE2-1 |
| **KISS TNC** | Usa o rádio como TNC para o APRSdroid via USB |
| **ACARS RX** | Recebe ACARS em 131,725 MHz (AM) |
| **Beacon** | Transmissor de raposa/ARDF em CW |
| **FoxHunt** | Caça de raposa por intensidade de sinal |
| **EPIRB 406** | Decodifica beacons Cospas-Sarsat de 406 MHz |
| **Sonde MTS01** | Decodifica radiossondas MTS01 |
| **SSTV** | Recebe imagens SSTV (14 modos) |
| **Spectrum3D** | Espectro em visualização 3D |
| **Jogos e demos** | Breakout, Tetris, Snake, Minesweeper, RapidRoll, SpaceImpact, Cube3D, Plasma |

> [!NOTE]
> O app **Beam** não é suportado no GOGUFW. Apps de terceiros funcionam quando os recursos que eles declaram estão disponíveis no firmware.

---

## 20. Multiboot

Permite manter até 5 firmwares no rádio: o slot **M** (0, backup do principal) e os slots **1–4**.

- **Abrir**: segure **MENU** ao ligar.
- **UP/DOWN** escolhe o slot; **MENU** pergunta `Restore …?`; **MENU** de novo grava e reinicia; **EXIT** continua o boot normal.
- A imagem é verificada por CRC antes de apagar a Flash interna.
- No primeiro uso, o firmware principal é copiado para o slot 0 (`Init Main`). **Não desligue o rádio** enquanto aparece `DO NOT POWER OFF`.

### 20.1 Bancos de configuração

- Cada slot usa o próprio banco de configurações (incluindo configurações, rascunhos e CryKey do Messenger e nomes de FM).
- **`SetCfg`** (Channels) associa o slot atual a outro banco (`CFG M`, `CFG 1`–`CFG 4`). Pede `SURE?` e reinicia.
- **Compartilhados entre slots**: calibração, logo de boot e a área de apps.

---

## 21. F Lock e menu oculto (Service)

### 21.1 Entrar no menu oculto

1. Desligue o rádio.
2. Segure **PTT + SIDE1** e ligue.
3. O menu abre direto em **All → F Lock**, com a categoria **Service** visível.

O acesso dura até o próximo boot normal.

### 21.2 Itens do Service

| Item | Opções | Padrão | Descrição |
|---|---|---|---|
| **F Lock** | BRASIL, DISABLE ALL, UNLOCK ALL | BRASIL | Plano de frequências liberadas para TX |
| **350 En** | OFF, ON | ON | Recepção na faixa de 350–400 MHz |
| **BatCal** | 1500–3500 | — | Calibração da leitura de tensão da bateria |
| **BatTyp** | 1600 mAh K5, 2200 mAh K5, 3500 mAh K5, 1400 mAh K1, 2500 mAh K1 | 1600 mAh K5 | Tipo de bateria |
| **SetNav** | LEFT RIGHT UV-K1, UP DOWN UV-K5(8) | — | Direção das setas (seção 24) |
| **Reset** | VFO, ALL | — | Restaura o rádio |

### 21.3 Planos do F Lock

| Plano | TX permitido |
|---|---|
| **BRASIL** | Só nos canais de uso geral da Anatel (Ato 14448, Tabela XII): **462,5625–462,7125 MHz** e **467,5625–467,7125 MHz** |
| **DISABLE ALL** | Nenhum TX (também religa o TXLock de todos os canais) |
| **UNLOCK ALL** | Todas as bandas suportadas. Exige **três confirmações** (aparece `READ MANUAL`) |

- Radioamadores licenciados (COER) precisam de **UNLOCK ALL** para 2 m e 70 cm.
- O firmware não limita a potência. A regra da Anatel é escrita para rádios homologados de uso geral: você é responsável por cumprir as normas.
- Planos antigos (FCC, CE, PMR446, GMRS etc.) foram removidos. Qualquer valor antigo, exceto DISABLE ALL e UNLOCK ALL, é carregado como BRASIL.

### 21.4 Reset

> [!WARNING]
> **As duas opções de Reset (VFO e ALL) apagam a memória de canais.** ALL também apaga as configurações. Faça backup com o CHIRP antes.

Depois do reset: VFO A = 145,500 MHz, VFO B = 433,500 MHz, ambos em Narrow, e `SetNFM = NARROW`. O rádio pede `SURE?`, mostra `WAIT!` e reinicia.

---

## 22. Políticas por canal e ícones

Estas políticas valem para **canais de memória** e são configuradas **só pelo CHIRP** (colunas da lista e **Properties → Extra**):

| Política | Efeito |
|---|---|
| **No FSK TX** | Bloqueia Messenger, ACK, PING e PONG neste canal. Voz e recepção FSK continuam |
| **No Roger** | Suprime o Roger/MDC neste canal |

**Ícones na linha do VFO:**

| Ícone | Significado |
|---|---|
| Ondas de rádio | TX FSK (Messenger) permitido neste canal |
| Nota musical | Roger/MDC global ativo e permitido neste canal |
| Onda dupla | Scrambler ligado |

---

## 23. Editor de texto

Usado em `ChName` (10 caracteres), `CllSgn` (6), Messenger Compose (36), nomes de FM e `CryKey` (10).

| Tecla | Função |
|---|---|
| **0** | Espaço |
| **1** | `.` `,` `?` `!` |
| **2–9** | ABC, DEF, GHI, JKL, MNO, PQRS, TUV, WXYZ (toques repetidos trocam a letra; ela é fixada após 800 ms) |
| **Segurar 0–9** | Insere o próprio dígito |
| **\*** | Modo: `ABC` → `abc` → `123` |
| **F** | Apaga o caractere antes do cursor |
| **UP / DOWN** | Move o cursor para a esquerda / direita (inserção no meio do texto) |
| **MENU** | Salva (menu) ou envia (Messenger) |
| **EXIT** | Cancela sem salvar |

---

## 24. Bloqueio de teclado e navegação UV-K1 × UV-K5

### 24.1 Bloqueio

- **Segurar F** bloqueia/desbloqueia. Também pela ação **LOCK KEYPAD** ou pelo bloqueio automático (`KeyLck`).
- Bloqueado, qualquer tecla mostra `UNLOCK KEYBOARD`; SIDE1/SIDE2 também ficam travados.
- `SetLck = KEYS` deixa o PTT livre; `KEYS+PTT` trava o PTT também.

### 24.2 SetNav

O teclado é o mesmo nos dois rádios; muda só a direção das setas, conforme `SetNav` (menu oculto):

- `LEFT RIGHT UV-K1`: valor após o Reset; inverte UP/DOWN na tela principal, nos valores do menu, no Spectrum e nos apps.
- `UP DOWN UV-K5(8)`: direção tradicional do UV-K5.

Se as setas parecerem invertidas, ajuste `SetNav`.

---

## 25. CHIRP

1. Abra o CHIRP e escolha **File → Load Module**.
2. Selecione o `Gogufw_3.6.2_chirp_module.py` (mesma versão do firmware).
3. Baixe do rádio com **Quansheng → UV-K1 / UV-K5 V3 GOGUFW Messenger**.
4. Edite canais, ações de teclas, configurações do Messenger/CALLTX, nomes de FM, `No FSK TX`, `No Roger`, `Scrambler` (incluindo HOP) e `Compander`.
5. Envie para o rádio e **espere cerca de 2 s** antes de desligá-lo (veja a seção 27.5).

**O que o módulo expõe:**

- 1024 canais de memória;
- grupo GGFW: MsgRx, MsgAck, MsgBeep, MsgLed, CllTon, CllVol, RngRsp, indicativo e os 8 rascunhos;
- por canal: TXLock, No FSK TX, No Roger, Scrambler, BusyCL, FreqRev, PTT ID, Compander e listas de scan;
- 48 nomes de FM (15 caracteres);
- SetNFM e todas as ações de teclas laterais.

> [!WARNING]
> Não use um módulo mais antigo: ele pode não reconhecer campos novos e sobrescrevê-los.

**Limitações conhecidas do módulo atual:**

- A lista de **F Lock** ainda mostra os planos antigos: qualquer escolha diferente de DISABLE ALL ou UNLOCK ALL vira BRASIL no rádio.
- A **CryKey** não é editada pelo CHIRP: use o menu `CryKey`. Uma gravação pelo CHIRP mantém a chave do rádio.
- A ação **RF LOG** ainda aparece, mas não faz nada neste build.

---

## 26. Dicas e melhores práticas

### Messenger

- **Ligue `MsgAck` nos dois rádios** para ter confirmação de entrega (`+` / `x`).
- **Defina um `CllSgn` único** em cada rádio: com todos em `UVK1`, não dá para saber quem respondeu.
- **Leia ou anote mensagens importantes antes de desligar**: Inbox e Sent não sobrevivem a desligar o rádio.
- **Use os rascunhos** para mensagens frequentes: editar e enviar um rascunho é mais rápido que digitar.
- **Confira o `TO:`** no rodapé antes de enviar: a mensagem vai para o canal exibido.
- **Evite VOX** em rádios que precisam receber mensagens: VOX ligado desativa o RX do Messenger e do HOP.
- **Use FM**: o Messenger não transmite em AM/USB.
- **Grupos grandes**: os atrasos aleatórios de ACK e PONG reduzem colisões, mas evite enviar várias mensagens seguidas sem esperar o ciclo de ACK (até cerca de 12 s).

### Range Check e AUTO CHECK

- Use **SEND PING** para conferir alcance antes de uma atividade em grupo.
- Para monitorar um grupo parado, **5 ou 10 min** no AUTO CHECK equilibram informação e consumo de bateria. 30 s ocupa muito o canal.
- Deixe `RngRsp = ON` nos rádios do grupo; desligue em rádios que não devem revelar presença.

### Criptografia e scrambler

- Combine a mesma **CryKey** em todos os rádios do grupo **antes** da atividade e confira trocando uma mensagem.
- Lembre que indicativo, ACK, PING/PONG e tensão da bateria vão **em claro**.
- **HOP**: aperte o PTT, espere cerca de meio segundo e só então fale. Em câmbios longos, espere a pausa curta a cada 8 s.
- Quem entra no meio de um câmbio HOP ouve embaralhado por até 8 s: isso é normal.
- Prefira **Narrow** com HOP e scrambler fixo: em N/N+, portadoras fixas altas (como 3200 Hz) cortam os graves da voz; o HOP já ajusta a janela à banda.

### Operação geral

- **Espere cerca de 2 s antes de desligar** depois de mudar algo: as alterações são gravadas de uma vez após uma pausa (seção 27.5).
- **Faça backup com o CHIRP** antes de atualizar, de usar `Reset` e de mexer em `SetCfg`.
- Use **Narrow (12,5 kHz)** como padrão. N+ não melhora o alcance e reduz o desvio de TX.
- Use **`ChDisp = NAME+FREQ`** para ver o nome e a frequência do canal ao mesmo tempo.
- Grave as frequências do grupo em canais de memória e use **listas de scan** (`ChList`) para separar grupos de canais.
- Use **PriCh1/PriCh2** com Full Watch para nunca perder o canal principal do grupo.
- Use **Search + SAVE** para gravar rapidamente frequência e tom de um rádio desconhecido.
- Instale `FMVFO.app` e `FMMemory.app` **juntos**; com só um deles a troca de modo fica incompleta.

---

## 27. Otimizações

### 27.1 Bateria

| Ajuste | Recomendação |
|---|---|
| `BatSav` | Mantenha 1:4 ou 1:5 em escuta longa. O WAKE do Messenger acorda rádios em economia |
| `BLTime` | 10–30 s; use **F+8** para acender sob demanda |
| `BLMax` | Reduza (5–7) em ambientes internos |
| `BLTxRx` | OFF ou só RX se não precisar da luz no TX |
| `SetOff` | Ative o modo sono (30 min–1 h) para rádios esquecidos ligados |
| `Power` | Use a menor potência que dá o alcance: LOW em curta distância, HIGH só quando necessário |
| AUTO CHECK | Intervalos longos (5–10 min) |
| `MsgLed` | OFF economiza um pouco em espera prolongada |

### 27.2 Alcance e qualidade de áudio

- **Squelch**: `Sql` baixo (1–2) ouve sinais fracos; aumente se houver ruído.
- **Narrow** em todos os rádios do grupo evita distorção e sobremodulação.
- `SetRxA`: experimente **CLEAN** ou **MID** para inteligibilidade; **BOOST/MAX** em ambiente ruidoso.
- `Mic`: ajuste com o `MicBar` ligado; o áudio não deve encostar no fim da barra.
- `STE = ON` e `RP STE` removem o ruído de cauda.
- Use o **Spectrum** para escolher um canal livre antes de uma atividade.

### 27.3 Confiabilidade do Messenger

- Mantenha **MsgRx = ON** e **VOX = OFF** em todos os rádios.
- Em canais muito movimentados, use `BusyCL` e espere o canal liberar; o Messenger adia retentativas com o canal ocupado.
- Se usar Dual/Full Watch, coloque o canal de mensagens como principal ou prioritário.
- Bateria fraca bloqueia o FSK: carregue antes de atividades longas.

### 27.4 Scan

- `SetScn = FAST` acelera a varredura (com risco de perder sinais muito curtos).
- Use listas pequenas e exclua canais ruidosos com **MENU segurado**.
- `ScnRev = CARRIER` (padrão 3,5 s) retoma assim que o sinal some; `TIMEOUT` é melhor para ouvir câmbios com pausas.

---

### 27.5 Memória Flash (desgaste)

A memória Flash do rádio suporta um número limitado de apagamentos por setor. O firmware reduz esse desgaste assim:

- **Gravação adiada**: as alterações ficam num cache e são gravadas de uma vez **cerca de 2 s após a última mudança**, com o rádio parado (fora de RX/TX, troca de mensagens ou sessão HOP), ou antes, ao trocar de área da memória, reiniciar, abrir um app ou o Spectrum. Sintonizar com UP/DOWN, digitar uma frequência, trocar canais ou mudar vários itens do menu em sequência custa um único apagamento.
- **Upload do CHIRP**: cerca de um apagamento por setor, em vez de um a cada 8 bytes.
- **Messenger e AUTO CHECK**: o número das mensagens é reservado em blocos de 64, então enviar mensagens ou PINGs (inclusive o AUTO CHECK de 30 s) quase não grava na Flash.
- **Mute**: o volume salvo é sempre o volume real, nunca o 0 do mudo.

> [!IMPORTANT]
> Depois de mudar uma configuração, **espere cerca de 2 s antes de desligar o rádio**. Se desligar antes, essa última mudança se perde, mas o restante da memória continua íntegro.

## 28. Solução de problemas

| Sintoma | Causa provável | Solução |
|---|---|---|
| `TX BLOCKED` no Messenger | Veja a tabela da seção 9.6 | Corrija o motivo exibido |
| Rádio não transmite voz | Frequência fora do F Lock BRASIL | Ajuste `F Lock` (seção 21) ou `TXLock` do canal |
| Mensagens não chegam | MsgRx OFF, VOX ON, CryKey diferente, No FSK TX, canais diferentes | Confira MsgRx/VOX/CryKey e use o mesmo canal |
| Sempre `?` em Sent | `MsgAck` OFF no remetente | Ligue `MsgAck` |
| Sempre `x` em Sent | `MsgAck` OFF no receptor, CryKey diferente ou fora de alcance | Ligue `MsgAck` no receptor, confira a CryKey, faça um SEND PING |
| Mensagens sumiram | Inbox/Sent ficam só na RAM | Comportamento normal após desligar |
| Última configuração não ficou salva | Rádio desligado menos de 2 s após a mudança | Espere cerca de 2 s antes de desligar |
| Tons/offset sumiram ao digitar uma frequência | Frequência digitada entra em simplex limpo | Configure tons e offset depois de digitar, ou use F+1 a partir de um canal |
| HOP sai embaralhado | Receptor perdeu o burst, CryKey diferente, receptor fora de HOP, VOX ON ou MsgRx OFF | Confira os requisitos da seção 12.3 e espere o próximo resync (8 s) |
| Início da fala cortado no HOP | Mic mudo durante o burst | Espere meio segundo após o PTT |
| F+0 só dá bipe duplo | Faltam `FMVFO.app` / `FMMemory.app` | Instale os dois |
| F+4 / F+\* só dão bipe duplo | Falta `Search.app` | Instale-o |
| `APP ERROR` ao abrir um app | App da API antiga ou que precisa de recurso ausente (ex.: Beam) | Instale a versão compatível |
| Setas invertidas | `SetNav` | Ajuste no menu oculto |
| Não acho o HEARD | Sem atalho de teclado | Atribua a ação HEARD a uma tecla lateral |
| Service não aparece no menu | Boot normal | Ligue segurando PTT + SIDE1 |
| Configurações "sumiram" após trocar de slot | Cada slot tem seu banco | Use `SetCfg` para escolher o banco |
| Tela escura ou clara demais | `SetCtr` / `SetInv` | Ajuste o contraste |

---

## 29. Compilar a partir do código

### 29.1 Firmware

Requer CMake, Ninja e `arm-none-eabi-gcc`:

```bash
cmake --preset Fusion --fresh
cmake --build --preset Fusion
```

Saída em `build/Fusion/`: `gogufw.bin` e a imagem multiboot `f4hwn.gogufw.v3.6.2m.bin`.

Veja também `BUILD_WITH_VSCODE.md`, `BUILD_VSCODE_MAC.md` e `compile-with-docker.sh` (build em Docker).

### 29.2 Apps

Cada app em `App/apps/<nome>/` tem um `build.sh` que gera o `.app`. Exemplos:

- `App/apps/fm` → `FMVFO.app`
- `App/apps/fmmemory` → `FMMemory.app`
- `App/apps/search` → `Search.app`

Cada app tem no máximo 4 KiB de código.

### 29.3 Espaço e otimizações de build

A Flash interna é de **120.832 bytes**, e o build atual usa **120.364 bytes** (incluindo os 920 bytes do stub do multiboot): restam **468 bytes**. Recursos novos precisam liberar espaço.

Técnicas já usadas no projeto:

- **LTO** com uma única partição.
- Flags de release `-fno-tree-ch -fno-ssa-phiopt` (cerca de −276 bytes). O `mb_flash.c` restaura essas passagens para o stub do multiboot continuar idêntico byte a byte.
- Recursos grandes movidos para **apps overlay** (FM, Search).
- Recursos desligados no Fusion para liberar Flash/RAM: K5Viewer, RF Log Lite, Audio Scope, barra de progresso do scan (`SCAN_PROGRESS`), AirCopy, NOAA, Voice, Alarm, TX1750, DTMF Calling e Resume State (retomar scan/Spectrum/FM ao ligar).
- Memória do Spectrum alocada só enquanto ele está aberto.
- Gravação adiada na Flash SPI (seção 27.5): o driver `App/driver/py25q16.c` mantém o setor alterado no cache e chama `PY25Q16_Flush()` no tick ocioso de 2 s, ao trocar de setor e antes de reset, troca de banco, apps, Spectrum e multiboot. Código novo que reinicie o rádio ou reaproveite o cache deve chamar `PY25Q16_Flush()` antes.
- Pilha mínima de **4 KiB** verificada no link. A RAM está praticamente cheia (cerca de 16.328 de 16.384 bytes), então evite novos buffers globais.

Os recursos ficam em flags `ENABLE_*` no `CMakePresets.json`. Para liberar espaço, desligue os que você não usa no preset `Fusion` (por exemplo `ENABLE_GOGUFW_CALLTX` ou `ENABLE_FEAT_F4HWN_SCAN_RSSI`) e recompile.

---

*Este firmware é fornecido como está. Você é responsável por cumprir as regras de licenciamento, frequência, potência e emissão do seu país.*
