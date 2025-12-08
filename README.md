# ipcalc

**Calculadora de IP - Criado com auxilio de IA (Google Antigravity)**

`ipcalc` é uma ferramenta de linha de comando simples e eficiente para calcular detalhes de redes IPv4. Ela aceita um endereço IP e uma máscara CIDR, ou pode tentar descobrir o CIDR automaticamente usando `whois` para IPs públicos.

## Funcionalidades

*   **Cálculo de Sub-rede**: Calcula Endereço de Rede, Broadcast, Máscara de Sub-rede, Faixa de Hosts (Min/Max) e Número de Hosts.
*   **Auto-discovery de CIDR**: Se o CIDR não for fornecido, a ferramenta consulta o `whois` para tentar determinar o bloco alocado (apenas para IPs públicos).
*   **Validação de IP Privado**: Detecta e alerta sobre IPs privados (RFC 1918), exigindo CIDR explícito para esses casos.
*   **Suporte a CIDR /31 e /32**: Trata corretamente redes ponto-a-ponto (/31) e hosts únicos (/32).

## Instalação

Para compilar o projeto, basta utilizar o `make`:

```bash
make
```

Para limpar os arquivos gerados:

```bash
make clean
```

## Uso

A sintaxe básica é:

```bash
./ipcalc <IP>/[<CIDR>]
```

### Exemplos

**1. Calcular rede com CIDR específico:**

```bash
./ipcalc 200.147.35.149/17
```

_Saída:_
```text
Dados da Rede para:  200.147.35.149/17
----------------------------------------
IP Address:          200.147.35.149
Netmask:             255.255.128.0
CIDR:                /17
Network Address:     200.147.32.0
Broadcast Address:   200.147.63.255
Host Min:            200.147.32.1
Host Max:            200.147.63.254
Hosts/Net:           65534
```

**2. Auto-discovery (sem CIDR):**

```bash
./ipcalc 200.147.35.149
```
_A ferramenta irá consultar o whois para encontrar o bloco do IP._

## Licença

Este projeto está licenciado sob a licença **GPLv3**.

## Autor

*   **Fabio Gil** - *Versão 1.0 (03/12/2025)*
