#ifndef COMMON_VARIABLES_H
#define COMMON_VARIABLES_H

//according to Required Port Number Allocation
#define LOCALHOST "127.0.0.1"
#define USC_ID_SUFFIX 684

#define MAX_PENDING_CLIENTS 20

// UDP ports for backend servers
#define PORT_SERVER_A (41000 + USC_ID_SUFFIX)
#define PORT_SERVER_P (42000 + USC_ID_SUFFIX)
#define PORT_SERVER_Q (43000 + USC_ID_SUFFIX)
#define PORT_SERVER_M_UDP (44000 + USC_ID_SUFFIX)

// TCP port for Server M <-> clients
#define PORT_SERVER_M_TCP (45000 + USC_ID_SUFFIX)

// Common message strings
#define MSG_AUTH_GRANTED "GRANTED"
#define MSG_AUTH_DENIED  "DENIED"
#define MSG_PASSWORD_MASK "******"

#endif
