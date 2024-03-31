// IMPORTANT: These are just some nodes i took while reading the 
// windows docs. This is not supposed to be a compiling program
// Just a reference for myself :)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

// Also dont forget tot link to `ws2_32.lib`

static ADDRINFOA* win32_select_addr(ADDRINFOA *addr)
{
    ADDRINFOA *selected = NULL;

    while (addr) {
        // We prefer ipv6
        if (!selected || addr->ai_family == AF_INET6) {
            selected = addr;
        }
        addr = addr->ai_next;
    }

    return selected;
}

static ADDRINFO* win32_get_addr_info(char *name, char *port)
{
    ADDRINFOA hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    if (!name) {     
        // NOTE: Dont set this when connecting to remote host
        hints.ai_flags |= AI_PASSIVE;
    }

    ADDRINFOA *result;
    if (getaddrinfo(name, port, &hints, &result)) {
        // TODO: Handle this
        assert(0);
    }

    return win32_select_addr(result);
}

static void win32_create_socket()
{
    WSADATA wsa_data;

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        platform_log("WSAStartup failed\n");
        assert(0);
    }

    if (LOBYTE(wsa_data.wVersion) != 2 || HIBYTE(wsa_data.wVersion) != 2) {
        platform_log("Version 2.2 of Winsock is not available\n");
        WSACleanup();
        assert(0);
    }

    ADDRINFOA *addr = win32_get_addr_info(NULL, "6969");
    ADDRINFOA *server = win32_get_addr_info("endrealm.net", "6969");

    i32 sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
    if (sock == -1) {
        // TODO: We failed opening a socket for some reason. Handle this
        assert(0);
    }

    // This only assigns a port. THIS DOES NOT LISTEN FOR INCOMING CONNECTIONS
    // Do we need to listen for udp "connections"?
    // No we dont.
    if (bind(sock, addr->ai_addr, addr->ai_addrlen)) {
        // TODO: We failed to bind for some reason. Handle this
        assert(0);
    }

    // Do this on the client? Automatically assings a port for us...
    // Also when using udp you can then just recv / send ( instead of recvfrom / sendto )
    // connect(sock, addr->ai_addr, addr->ai_addrlen);
    
    char *message = "Hello world this my test socket";
    u32 len = sizeof(message);
    i32 bytes_sent = send(sock, message, len, 0);
    if (bytes_sent < len) {
        // Message got sent partially. Resend it
    }
    if (bytes_sent == -1) {
        // Cant send. Handle this
    }

    char buffer[256];
    i32 bytes_read = recv(sock, buffer, sizeof(buffer), 0);
    if (bytes_read == 0) {
        // The connection is closed.
    }
    if (bytes_read < 0) {
        // Reading failed somehow
    }

    // For dgram (udp) use sendto / recvfrom instead of send / recv
    u32 addr_size = sizeof(*server);
    bytes_sent = sendto(sock, message, len, 0, (struct sockaddr*) server, addr_size);

    closesocket(sock);
    WSACleanup();
}
