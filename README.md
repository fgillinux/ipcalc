# ipcalc

**Calculadora de IP**

`ipcalc` é uma ferramenta de linha de comando simples para calcular detalhes de redes IPv4. Ela aceita um endereço IP e uma máscara CIDR, ou pode tentar descobrir o CIDR automaticamente usando o comando `whois` para IPs públicos. Além disso, a ferramenta pode calcular o número mínimo de sub-redes necessárias para atender a um requisito de número de hosts, o que é útil para planejamento de redes de computadores.

## Funcionalidades

### Cálculo de Redes
*   **Cálculo de Sub-rede**: Calcula Endereço de Rede, Broadcast, Máscara de Sub-rede, Faixa de Hosts (Min/Max) e Número de Hosts.
*   **Auto-discovery de CIDR**: Se o CIDR não for fornecido, a ferramenta consulta o comando `whois` para tentar determinar o bloco alocado (apenas para IPs públicos).
*   **Validação de IP Privado**: Detecta e alerta sobre IPs privados (RFC 1918), exigindo CIDR explícito para esses casos.
*   **Suporte a CIDR /31 e /32**: Trata corretamente redes ponto-a-ponto (/31) e hosts únicos (/32).

### Planejamento
*   **Modo Planejamento**: Com as flags `--plan-hosts` ou `--plan-subnets`, encontra o CIDR mínimo que atende ao requisito e lista os blocos disponíveis dentro da rede fornecida.

### Operações Avançadas (v1.2+)
*   **Verificação de IP**: Verifica se um IP pertence a uma rede específica (`--check-ip`)
*   **Comparação de Redes**: Identifica se duas redes se sobrepõem e se uma está contida na outra (`--compare-nets`)
*   **Listagem de IPs**: Lista os IPs usáveis em um intervalo com limite de segurança (`--list-range`)
*   **Merge de Sub-redes**: Encontra o supernet (rede pai) que engloba múltiplas sub-redes (`--merge`)

## Instalação

Obtenha os pacotes de instalação em release no github: https://github.com/fgillinux/ipcalc/releases

Faça a instalação:

Debian/Ubuntu (.deb):

```bash
sudo dpkg -i ipcalc_1.x.x86_64.deb
```

Fedora (.rpm):

```bash
sudo dnf install ipcalc-1.x.x86_64.rpm
```
_Ou, se preferir, você pode compilar e instalar o projeto, seguindo as instruções de compilação descritas abaixo._

## Requisitos para Compilação (opcional)

*   **GCC**: Compilador de C
*   **Make**: Utilizado para compilação
*   **Whois**: Utilizado para auto-discovery de CIDR    

## Compilação (opcional)

Obtenha o código-fonte do projeto em release no github: https://github.com/fgillinux/ipcalc/releases

Descompacte o arquivo .tar.gz

```bash
tar -xvf ipcalc-1.x.x86_64.tar.gz
```

Entre na pasta descompactada

```bash
cd ipcalc-1.x.x86_64
```

Ou, clone o repositório:

```bash
git clone https://github.com/fgillinux/ipcalc.git
```

Entre na pasta do repositório:

```bash
cd ipcalc
```

Para compilar o projeto, basta utilizar o `make`:

```bash
make
```

Para instalar o projeto, basta utilizar o `make install`:

```bash
sudo make install
```

Para limpar os arquivos gerados (quando necessário):

```bash
make clean
```


## Uso

A sintaxe básica é:

```bash
./ipcalc <IP>/[<CIDR>] [opções]
```

### Opções Disponíveis

**Planejamento:**
- `--plan-hosts N` - Encontra o CIDR adequado para N hosts
- `--plan-subnets N` - Divide a rede em N sub-redes

**Operações Avançadas:**
- `--check-ip <IP>` - Verifica se um IP pertence à rede
- `--compare-nets <IP>/<CIDR>` - Compara duas redes para sobreposição
- `--list-range` - Lista IPs do intervalo (máx. 256)
- `--merge <IP>/<CIDR> [...]` - Encontra supernet para múltiplas redes

### Exemplos

**1. Cálculo básico de rede:**

```bash
./ipcalc 200.147.35.149/17
```

**2. Auto-discovery de CIDR:**

```bash
./ipcalc 200.147.35.149
```

**3. Planejamento de hosts:**

```bash
./ipcalc 192.168.0.0/24 --plan-hosts 50
```

**4. Verificar se um IP pertence à rede:**

```bash
./ipcalc 192.168.0.0/24 --check-ip 192.168.0.100
```

_Saída:_
```text
[Verificação de IP]
Verificando se 192.168.0.100 está em 192.168.0.0/24:
✓ SIM, 192.168.0.100 pertence à rede 192.168.0.0/24
```

**5. Comparar duas redes:**

```bash
./ipcalc 192.168.0.0/24 --compare-nets 192.168.1.0/24
```

_Saída:_
```text
[Comparação de Redes]
----------------------------------------
Rede 1: 192.168.0.0/24 (192.168.0.0 - 192.168.0.255)
Rede 2: 192.168.1.0/24 (192.168.1.0 - 192.168.1.255)
----------------------------------------
Status: NÃO SOBREPÕEM (Sem conflito)
```

**6. Listar IPs de um intervalo:**

```bash
./ipcalc 192.168.0.0/28 --list-range
```

**7. Encontrar supernet para múltiplas sub-redes:**

```bash
./ipcalc 192.168.0.0/25 --merge 192.168.0.128/25
```

_Saída:_
```text
[Merge de Sub-redes]
Processando 2 redes:
  1. 192.168.0.0/25
  2. 192.168.0.128/25

✓ Supernet encontrado:
  Rede: 192.168.0.0/24
  Intervalo: 192.168.0.0 - 192.168.0.255
```

## Licença

Este projeto está licenciado sob a licença **GPLv3**.

## CHANGELOG

Para maiores detalhes, ver arquivo [CHANGELOG.md](CHANGELOG.md)

## Autor

*   **Fábio Gil** - *Versão 1.1 (14/12/2025)*

##
*Criado com auxilio da Inteligência Artificial, usando:*
- Google Antigravity
- Codex da OpenAI
