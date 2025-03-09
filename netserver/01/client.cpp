all:client tcpepoll

client: client.cpp
    g++ -g -o client client.cpp

tcpepoll
    g++ -g -o tcpepoll tcpepoll.cpp

clean:
    rm -rf client tcpepoll