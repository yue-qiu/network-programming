from socket import *


class Server:
    def __init__(self, host, port):
        self.host = host
        self.server_port = port
        self.server_socket = socket(AF_INET, SOCK_DGRAM)
        self.server_socket.bind((self.host, self.server_port))

    def start(self):
        while True:
            print("server is running")
            message, address = self.server_socket.recvfrom(2048)
            if message.decode() != "exit":
                print("reciveing the message from %s:" % (address, ), message.decode())
                respond_message = input("please input your respond: ")
                self.server_socket.sendto(respond_message.encode(), address)


server = Server("0.0.0.0", 9999)
server.start()
