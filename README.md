# Homelab Dashboard

Used for getting basic system info mainly about my RPi 5 on user-friendly web interface.
Uses files as `/proc/meminfo` and `/sys/class/thermal/thermal_zone0/temp` to get device's temperature and memory usage.
Sends data from C++ script to Python running Flask website.

# Installation

### Clone the repo:

`git clone https://github.com/lukasbabuchna/homelab-dashboard.git`

### Create docker image:

`sudo docker build -t homelab-dashboard .`

### Run the container from the image:

`sudo docker run -d --rm --network="host" homelab-dashboard`

(`--network="host"` means that the container is using host's network directly and doesn't need mapping.)

# Setup

In `main.cpp` `main` function edit `SERVER_ADDRESS` to the address of device running the dashboard (default `127.0.0.1`).
The app shows the IP address the device has connected from, so using the loopback address won't show you the real address.

### You already have one device running the dashboard and want to add another device to monitor

In `start.sh` remove the `python3 app.py &` line to run only the C++ binary and continue with installation.
