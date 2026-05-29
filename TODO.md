# TODO - Funcionalidades Futuras do ipcalc

## Status de Implementação

- ✅ **CONCLUÍDO**: Operações Avançadas de Rede (v1.2)
  - `--check-ip`: Verificação de IP em rede
  - `--compare-nets`: Comparação de redes
  - `--list-range`: Listagem de IPs
  - `--merge`: Merge de sub-redes

---

## 🎯 Roadmap de Desenvolvimento

### Curto Prazo (v1.3-1.4)

#### 1. Suporte a Múltiplos Formatos de Saída ⭐
- **Prioridade**: Alta
- **Descrição**: Adicionar suporte para JSON, CSV e formatos tabulares melhorados
- **Tarefas**:
  - [ ] Implementar flag `--json` para saída JSON
  - [ ] Implementar flag `--csv` para saída CSV
  - [ ] Adicionar cores ANSI para melhor visualização
  - [ ] Suporte a exportação em arquivo (`--output` ou `> file.json`)
- **Benefício**: Facilita integração com scripts e ferramentas de automação

#### 2. Testes Unitários Automatizados
- **Prioridade**: Alta
- **Descrição**: Build comprehensive test suite usando framework C
- **Tarefas**:
  - [ ] Escolher framework de testes (cmocka, minunit, etc)
  - [ ] Criar suite de testes para todas as operações
  - [ ] Adicionar testes de edge cases
  - [ ] Integrar com CI/CD (GitHub Actions)
  - [ ] Coverage > 80%
- **Benefício**: Aumenta confiabilidade e facilita manutenção futura

#### 3. Modo Batch/Entrada de Arquivo
- **Prioridade**: Média
- **Descrição**: Processar múltiplas redes de um arquivo
- **Tarefas**:
  - [ ] Flag `--input-file <arquivo>` para ler lista de IPs
  - [ ] Flag `--batch` para processar múltiplos comandos
  - [ ] Formato: `IP/CIDR [operação] [argumento]` por linha
  - [ ] Saída consolidada com opção de formato
- **Benefício**: Processamento em lote para redes corporativas

---

### Médio Prazo (v2.0)

#### 4. Suporte a IPv6 ⭐⭐⭐
- **Prioridade**: Muito Alta
- **Descrição**: Estender funcionalidades para IPv6
- **Tarefas**:
  - [ ] Parsear endereços IPv6 (notação expanded e compressed)
  - [ ] Cálculos de sub-redes IPv6
  - [ ] Conversão IPv4-to-IPv6 (IPv4-mapped IPv6)
  - [ ] Suporte a auto-discovery CIDR para IPv6
  - [ ] Detectar endereços reservados IPv6 (link-local, multicast, etc)
  - [ ] Merge e comparação de redes IPv6
- **Benefício**: Essencial para redes modernas

#### 5. Operações Avançadas de Agregação de Redes
- **Prioridade**: Alta
- **Descrição**: Merge automático e otimização de blocos
- **Tarefas**:
  - [ ] `--aggregate`: Agregar múltiplas redes em supernet único
  - [ ] `--optimize`: Encontrar alocação otimizada de blocos
  - [ ] `--split`: Dividir rede em subnets ótimas (ao invés de planejamento fixo)
  - [ ] Suporte a exclusões (e.g., agregar EXCETO um bloco)
- **Benefício**: Planejamento de redes mais eficiente

#### 6. Validação e Segurança Aprimorada
- **Prioridade**: Média
- **Descrição**: Detectar padrões de segurança/conformidade
- **Tarefas**:
  - [ ] `--validate-rfc`: Validar conformidade com RFCs de redes especiais
  - [ ] `--detect-reserved`: Alertar sobre blocos reservados
  - [ ] Suporte a listas negras comuns (e.g., Spamhaus)
  - [ ] `--check-multicast`: Detectar endereços multicast (224.0.0.0/4)
  - [ ] `--check-experimental`: Alertar sobre ranges experimentais
- **Benefício**: Evitar erros de configuração críticos

#### 7. Integração com WHOIS Melhorada
- **Prioridade**: Média
- **Descrição**: Cache e informações mais ricas
- **Tarefas**:
  - [ ] Implementar cache local de queries WHOIS
  - [ ] Extrair info de ASN, país, provedor
  - [ ] `--whois-info`: Exibir informações completas do bloco
  - [ ] Histórico de alocações (via API pública)
  - [ ] Timeout configurável para queries
- **Benefício**: Informações de geolocalização e provedor

---

### Longo Prazo (v2.1+)

#### 8. API REST / Web Interface
- **Prioridade**: Média
- **Descrição**: Servidor HTTP para consultas remotas
- **Tarefas**:
  - [ ] Implementar servidor HTTP simples (`--server` ou `--web-ui`)
  - [ ] Endpoints REST para cada operação
  - [ ] Documentação OpenAPI/Swagger
  - [ ] Dashboard web (HTML/CSS/JS)
  - [ ] Autenticação básica (opcional)
- **Benefício**: Integração em dashboards corporativos

#### 9. Planejamento Multi-Camada Avançado
- **Prioridade**: Baixa
- **Descrição**: Planejamento hierárquico de redes
- **Tarefas**:
  - [ ] `--hierarchy`: Planejar core/distribution/access layers
  - [ ] `--visualize`: Árvore ASCII de hierarquia de sub-redes
  - [ ] Salvar/carregar planos em arquivo (JSON)
  - [ ] Sugestões de VLAN allocation
  - [ ] Cálculo de redundância (HSRP/VRRP)
- **Benefício**: Planejamento de redes empresariais complexas

#### 10. Integração com Ferramentas Populares
- **Prioridade**: Baixa
- **Descrição**: Plugins e integração com infraestrutura
- **Tarefas**:
  - [ ] Plugin Terraform para gerenciamento de redes
  - [ ] Integração com Ansible (módulo)
  - [ ] Docker image pronto para usar
  - [ ] Pacote no Homebrew (macOS)
  - [ ] Pacotes no APT (Debian/Ubuntu) e YUM (RHEL/Fedora)
  - [ ] Integração com Netbox (plugin)
- **Benefício**: Facilita adoção em ambientes DevOps

#### 11. Performance e Escalabilidade
- **Prioridade**: Baixa (se necessário)
- **Descrição**: Otimizar para operações em larga escala
- **Tarefas**:
  - [ ] Benchmark e profiling
  - [ ] Compilação com otimizações avançadas
  - [ ] Suporte a processamento paralelo (threads)
  - [ ] Otimização de memória para redes muito grandes
  - [ ] Benchmarks vs ferramentas similares
- **Benefício**: Processar grandes datasets

#### 12. Documentação e Comunidade
- **Prioridade**: Média
- **Descrição**: Melhorar documentação e suporte
- **Tarefas**:
  - [ ] Man page melhorado (`man ipcalc`)
  - [ ] Guia de uso para iniciantes (tutorial)
  - [ ] Exemplos adicionais no README
  - [ ] Wiki no GitHub
  - [ ] FAQ com casos de uso comuns
  - [ ] Tradução para outros idiomas (EN, ES, FR, etc)
- **Benefício**: Comunidade engajada e documentação de qualidade

---

## 📋 Legenda de Prioridade

| Símbolo | Descrição |
|---------|-----------|
| ⭐⭐⭐ | Muito Alta - Implementar primeiro |
| ⭐⭐ | Alta - Importante para usabilidade |
| ⭐ | Média - Nice-to-have |
| (vazio) | Baixa - Futuro distante |

---

## 🚀 Recomendação de Ordem de Implementação

### Fase 1 (v1.3)
1. ✅ Operações Avançadas de Rede (CONCLUÍDO)
2. Múltiplos Formatos de Saída (JSON/CSV)
3. Testes Unitários Automatizados

### Fase 2 (v1.4-1.5)
4. Modo Batch/Entrada de Arquivo
5. Validação e Segurança Aprimorada
6. WHOIS Melhorado com Cache

### Fase 3 (v2.0)
7. ⭐⭐⭐ **Suporte a IPv6** (PRIORIDADE MÁXIMA)
8. Operações Avançadas de Agregação
9. Testes de performance

### Fase 4 (v2.1+)
10. API REST / Web Interface
11. Planejamento Multi-Camada
12. Integração com Ferramentas (Terraform, Ansible, Docker)

---

## 📞 Considerações

- Manter compatibilidade com versões anteriores
- Código bem documentado e testado
- Contribuições da comunidade bem-vindas
- Seguir padrões GNU para ferramentas de linha de comando

---

**Última atualização**: 29 de Maio de 2026
**Versão atual**: 1.2 (Operações Avançadas de Rede)
