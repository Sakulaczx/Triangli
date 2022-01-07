from socket import *
class udp(object):
    """wifi udp to esp32"""
    def __init__(self,HOST = '192.168.4.1',PORT = 2333):

        self.HOST = HOST
        self.PORT =PORT
        self.BUFSIZ = 1024
        self.ADDRESS = (self.HOST, self.PORT)

        self.udpClientSocket = socket(AF_INET, SOCK_DGRAM)

        try:
            s = socket(AF_INET, SOCK_DGRAM)
            s.connect(('8.8.8.8', 80))
            self.user_ip = s.getsockname()[0]
        finally:
            s.close()
    def send_message(self,data):
        if not data:
            return 0
        self.udpClientSocket.sendto(data.encode('utf-8'),self.ADDRESS)

if __name__ == '__main__':
    udp = udp()
    while True:
        data = input('>')
        udp.send_message(data)
        # 接收数据
        data, ADDR = udp.udpClientSocket.recvfrom(udp.BUFSIZ)
        if not data:
            break
        print("服务器端响应：", data.decode('utf-8'))

    udp.udpClientSocket.close()