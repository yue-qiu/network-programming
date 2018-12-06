from socket import *


class Client:
    def __init__(self, host, port):
        self.host = host
        self.target_port = port
        self.client = socket(AF_INET, SOCK_STREAM)
        self.client.connect((self.host, self.target_port))

    def send(self, msg):
        self.client.send(msg)

    def receive(self):
        data = self.client.recv(2048)
        print(data.decode())

    def close(self):
        self.client.close()


client = Client("127.0.0.1", 9999)
while True:
    msg = input("please input your message: ")
    if msg != "exit":
        client.send(msg.encode())
        client.receive()
    else:
        client.close()
        break
