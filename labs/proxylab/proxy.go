package main

import (
    "fmt"
    "flag"
    "./http"
    "net"
    "os"
)

const max_cache_size int = 1049000  // 1 MiB
const max_object_size int = 102400  // 100 KiB

/*
 * Generic error handler which exits the program if error exists.
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
        // TODO: Forward request to server connection.
        _, writeErr := conn.Write(buf[0:nBytes])
        if writeErr != nil {
            return
        }
    }
}

func main() {
    fmt.Printf("[+] Starting TCP proxy\n")

    var port = flag.Int("port", 1234, "listening port number for proxy")
    flag.Parse()

    clientPort := fmt.Sprintf(":%d", *port)

    fmt.Printf("[*] Resolving TCP address localhost:%d\n", *port)
    laddr, err := net.ResolveTCPAddr("tcp4", clientPort)
    checkError(err)

    fmt.Printf("[*] Proxy listening for client @ localhost:%d\n", *port)
    clientListener, err := net.ListenTCP("tcp", laddr)
    checkError(err)

    for {
        clientConn, err := clientListener.Accept()
        if err != nil {
            continue
        }
        fmt.Printf("[*] Accepted Client Connection\n")
        handleClient(clientConn)
    }
}
