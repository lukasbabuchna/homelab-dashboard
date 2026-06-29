from flask import Flask
import socket
import threading
import json

dataJson = dict()

def socketListener():
    s = socket.socket()
    s.bind(('', 8181))
    s.listen()

    while True:
        client, addr = s.accept()
        jsonStr = client.recv(1024).decode()

        deviceDataJson = json.loads(jsonStr)

        dataJson[deviceDataJson["hostname"]] = deviceDataJson

        client.close()




app = Flask(__name__)

@app.route('/')
def index():


    return json.dumps(dataJson)


if __name__ == '__main__':
    listenerT = threading.Thread(target=socketListener)
    listenerT.daemon = True
    listenerT.start()


    app.run(host="0.0.0.0", port=8080, debug=True, use_reloader=False)