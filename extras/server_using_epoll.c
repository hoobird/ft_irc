#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static const int PORT = 8080;
static const int LISTEN_BACKLOG = 42;
#define MAX_EVENTS 512

static char client_buffers[MAX_EVENTS][2048]; // Simple per-client buffer
static size_t client_buffer_lens[MAX_EVENTS] = {0};

int main()
{
    // 1. Create a Listening Socket (Non-Blocking)
    // create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket failed");
        return 1;
    }

    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt failed");
        return 1;
    }

    // Make socket non-blocking
    int flags = fcntl(server_fd, F_GETFL, 0);
    fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);

    // Bind and listen
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)};

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("bind failed");
        return 1;
    }

    if (listen(server_fd, LISTEN_BACKLOG) < 0)
    {
        perror("listen failed");
        return 1;
    }

    printf("Server listening on port 8080...\n");

    // 2. Create an Epoll Instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd < 0)
    {
        perror("epoll_create1 failed");
        return 1;
    }

    // 3. Add the Listening Socket to Epoll
    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.data.fd = server_fd;
    event.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) < 0)
    {
        perror("epoll_ctl failed");
        return 1;
    }

    // 4. Event Loop
    struct epoll_event events[MAX_EVENTS];
    while (1)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0)
        {
            perror("epoll_wait failed");
            return 1;
        }

        for (int i = 0; i < nfds; i++)
        {
            if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN)))
            {
                fprintf(stderr, "epoll error on fd %d\n", events[i].data.fd);
                close(events[i].data.fd);
                continue;
            }
            if (events[i].data.fd == server_fd)
            {
                // Accept new connections
                while (1)
                {
                    struct sockaddr_in client_addr;
                    socklen_t client_len = sizeof(client_addr);
                    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
                    if (client_fd < 0)
                    {
                        if (errno == EAGAIN || errno == EWOULDBLOCK)
                        {
                            // we processed all of the incoming new connections
                            break;
                        }
                        perror("accept failed");
                        continue;
                    }

                    // Make the client socket non-blocking
                    flags = fcntl(client_fd, F_GETFL, 0);
                    fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);

                    // Add the client socket to epoll
                    event.data.fd = client_fd;
                    event.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) < 0)
                    {
                        perror("epoll_ctl for client failed");
                        close(client_fd);
                        continue;
                    }

                    printf("Accepted new connection: %d\n", client_fd);
                }
            }
            else
            {
                // Handle client socket events
                int client_fd = events[i].data.fd;
                char buffer[1024];
                ssize_t bytes_read = recv(client_fd, buffer, sizeof(buffer), 0);
                if (bytes_read > 0)
                {
                    // Append to client buffer
                    size_t idx = client_fd; // For demo, use fd as index (not robust for real use)
                    memcpy(client_buffers[idx] + client_buffer_lens[idx], buffer, bytes_read);
                    client_buffer_lens[idx] += bytes_read;

                    // Process complete lines
                    char *start = client_buffers[idx];
                    char *newline;
                    while ((newline = memchr(start, '\n', client_buffer_lens[idx] - (start - client_buffers[idx]))) != NULL)
                    {
                        size_t line_len = newline - start + 1;
                        printf("Message Received: %.*s", (int)line_len, start);
                        send(client_fd, "Successfully Received Message\n", 31, 0);
                        start = newline + 1;
                    }
                    // Move remaining data to front of buffer
                    size_t remaining = client_buffer_lens[idx] - (start - client_buffers[idx]);
                    memmove(client_buffers[idx], start, remaining);
                    client_buffer_lens[idx] = remaining;
                }
                else if (bytes_read == 0)
                {
                    // Client disconnected
                    printf("Client disconnected: %d\n", client_fd);
                    close(client_fd);
                    continue;
                }
                else
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        printf("finished reading data from client\n");
                        continue;
                    }
                    else
                    {
                        perror("read()");
                        return 1;
                    }
                }
            }
        }
    }

    // Cleanup (not reached in this example)
    close(server_fd);
    close(epoll_fd);
    printf("Server shutting down...\n");
    return 0;
}
