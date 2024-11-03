# Python module that implement a basic TCP server that send messages to connected clients.

import socket
import time

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.bind(('0.0.0.0', 8080))
server.listen(5)

while True:
	client, addr = server.accept()
	print(f'Connected by {addr}')
	for i in range(100):
		message = f'Message {i} !\r\n'
		client.sendall(message.encode('utf-8'))
		time.sleep(1)
	client.close()
