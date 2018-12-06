from socket import *
import threading
import time


class Server:
    def __init__(self, host, port):
        self.host = host
        self.server_port = port
        self.server_socket = socket(AF_INET, SOCK_STREAM)
        self.server_socket.bind((self.host, self.server_port))
        self.server_socket.listen(5)

    def start(self):
        while True:
            print("server is running")
            # 创建 TCP 套接字，所有 （源IP， 源port， 目的IP， 目的port）相同的 TCP 报文会被送到同一个 socket
            sock, addr = self.server_socket.accept()
            t = threading.Thread(target=Server.response, args=(sock, addr))
            t.start()


    @staticmethod
    def response(sock, addr):
        print("reciveing the message from %s: " % (addr, ), end="")
        while True:
            data = sock.recv(2048)
            print(data.decode())
            time.sleep(1)
            if data.decode() != '' and data.decode("utf-8") != "exit":
                msg = input("please input your response: ")
                sock.send(msg.encode())
            else:
                break
        sock.close()
        print("connection from %s has closed" % (addr, ))


server = Server("0.0.0.0", 9999)
server.start()
