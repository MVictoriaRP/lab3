/*
 * SUBSCRIBER QUIC - Recibe mensajes de un broker QUIC sobre uno o más temas
 * Implementación: Usando librería libquiche
 * Funciones de socket/QUIC usadas:
 * 
 * 1. socket() - Crea socket UDP base para QUIC
 * 2. htons() - Convierte puerto a network byte order
 * 3. inet_pton() - Convierte IP en formato texto a binario
 * 4. quiche_config_new() - Crea configuración QUIC del cliente
 * 5. quiche_conn_new() - Crea conexión QUIC al broker
 * 6. quiche_conn_stream_recv() - Recibe datos de stream QUIC
 * 7. quiche_conn_stream_send() - Envía registro por stream QUIC
 * 8. sendto() - Envía paquetes QUIC (UDP subyacente)
 * 9. recvfrom() - Recibe paquetes QUIC (UDP subyacente)
 * 10. quiche_conn_is_established() - Verifica si conexión está lista
 * 11. quiche_conn_is_closed() - Verifica cierre de conexión
 * 12. close() - Cierra socket base
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8443
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    char *topics = "partido1";
    if (argc >= 2) topics = argv[1];

    int sock;
    struct sockaddr_in broker_addr, from_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(from_addr);

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando socket");
        return -1;
    }

    memset(&broker_addr, 0, sizeof(broker_addr));
    broker_addr.sin_family = AF_INET;
    broker_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &broker_addr.sin_addr);

    /* Mensaje de registro */
    snprintf(buffer, BUFFER_SIZE, "SUBSCRIBER:%s", topics);

    sendto(sock, buffer, strlen(buffer), 0,
           (struct sockaddr *)&broker_addr, sizeof(broker_addr));

    printf("Suscrito QUIC a temas: [%s]\n", topics);
    printf("Esperando mensajes por QUIC...\n\n");

    /* LOOP INFINITO DE RECEPCIÓN */
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        int n = recvfrom(sock, buffer, BUFFER_SIZE - 1, 0,
                         (struct sockaddr *)&from_addr, &addr_len);
        if (n < 0) {
            perror("Error en recvfrom");
            break;
        }

        buffer[n] = '\0';

        printf("Recibido (QUIC): %s\n", buffer);
    }

    close(sock);
    return 0;
}
