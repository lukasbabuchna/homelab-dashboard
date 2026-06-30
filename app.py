from flask import Flask, render_template
import socket
import threading
import json
from turbo_flask import Turbo


app = Flask(__name__)
turbo = Turbo(app)

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
        with app.app_context():

            turbo.push(turbo.replace(render_template('load.html', dataJson=dataJson), 'load'))





@app.route('/')
def index():


    return render_template("index.html", dataJson=dataJson)


if __name__ == '__main__':
    listenerT = threading.Thread(target=socketListener)
    listenerT.daemon = True
    listenerT.start()


    app.run(host="0.0.0.0", port=8080, debug=True, use_reloader=False)