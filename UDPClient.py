from socket import *


class Client:
    def __init__(self, host, port):
        self.host = host
        self.target_port = port
        self.client_socket = socket(AF_INET, SOCK_DGRAM)

    def send(self, message):
        self.client_socket.sendto(message, (self.host, self.target_port))

    def receive(self):
        rec_message, address = self.client_socket.recvfrom(2048)
        print(rec_message.decode())

    def close(self):
        self.client_socket.close()


client = Client("127.0.0.1", 9999)
while True:
    msg = input("please input your msg: ")
    if msg != "exit":
        client.send(msg.encode())
        client.receive()
    else:
        client.close()
        break

