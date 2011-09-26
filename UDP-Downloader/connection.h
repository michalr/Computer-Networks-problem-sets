#ifndef CONNECTION_H
#define	CONNECTION_H

int create_connected_socket(const char * addr, const char * port);

void send_msg(int connected_socket, char * msg);

#endif	/* CONNECTION_H */

