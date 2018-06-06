package main

import (
    "./http"
    "fmt"
    "flag"
    "net"
    "os"
)

const max_cache_size int = 1049000  // 1 MiB
const max_object_size int = 102400  // 100 KiB

/*
 * Generic error handler which logs the error and exits the program if error exists.
 */
func checkError(err error) {
    if err != nil {
        fmt.Fprintf(os.Stderr, "Fatal error: %s", err.Error())
        os.Exit(1)
    }
}

/*
 * Handle incoming client connections.
 */
func handleClient(conn net.Conn) {
    var buf [512]byte

    for {
        nBytes, readErr := conn.Read(buf[0:])

        if readErr != nil {
            continue
        }

        http.ParseHttpRequest(string(buf[0:]))

        // Echo request back to client.
        _, writeErr := conn.Write(buf[0:nBytes])

        if writeErr != nil {
            return
        }
    }
}

func main() {
    fmt.Printf("[+] Starting TCP proxy\n")

    var srcPort = flag.Int("client-side port", 1234, "listening port number for client")
    var dstPort = flag.Int("server-side port", 4321, "listening port number for server")

    flag.Parse()

    /* client */
    clientPort := fmt.Sprintf(":%d", *srcPort)
    laddr, err := net.ResolveTCPAddr("tcp4", clientPort)
    checkError(err)

    clientListener, err := net.ListenTCP("tcp", laddr)
    checkError(err)

    defer clientListener.Close()

    /* server */
    serverPort := fmt.Sprintf(":%d", *dstPort)
    serverConnection, err := net.Dial("tcp", serverPort)
    checkError(err)

    defer serverConnection.Close()

    for {
        clientConn, err := clientListener.Accept()

        if err != nil {
            continue
        }

        handleClient(clientConn)
    }
}
