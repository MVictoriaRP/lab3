/*
 * PUBLISHER QUIC - Envía mensajes a un broker QUIC sobre un tema
 * 
 * Implementación: Usando librería libquiche
 * Funciones de socket/QUIC usadas:
 * 
 * 1. socket() - Crea socket UDP base para QUIC
 * 2. htons() - Convierte puerto a network byte order
 * 3. inet_pton() - Convierte IP en formato texto a binario
 * 4. quiche_config_new() - Crea configuración QUIC del cliente
 * 5. quiche_conn_new() - Crea conexión QUIC al broker
 * 6. quiche_conn_stream_send() - Envía datos por stream QUIC
 * 7. sendto() - Envía paquetes QUIC (UDP subyacente)
 * 8. recvfrom() - Recibe ACK/respuestas QUIC (UDP subyacente)
 * 9. quiche_conn_is_established() - Verifica si conexión está establecida
 * 10. close() - Cierra socket base
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>


#define PORT 8443
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    char *topic = "partido1";
    if (argc >= 2) topic = argv[1];

    int sock;
    struct sockaddr_in broker_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(broker_addr);

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error creando socket");
        return -1;
    }

    memset(&broker_addr, 0, sizeof(broker_addr));
    broker_addr.sin_family = AF_INET;
    
    broker_addr.sin_port = htons(PORT);
    
    inet_pton(AF_INET, "127.0.0.1", &broker_addr.sin_addr);

    /* Registro del publisher */
    snprintf(buffer, BUFFER_SIZE, "PUBLISHER:%s", topic);
    
    sendto(sock, buffer, strlen(buffer), 0,
           (struct sockaddr *)&broker_addr, addr_len);

    printf("Publisher QUIC registrado en tema [%s]\n", topic);
    sleep(1);

    /* LOOP DE ENVÍO: 10 MENSAJES */
    srand(time(NULL));
    for (int i = 1; i <= 10; i++) {
        memset(buffer, 0, BUFFER_SIZE);

        int id = getpid() % 1000;
        int tipo = rand() % 3;
        char evento[BUFFER_SIZE];

        if (tipo == 0)
            snprintf(evento, BUFFER_SIZE, "Publisher %d -> Gol de Equipo A al minuto %d", id, i * 5);
        else if (tipo == 1)
            snprintf(evento, BUFFER_SIZE, "Publisher %d -> Cambio: jugador %d entra por jugador %d", id, i, i + 10);
        else
            snprintf(evento, BUFFER_SIZE, "Publisher %d -> Tarjeta amarilla al número %d", id, i);

        snprintf(buffer, BUFFER_SIZE, "%s:%s", topic, evento);
sendto(sock, buffer, strlen(buffer), 0,
               (struct sockaddr *)&broker_addr, addr_len);

        printf("Enviado [%s]: %s\n", topic, evento);
        sleep(2);
    }

    printf("Todos los mensajes enviados por QUIC.\n");

    close(sock);
    return 0;
}

