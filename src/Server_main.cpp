#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define MAX_EVENTS 4096

void send_http_response_with_file(int client_fd, const char *file_name) {
    int file_fd = open("hey.html", O_RDONLY);
    if (file_fd == -1) {
        const char* not_found_response =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 100\r\n"
            "\r\n"
            "<h1>404 Page not found. Hahahaha</h1>\n";
    send(client_fd, not_found_response, strlen(not_found_response), 0);
    return;
    }
	off_t file_size = lseek(file_fd, 0, SEEK_END); // get file size
	lseek(file_fd, 0, SEEK_SET); // reset file pointer to start

	char header[256]; //send HTTP header response
	int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %ld\r\n"
        "\r\n", file_size);
	// send (client_fd, header, header_len, 0);

	//use sendfile to send the file directly from the fd to socket
	// sendfile(client_fd, file_fd, NULL, file_size);

	close(file_fd);
}

void handle_data(int client_fd) {
    char buffer[1024] = {0};
    ssize_t bytes_received;

    // Receive data from client
    bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
    if (bytes_received <= 0) {
        // Error or connection closed
        if (bytes_received == 0) {
            // Connection closed by client
            printf("Client disconnected.\n");
        } else {
            perror("recv");
        }
        close(client_fd);
    } else {
        // Process received data
		if (strncmp(buffer, "GET /", 5) == 0) {
			send_http_response_with_file(client_fd, "hey.html");
		}
        printf("Received data from client: %.*s\n", (int)bytes_received, buffer);

        // Echo back to the client
        send(client_fd, buffer, bytes_received, 0);
    }
	close(client_fd);
}

int create_and_bind_socket(const char *port) {
    struct sockaddr_in server_addr;
    int listen_fd;

    // Create a TCP socket
    if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Enable address reuse to avoid "Address already in use" error
    int enable = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(port));

    // Bind the socket to the specified port
    if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    return listen_fd;
}





int main() {
    int epoll_fd = epoll_create(42); //could be anything but zero (it's ignored)
    if (epoll_fd == -1){;} //epoll error
    struct epoll_event event; /*The  epoll_event  structure specifies data
                                that the kernel should save and return when
                                the corresponding file descriptor becomes ready.
                                 
                                 struct epoll_event {
                                    uint32_t      events;   Epoll events 
                                    epoll_data_t  data;     User data variable
                                };*/
    struct epoll_event events[MAX_EVENTS];

    struct sockaddr_storage client_addr; /*sockaddr structure that is not
                                        associated with address family,
                                        instead it is large enough to be   
                                        keep info for a sockaddr of any type
                                        within a larger struct
                                        
                                        Socket Structure        Address Family
                                        struct sockaddr_dl      AF_LINK
                                        struct sockaddr_in      AF_INET
                                        struct sockaddr_in6     AF_INET6
                                        struct sockaddr_ll      AF_PACKET
                                        struct sockaddr_un      AF_UNIX*/

    socklen_t client_addrlen = sizeof(client_addr);

    // Set up listening socket and add to epoll
    int listen_fd = create_and_bind_socket("8080"); //created function (look up ^)
    listen(listen_fd, SOMAXCONN); /*Marks the socket as a passive
                                socket (it would only accept incoming
                                    connection requests)*/
    event.events = EPOLLIN;
    /*EPOLLIN
       The associated file is available for read(2) operations.*/
    event.data.fd = listen_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event); /*This system call is
                                    used to add, modify, or remove entries
            EPOLL_CTL_ADD
              Add an entry to the interest list of the epoll file
              descriptor, epfd.  The entry includes the file descriptor,
              fd, a reference to the corresponding open file description
              (see epoll(7) and open(2)), and the settings specified in
              event.*/

    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        /*The epoll_wait() system call waits for events on the epoll(7)
       instance referred to by the file descriptor epfd.  The buffer
       pointed to by events is used to return information from the ready
       list about file descriptors in the interest list that have some
       events available.  Up to maxevents are returned by epoll_wait().
       The maxevents argument must be greater than zero.*/
       
        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == listen_fd) {
                // Accept incoming connection
                int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr,
                    &client_addrlen);
                // Add new client socket to epoll
                event.events = EPOLLIN | EPOLLET;
                event.data.fd = client_fd;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
            } else {
                // Handle data on existing connection
                handle_data(events[i].data.fd);
            }
        }
    }

    close(epoll_fd);
    return 0;
}
