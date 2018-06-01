package proxy

import (
     //  "flag"
    "fmt"
    "io"
    "log"
    "net"
)


/*
 * A simple TCP proxy that caches web objects.
 */
func Proxy(srvConn, cliConn *net.TCPConn) {
    serverClosed := make(chan struct{}, 1)
    clientClosed := make(chan struct{}, 1)

    go cacheBroker(srvConn, cliConn, clientClosed)

    var waitFor chan struct{}

    select {
        case <-clientClosed:
            srvConn.SetLinger(0)
            srvConn.CloseRead()
            waitFor = serverClosed
        case <- serverClosed:
            cliConn.CloseRead()
            waitFor = clientClosed
    }
    <-waitFor
}


func cacheBroker(dst, src net.Conn, srcClose chan struct{}) {
    _, err := io.Copy(dst, src)
    if err != nil {
        log.Printf("Copy error: %s", err)
    }

    if err := src.Close(); err != nil {

    }
}


func main() {
    client := "localhost:10042"
    server := "localhost:10043"

    clientListener, err := net.TCPListener("tcp", client)
    if err != nil {
        log.Printf("Error connecting to %s", client)
    }

    defer clientListener.Close()

    serverListener, err := net.Listen("tcp", server)
    if err != nil {
        log.Printf("Error connecting to %s", server)
    }

    defer serverListener.Close()

    for {
        clientConn, err := clientListener.Accept()
        if err != nil {
            log.Printf("Error accepting %s", err.Error())
        }
        serverConn, err := serverListener.Accept()
        if err != nil {
            log.Printf("Error accepting %s", err.Error())
        }

        go Proxy(serverConn, clientConn)
    }
}
