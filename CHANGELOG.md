# Changelog

## 2025-12-29

### Novas Funcionalidades (v1.2 - Operações Avançadas)

- **Verificação de IP** (`--check-ip`): Verifica se um endereço IP pertence a uma rede específica.
- **Comparação de Redes** (`--compare-nets`): Identifica se duas redes se sobrepõem e se uma está contida na outra.
- **Listagem de IPs** (`--list-range`): Lista os IPs usáveis em um intervalo com limite de segurança (máx. 256 IPs).
- **Merge de Sub-redes** (`--merge`): Encontra o supernet (rede pai) que engloba múltiplas sub-redes.
- Atualização da documentação com exemplos de uso das novas operações.
- Adicionado script de testes para operações avançadas (`test_advanced_ops.sh`).

### Correções
- Correção do algoritmo de busca de supernet para encontrar o prefixo mais específico.

## 2025-12-17

- Atualização do Makefile para suporte a compilação de SRPM.

## 2025-12-14

- Atualização do Makefile para suporte a compilação de RPM e DEB.
- Adicionado arquivo de controle .gitignore.
- Atualização do README.md adicionando informações sobre instalação e compilação.
- Adicionado arquivo de licença LICENSE, GPLv3.
- Correção de formatação de saída de blocos de sub-redes.

## 2025-12-09

- Adicionado o modo de planejamento com suporte a `--plan-hosts` e `--plan-subnets`, incluindo cálculo automático do CIDR adequado.
- Implementada a listagem dos blocos disponíveis para facilitar o design de sub-redes.
- Atualizada a documentação no `README.md` com a nova sintaxe, descrição do recurso e exemplo de uso.
