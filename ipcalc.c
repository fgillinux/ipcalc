
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
 *  Modificado em: 09/12/2025
 *      Autor: Fabio Gil
 *      Versão: 1.1
 *      Licença: GPLv3
 */

#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PLANNING_BLOCKS 1024

typedef enum { PLAN_NONE = 0, PLAN_HOSTS, PLAN_SUBNETS } plan_mode_t;

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
  if (argc != 2 && argc != 4) {
    fprintf(stderr, "Uso: %s <IP>[/CIDR] [--plan-hosts N | --plan-subnets N]\n",
            argv[0]);
    fprintf(stderr, "Exemplo: %s 200.147.35.149/17\n", argv[0]);
    fprintf(stderr, "Exemplo (auto-discovery): %s 200.147.35.149\n", argv[0]);
    fprintf(stderr,
            "Exemplo (planejamento): %s 192.168.0.0/24 --plan-hosts 50\n",
            argv[0]);
    return 1;
  }

  // Inicializa variáveis de planejamento
  plan_mode_t plan_mode = PLAN_NONE;
  uint64_t plan_value = 0;

  //--Verifica parametros de entrada--

  if (argc == 4) {
    if (strcmp(argv[2], "--plan-hosts") == 0) {
      plan_mode = PLAN_HOSTS;
    } else if (strcmp(argv[2], "--plan-subnets") == 0) {
      plan_mode = PLAN_SUBNETS;
    } else {
      fprintf(stderr, "Opção inválida: %s\n", argv[2]);
      return 1;
    }

    errno = 0;
    char *endptr = NULL;
    plan_value = strtoull(argv[3], &endptr, 10);

    if (errno == ERANGE) {
      perror("strtoull");
      return 1;
    }

    if (endptr == argv[3] || *endptr != '\0') {
      fprintf(stderr, "Valor numérico inválido: %s\n", argv[3]);
      return 1;
    }

    if (plan_value == 0) {
      fprintf(stderr,
              "O valor informado para planejamento deve ser maior que zero.\n");
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

  if (plan_mode == PLAN_HOSTS) {
    plan_for_hosts(network, cidr, plan_value);
  } else if (plan_mode == PLAN_SUBNETS) {
    plan_for_subnets(network, cidr, plan_value);
  }

  free(input);
  return 0;
}
