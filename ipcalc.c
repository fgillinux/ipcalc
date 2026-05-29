
#define _GNU_SOURCE
/*****************************************
 * ipcalc.c
 *  Calculadora de IP - Criado com auxilio de IA (Google Antigravity)
 *
 * Sintaxe: ipcalc <IP>/[<CIDR>]
 * Exemplo: ipcalc 200.147.35.149/17
 * Exemplo (auto-discovery): ipcalc 200.147.35.149
 *
 *  Criado em: 03/12/2025
 *  Modificado em: 14/12/2025
 *      Autor: Fabio Gil
 *      Versão: 1.1
 *      Licença: GPLv3
 */

#define VERSION "1.2"

#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PLANNING_BLOCKS 1024
#define MAX_LIST_RANGE_IPS 256
#define MAX_SUBNETS_MERGE 16

typedef enum { 
  PLAN_NONE = 0, 
  PLAN_HOSTS, 
  PLAN_SUBNETS,
  OP_CHECK_IP,
  OP_MERGE_SUBNETS,
  OP_COMPARE_NETS,
  OP_LIST_RANGE
} operation_mode_t;

typedef struct {
  uint32_t network;
  int cidr;
} subnet_t;

// Função para imprimir IP em decimal
void print_ip(const char *label, uint32_t ip) {
  struct in_addr addr;
  addr.s_addr = htonl(ip);
  printf("%-20s %s\n", label, inet_ntoa(addr));
}

// Função para converter CIDR para máscara
uint32_t cidr_to_mask(int cidr) {
  if (cidr <= 0)
    return 0;
  if (cidr == 32)
    return 0xFFFFFFFF;
  return 0xFFFFFFFF << (32 - cidr);
}

// Função para calcular a capacidade de hosts para um prefixo
uint64_t host_capacity_for_prefix(int cidr) {
  if (cidr == 32)
    return 1;
  if (cidr == 31)
    return 2;
  uint64_t total_ips = 1ULL << (32 - cidr);
  if (total_ips < 2)
    return 0;
  return total_ips - 2;
}

// Função para encontrar o melhor CIDR para um número de hosts desejados
int best_cidr_for_hosts(uint64_t desired_hosts) {
  if (desired_hosts == 0)
    return -1;
  for (int cidr = 32; cidr >= 0; --cidr) {
    if (host_capacity_for_prefix(cidr) >= desired_hosts)
      return cidr;
  }
  return -1;
}

// Função para converter IP em string
void ip_to_string(uint32_t ip, char *buf, size_t len) {
  struct in_addr addr;
  addr.s_addr = htonl(ip);
  if (!inet_ntop(AF_INET, &addr, buf, len)) {
    snprintf(buf, len, "?");
  }
}

// Função para imprimir um bloco de planejamento
void print_planning_block(uint64_t index, uint32_t network, int cidr) {
  uint32_t mask = cidr_to_mask(cidr);
  uint32_t broadcast = network | (~mask);
  uint32_t host_min, host_max;

  if (cidr == 32) {
    host_min = network;
    host_max = network;
  } else if (cidr == 31) {
    host_min = network;
    host_max = broadcast;
  } else {
    host_min = network + 1;
    host_max = broadcast - 1;
  }

  char net_buf[INET_ADDRSTRLEN];
  char broadcast_buf[INET_ADDRSTRLEN];
  char host_min_buf[INET_ADDRSTRLEN];
  char host_max_buf[INET_ADDRSTRLEN];

  ip_to_string(network, net_buf, sizeof(net_buf));
  ip_to_string(broadcast, broadcast_buf, sizeof(broadcast_buf));
  ip_to_string(host_min, host_min_buf, sizeof(host_min_buf));
  ip_to_string(host_max, host_max_buf, sizeof(host_max_buf));

  printf("Bloco %4" PRIu64 ": %s/%d\tHosts: %s->%s\tBroadcast: %s\n", index + 1,
         net_buf, cidr, host_min_buf, host_max_buf, broadcast_buf);
}

// Função para listar blocos disponíveis
void list_available_blocks(uint32_t network, int target_cidr,
                           uint64_t total_blocks) {
  if (total_blocks == 0)
    return;

  uint64_t limit = total_blocks;
  if (limit > MAX_PLANNING_BLOCKS) {
    printf(
        "Serão exibidos apenas os primeiros %d blocos de um total de %" PRIu64
        " blocos.\n",
        MAX_PLANNING_BLOCKS, total_blocks);
    limit = MAX_PLANNING_BLOCKS;
  }

  uint64_t increment = 1ULL << (32 - target_cidr);

  for (uint64_t i = 0; i < limit; ++i) {
    uint64_t start = (uint64_t)network + (i * increment);
    print_planning_block(i, (uint32_t)start, target_cidr);
  }

  if (limit < total_blocks) {
    printf("... (mais %" PRIu64 " blocos não exibidos)\n",
           total_blocks - limit);
  }
}

// Função para planejar blocos de hosts
void plan_for_hosts(uint32_t network, int base_cidr, uint64_t desired_hosts) {
  uint64_t base_capacity = host_capacity_for_prefix(base_cidr);
  if (desired_hosts > base_capacity) {
    printf("\n[Planejamento - Hosts]\n");
    printf("Hosts desejados (%" PRIu64 ") excedem a capacidade do bloco atual "
           "(/%d).\n",
           desired_hosts, base_cidr);
    return;
  }

  int target_cidr = best_cidr_for_hosts(desired_hosts);
  if (target_cidr == -1) {
    printf("\n[Planejamento - Hosts]\n");
    printf("Não foi possível encontrar um CIDR adequado para %" PRIu64
           " hosts.\n",
           desired_hosts);
    return;
  }
  if (target_cidr < base_cidr) {
    target_cidr = base_cidr;
  }

  uint64_t hosts_per_subnet = host_capacity_for_prefix(target_cidr);
  uint64_t total_blocks = 1ULL << (target_cidr - base_cidr);

  printf("\n[Planejamento - Hosts]\n");
  printf("Hosts desejados: %" PRIu64 "\n", desired_hosts);
  printf("CIDR recomendado: /%d (%" PRIu64 " hosts utilizáveis por bloco)\n",
         target_cidr, hosts_per_subnet);
  printf("Blocos disponíveis dentro de /%d: %" PRIu64 "\n", base_cidr,
         total_blocks);

  list_available_blocks(network, target_cidr, total_blocks);
}

// Função para planejar blocos de subredes
void plan_for_subnets(uint32_t network, int base_cidr,
                      uint64_t desired_subnets) {
  if (desired_subnets == 0) {
    printf("\n[Planejamento - Subredes]\n");
    printf("Número de subredes deve ser maior que zero.\n");
    return;
  }

  int target_cidr = base_cidr;
  uint64_t available = 1;

  while (available < desired_subnets && target_cidr < 32) {
    target_cidr++;
    available <<= 1;
  }

  if (available < desired_subnets) {
    printf("\n[Planejamento - Subredes]\n");
    printf("Não é possível dividir /%d em %" PRIu64 " subredes.\n", base_cidr,
           desired_subnets);
    return;
  }

  uint64_t total_blocks = available;
  uint64_t hosts_per_subnet = host_capacity_for_prefix(target_cidr);

  printf("\n[Planejamento - Subredes]\n");
  printf("Subredes desejadas: %" PRIu64 "\n", desired_subnets);
  printf("CIDR por subrede: /%d (%" PRIu64 " hosts utilizáveis por bloco)\n",
         target_cidr, hosts_per_subnet);
  printf("Total de blocos possíveis dentro de /%d: %" PRIu64 "\n", base_cidr,
         total_blocks);

  list_available_blocks(network, target_cidr, total_blocks);
}

// Verifica se o IP é privado (RFC 1918)
int is_private_ip(uint32_t ip) {
  // 10.0.0.0/8
  if ((ip & 0xFF000000) == 0x0A000000)
    return 1;
  // 172.16.0.0/12
  if ((ip & 0xFFF00000) == 0xAC100000)
    return 1;
  // 192.168.0.0/16
  if ((ip & 0xFFFF0000) == 0xC0A80000)
    return 1;
  return 0;
}

// ===== OPERAÇÕES AVANÇADAS DE REDE =====

// Verifica se um IP está contido em uma rede
int is_ip_in_network(uint32_t ip, uint32_t network, int cidr) {
  uint32_t mask = cidr_to_mask(cidr);
  return (ip & mask) == network;
}

// Lista IPs dentro de um intervalo (com limite de segurança)
void list_ip_range(uint32_t start, uint32_t end, int limit) {
  uint32_t count = end - start + 1;
  
  if (count > limit) {
    printf("[Aviso] Intervalo contém %" PRIu32 " IPs. Exibindo apenas os primeiros %d.\n", 
           count, limit);
    end = start + limit - 1;
    count = limit;
  }
  
  char ip_buf[INET_ADDRSTRLEN];
  printf("\nListagem de IPs do intervalo:\n");
  printf("Total de IPs: %" PRIu32 "\n", count);
  printf("Primeiros/Últimos IPs:\n");
  
  for (uint32_t i = 0; i < count && i < 5; i++) {
    ip_to_string(start + i, ip_buf, sizeof(ip_buf));
    printf("  %s\n", ip_buf);
  }
  
  if (count > 10) {
    printf("  ...\n");
  }
  
  for (uint32_t i = (count > 10 ? count - 5 : count - 1); i < count && i > 4; i--) {
    ip_to_string(start + i, ip_buf, sizeof(ip_buf));
    printf("  %s\n", ip_buf);
  }
}

// Verifica se duas redes se sobrepõem
int networks_overlap(uint32_t net1, int cidr1, uint32_t net2, int cidr2) {
  uint32_t mask1 = cidr_to_mask(cidr1);
  uint32_t mask2 = cidr_to_mask(cidr2);
  
  return (net1 & mask2) == (net2 & mask2) || (net2 & mask1) == (net1 & mask1);
}

// Compara duas redes
void compare_networks(uint32_t net1, int cidr1, uint32_t net2, int cidr2) {
  uint32_t mask1 = cidr_to_mask(cidr1);
  uint32_t mask2 = cidr_to_mask(cidr2);
  uint32_t broadcast1 = net1 | (~mask1);
  uint32_t broadcast2 = net2 | (~mask2);
  
  char net1_buf[INET_ADDRSTRLEN], net2_buf[INET_ADDRSTRLEN];
  char bc1_buf[INET_ADDRSTRLEN], bc2_buf[INET_ADDRSTRLEN];
  
  ip_to_string(net1, net1_buf, sizeof(net1_buf));
  ip_to_string(net2, net2_buf, sizeof(net2_buf));
  ip_to_string(broadcast1, bc1_buf, sizeof(bc1_buf));
  ip_to_string(broadcast2, bc2_buf, sizeof(bc2_buf));
  
  printf("\n[Comparação de Redes]\n");
  printf("----------------------------------------\n");
  printf("Rede 1: %s/%d (%s - %s)\n", net1_buf, cidr1, net1_buf, bc1_buf);
  printf("Rede 2: %s/%d (%s - %s)\n", net2_buf, cidr2, net2_buf, bc2_buf);
  printf("----------------------------------------\n");
  
  if (networks_overlap(net1, cidr1, net2, cidr2)) {
    printf("Status: SOBREPÕEM (Há conflito entre as redes)\n");
    
    if (cidr1 > cidr2) {
      printf("Rede 1 está contida em Rede 2\n");
    } else if (cidr2 > cidr1) {
      printf("Rede 2 está contida em Rede 1\n");
    } else {
      printf("Redes são parcialmente sobrepostas\n");
    }
  } else {
    printf("Status: NÃO SOBREPÕEM (Sem conflito)\n");
  }
}

// Encontra o supernet para um conjunto de sub-redes
int find_supernet(subnet_t *subnets, int count, uint32_t *super_net, int *super_cidr) {
  if (count < 2)
    return -1;
  
  // Encontra o menor e maior IP em todos os blocos
  uint32_t min_ip = subnets[0].network;
  uint32_t max_ip = subnets[0].network;
  
  uint32_t mask = cidr_to_mask(subnets[0].cidr);
  max_ip |= (~mask);
  
  for (int i = 1; i < count; i++) {
    uint32_t net = subnets[i].network;
    mask = cidr_to_mask(subnets[i].cidr);
    uint32_t bc = net | (~mask);
    
    if (net < min_ip)
      min_ip = net;
    if (bc > max_ip)
      max_ip = bc;
  }
  
  // Encontra o CIDR do supernet (começa do mais específico até encontrar o que cobre tudo)
  for (int cidr = 31; cidr >= 0; cidr--) {
    mask = cidr_to_mask(cidr);
    uint32_t net = min_ip & mask;
    uint32_t bc = net | (~mask);
    
    if (net <= min_ip && bc >= max_ip) {
      *super_net = net;
      *super_cidr = cidr;
      return 0;
    }
  }
  
  return -1;
}

// Busca CIDR via whois
int get_cidr_from_whois(const char *ip_str) {
  char command[256];
  snprintf(command, sizeof(command), "whois %s", ip_str);

  FILE *fp = popen(command, "r");
  if (fp == NULL) {
    perror("popen");
    return -1;
  }

  char line[512];
  int cidr = -1;

  while (fgets(line, sizeof(line), fp) != NULL) {
    // Procura por "inetnum:" ou "CIDR:" ou "route:" que contenha uma barra "/"
    if (strcasestr(line, "inetnum:") || strcasestr(line, "CIDR:") ||
        strcasestr(line, "route:")) {
      char *slash = strchr(line, '/');
      if (slash) {
        // Verifica se o que vem depois é um número
        if (sscanf(slash + 1, "%d", &cidr) == 1) {
          // Encontrou um CIDR válido
          break;
        }
      }
    }
  }

  pclose(fp);
  return cidr;
}

// Função principal
int main(int argc, char *argv[]) {
  if (argc == 2 && strcmp(argv[1], "--version") == 0) {
    printf("ipcalc version %s\n", VERSION);
    printf("Copyleft (c) 2025 Fabio Gil\n");
    printf("License GPLv3: GNU GPL version 3\n");
    printf(
        "This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
    return 0;
  }

  if (argc < 2) {
    fprintf(
        stderr,
        "Uso: %s <IP>[/CIDR] [opções]\n"
        "\nOpciones de Planejamento:\n"
        "  --plan-hosts N          Planejamento de hosts\n"
        "  --plan-subnets N        Planejamento de subredes\n"
        "\nOperações Avançadas:\n"
        "  --check-ip <IP>         Verifica se IP pertence à rede\n"
        "  --compare-nets <IP2>/<CIDR2>  Compara duas redes\n"
        "  --list-range            Lista IPs do intervalo (max %d IPs)\n"
        "  --merge <IP2>/<CIDR2> [<IP3>/<CIDR3> ...]\n"
        "                          Encontra supernet de múltiplas sub-redes\n"
        "\nExemplos:\n"
        "  %s 200.147.35.149/17\n"
        "  %s 192.168.0.0/24 --plan-hosts 50\n"
        "  %s 192.168.0.0/24 --check-ip 192.168.0.50\n"
        "  %s 192.168.0.0/24 --compare-nets 192.168.1.0/24\n"
        "  %s 192.168.0.0/24 --list-range\n"
        "  %s 192.168.0.0/25 --merge 192.168.0.128/25\n",
        argv[0], MAX_LIST_RANGE_IPS, argv[0], argv[0], argv[0], argv[0], argv[0], argv[0]);
    return 1;
  }

  // Inicializa variáveis de operação
  operation_mode_t op_mode = PLAN_NONE;
  uint64_t plan_value = 0;
  char *op_arg1 = NULL;
  subnet_t merge_subnets[MAX_SUBNETS_MERGE];
  int merge_count = 0;

  //--Verifica parametros de entrada--

  if (argc >= 3) {
    if (strcmp(argv[2], "--plan-hosts") == 0) {
      if (argc < 4) {
        fprintf(stderr, "Erro: --plan-hosts requer um valor numérico\n");
        return 1;
      }
      op_mode = PLAN_HOSTS;
      errno = 0;
      char *endptr = NULL;
      plan_value = strtoull(argv[3], &endptr, 10);

      if (errno == ERANGE || endptr == argv[3] || *endptr != '\0') {
        fprintf(stderr, "Valor numérico inválido: %s\n", argv[3]);
        return 1;
      }

      if (plan_value == 0) {
        fprintf(stderr,
                "O valor informado para planejamento deve ser maior que zero.\n");
        return 1;
      }
    } else if (strcmp(argv[2], "--plan-subnets") == 0) {
      if (argc < 4) {
        fprintf(stderr, "Erro: --plan-subnets requer um valor numérico\n");
        return 1;
      }
      op_mode = PLAN_SUBNETS;
      errno = 0;
      char *endptr = NULL;
      plan_value = strtoull(argv[3], &endptr, 10);

      if (errno == ERANGE || endptr == argv[3] || *endptr != '\0') {
        fprintf(stderr, "Valor numérico inválido: %s\n", argv[3]);
        return 1;
      }

      if (plan_value == 0) {
        fprintf(stderr,
                "O valor informado para planejamento deve ser maior que zero.\n");
        return 1;
      }
    } else if (strcmp(argv[2], "--check-ip") == 0) {
      if (argc < 4) {
        fprintf(stderr, "Erro: --check-ip requer um endereço IP\n");
        return 1;
      }
      op_mode = OP_CHECK_IP;
      op_arg1 = argv[3];
    } else if (strcmp(argv[2], "--compare-nets") == 0) {
      if (argc < 4) {
        fprintf(stderr, "Erro: --compare-nets requer uma rede (IP/CIDR)\n");
        return 1;
      }
      op_mode = OP_COMPARE_NETS;
      op_arg1 = argv[3];
    } else if (strcmp(argv[2], "--list-range") == 0) {
      op_mode = OP_LIST_RANGE;
    } else if (strcmp(argv[2], "--merge") == 0) {
      if (argc < 4) {
        fprintf(stderr, "Erro: --merge requer ao menos uma segunda rede (IP/CIDR)\n");
        return 1;
      }
      op_mode = OP_MERGE_SUBNETS;
      
      // Processa todas as redes para merge
      for (int i = 3; i < argc && merge_count < MAX_SUBNETS_MERGE; i++) {
        char *subnet_str = strdup(argv[i]);
        char *slash = strchr(subnet_str, '/');
        
        if (!slash) {
          fprintf(stderr, "Erro: Rede inválida (use IP/CIDR): %s\n", argv[i]);
          free(subnet_str);
          return 1;
        }
        
        *slash = '\0';
        int cidr = atoi(slash + 1);
        
        if (cidr < 0 || cidr > 32) {
          fprintf(stderr, "CIDR inválido: %s (deve ser entre 0 e 32)\n", slash + 1);
          free(subnet_str);
          return 1;
        }
        
        struct in_addr addr;
        if (inet_aton(subnet_str, &addr) == 0) {
          fprintf(stderr, "IP inválido: %s\n", subnet_str);
          free(subnet_str);
          return 1;
        }
        
        uint32_t ip = ntohl(addr.s_addr);
        uint32_t mask = cidr_to_mask(cidr);
        uint32_t network = ip & mask;
        
        merge_subnets[merge_count].network = network;
        merge_subnets[merge_count].cidr = cidr;
        merge_count++;
        
        free(subnet_str);
      }
    } else {
      fprintf(stderr, "Opção inválida: %s\n", argv[2]);
      return 1;
    }
  }

  char *input = strdup(argv[1]);
  if (!input) {
    perror("strdup");
    return 1;
  }

  char *ip_str = input;
  char *cidr_str = NULL;
  int cidr = -1;

  char *slash = strchr(input, '/');
  if (slash) {
    *slash = '\0';
    cidr_str = slash + 1;
    cidr = atoi(cidr_str);
    if (cidr < 0 || cidr > 32) {
      fprintf(stderr, "CIDR invalido: %s (deve ser entre 0 e 32)\n", cidr_str);
      free(input);
      return 1;
    }
  }

  // Analisa IP
  struct in_addr ip_addr;
  if (inet_aton(ip_str, &ip_addr) == 0) {
    fprintf(stderr, "IP invalido: %s\n", ip_str);
    free(input);
    return 1;
  }
  uint32_t ip = ntohl(ip_addr.s_addr);

  // Se CIDR não foi fornecido
  if (cidr == -1) {
    if (is_private_ip(ip)) {
      fprintf(
          stderr,
          "IP Privado detectado (%s). CIDR é obrigatório para IPs privados.\n",
          ip_str);
      free(input);
      return 1;
    }

    printf("CIDR não fornecido. Buscando informações via whois...\n");
    cidr = get_cidr_from_whois(ip_str);

    if (cidr == -1) {
      fprintf(stderr, "Não foi possível determinar o CIDR via whois.\n");
      free(input);
      return 1;
    }
    printf("CIDR encontrado: /%d\n\n", cidr);
  }

  // Calcula Máscara de Sub-rede
  uint32_t mask = cidr_to_mask(cidr);

  // Calcula detalhes da rede
  uint32_t network = ip & mask;
  uint32_t broadcast = network | (~mask);

  uint32_t host_min, host_max;

  if (cidr == 32) {
    host_min = network;
    host_max = network;
  } else if (cidr == 31) {
    host_min = network;
    host_max = broadcast;
  } else {
    host_min = network + 1;
    host_max = broadcast - 1;
  }

  // Saída
  printf("Dados da Rede para:  %s/%d\n", ip_str, cidr);
  printf("----------------------------------------\n");
  print_ip("IP Address:", ip);
  print_ip("Netmask:", mask);
  printf("%-20s /%d\n", "CIDR:", cidr);
  print_ip("Network Address:", network);
  print_ip("Broadcast Address:", broadcast);
  print_ip("Host Min:", host_min);
  print_ip("Host Max:", host_max);

  // Calcular número de hosts
  uint64_t num_hosts;
  if (cidr == 32)
    num_hosts = 1;
  else if (cidr == 31)
    num_hosts = 2;
  else
    num_hosts = (uint64_t)broadcast - network - 1;

  printf("%-20s %" PRIu64 "\n", "Hosts/Net:", num_hosts);

  // Processa operações avançadas
  if (op_mode == PLAN_HOSTS) {
    plan_for_hosts(network, cidr, plan_value);
  } else if (op_mode == PLAN_SUBNETS) {
    plan_for_subnets(network, cidr, plan_value);
  } else if (op_mode == OP_CHECK_IP) {
    struct in_addr check_addr;
    if (inet_aton(op_arg1, &check_addr) == 0) {
      fprintf(stderr, "IP inválido para check: %s\n", op_arg1);
      free(input);
      return 1;
    }
    
    uint32_t check_ip = ntohl(check_addr.s_addr);
    char check_buf[INET_ADDRSTRLEN];
    ip_to_string(check_ip, check_buf, sizeof(check_buf));
    
    printf("\n[Verificação de IP]\n");
    printf("Verificando se %s está em %s/%d:\n", check_buf, ip_str, cidr);
    
    if (is_ip_in_network(check_ip, network, cidr)) {
      printf("✓ SIM, %s pertence à rede %s/%d\n", check_buf, ip_str, cidr);
    } else {
      printf("✗ NÃO, %s NÃO pertence à rede %s/%d\n", check_buf, ip_str, cidr);
    }
  } else if (op_mode == OP_COMPARE_NETS) {
    char *net2_str = strdup(op_arg1);
    char *slash = strchr(net2_str, '/');
    
    if (!slash) {
      fprintf(stderr, "Erro: Rede inválida (use IP/CIDR)\n");
      free(net2_str);
      free(input);
      return 1;
    }
    
    *slash = '\0';
    int cidr2 = atoi(slash + 1);
    
    if (cidr2 < 0 || cidr2 > 32) {
      fprintf(stderr, "CIDR inválido: %s\n", slash + 1);
      free(net2_str);
      free(input);
      return 1;
    }
    
    struct in_addr addr2;
    if (inet_aton(net2_str, &addr2) == 0) {
      fprintf(stderr, "IP inválido: %s\n", net2_str);
      free(net2_str);
      free(input);
      return 1;
    }
    
    uint32_t ip2 = ntohl(addr2.s_addr);
    uint32_t mask2 = cidr_to_mask(cidr2);
    uint32_t network2 = ip2 & mask2;
    
    compare_networks(network, cidr, network2, cidr2);
    
    free(net2_str);
  } else if (op_mode == OP_LIST_RANGE) {
    uint32_t host_start, host_end;
    
    if (cidr == 32) {
      host_start = network;
      host_end = network;
    } else if (cidr == 31) {
      host_start = network;
      host_end = broadcast;
    } else {
      host_start = network + 1;
      host_end = broadcast - 1;
    }
    
    list_ip_range(host_start, host_end, MAX_LIST_RANGE_IPS);
  } else if (op_mode == OP_MERGE_SUBNETS) {
    // Adiciona a primeira rede (do argumento principal) ao array de merge
    if (merge_count < MAX_SUBNETS_MERGE) {
      subnet_t first_subnet = {network, cidr};
      // Desloca os outros
      for (int i = merge_count; i > 0; i--) {
        merge_subnets[i] = merge_subnets[i - 1];
      }
      merge_subnets[0] = first_subnet;
      merge_count++;
    }
    
    if (merge_count < 2) {
      printf("[Erro] --merge requer ao menos 2 sub-redes\n");
      free(input);
      return 1;
    }
    
    printf("\n[Merge de Sub-redes]\n");
    printf("Processando %d redes:\n", merge_count);
    
    for (int i = 0; i < merge_count; i++) {
      char net_buf[INET_ADDRSTRLEN];
      ip_to_string(merge_subnets[i].network, net_buf, sizeof(net_buf));
      printf("  %d. %s/%d\n", i + 1, net_buf, merge_subnets[i].cidr);
    }
    
    uint32_t super_net;
    int super_cidr;
    
    if (find_supernet(merge_subnets, merge_count, &super_net, &super_cidr) == 0) {
      char super_buf[INET_ADDRSTRLEN];
      uint32_t super_mask = cidr_to_mask(super_cidr);
      uint32_t super_bc = super_net | (~super_mask);
      char super_bc_buf[INET_ADDRSTRLEN];
      
      ip_to_string(super_net, super_buf, sizeof(super_buf));
      ip_to_string(super_bc, super_bc_buf, sizeof(super_bc_buf));
      
      printf("\n✓ Supernet encontrado:\n");
      printf("  Rede: %s/%d\n", super_buf, super_cidr);
      printf("  Intervalo: %s - %s\n", super_buf, super_bc_buf);
    } else {
      printf("\n✗ Não foi possível encontrar um supernet comum\n");
    }
  }

  free(input);
  return 0;
}
