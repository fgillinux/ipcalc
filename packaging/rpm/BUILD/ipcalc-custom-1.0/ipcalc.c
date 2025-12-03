
#define _GNU_SOURCE
/*****************************************
 * ipcalc.c
 *  Calculadora de IP - Criado com auxilio de IA (Google Antigravity)
 *
 * Sintaxe: ipcalc <IP>/[<CIDR>]
 * Exemplo: ipcalc 201.28.120.50/29
 * Exemplo (auto-discovery): ipcalc 200.147.35.149
 *
 *  Criado em: 03/12/2025
 *      Autor: Fabio Gil
 *      Versão: 1.0
 *      Licença: GPLv3
 */

#include <arpa/inet.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Função para imprimir IP em decimal
void print_ip(const char *label, uint32_t ip) {
  struct in_addr addr;
  addr.s_addr = htonl(ip);
  printf("%-20s %s\n", label, inet_ntoa(addr));
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
  if (argc != 2) {
    fprintf(stderr, "Uso: %s <IP>[/CIDR]\n", argv[0]);
    fprintf(stderr, "Exemplo: %s 201.28.120.50/29\n", argv[0]);
    fprintf(stderr, "Exemplo (auto-discovery): %s 200.147.35.149\n", argv[0]);
    return 1;
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
  uint32_t mask = 0;
  if (cidr > 0) {
    mask = 0xFFFFFFFF << (32 - cidr);
  } else {
    mask = 0;
  }

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

  free(input);
  return 0;
}
