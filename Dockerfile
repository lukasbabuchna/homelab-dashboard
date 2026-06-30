FROM ubuntu:22.04 AS builder

RUN  apt-get update && apt-get install -y cmake build-essential git && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY CMakeLists.txt main.cpp ./

RUN mkdir build && cd build && cmake .. & cmake --build .



FROM python:3.11

WORKDIR /app

RUN pip install --no-cache-dir flask

COPY --from=builder /app/build/main .

COPY app.py .

COPY templates/ ./templates/

COPY start.sh .

RUN chmod +x start.sh

CMD ["./start.sh"]