package main

import (
    "fmt"
    "flag"
    "net"
    "os"
    "strings"
)

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

        parseHttpRequest(string(buf[0:]))

        _, writeErr := conn.Write(buf[0:nBytes])
        if writeErr != nil {
            return
        }
    }
}

/*
 *
 */
type HttpRequest struct {
}

func parseHttpVersion () {
}

/*
 * Parses an HTTP request according to RFC 1945
 * with the exception of multiline request fields.
 */
func parseHttpRequest(request string) HttpRequest {
    httpRequest := HttpRequest{}

    requestLines := strings.Split(request, "\n")
    httpRequest.Version = parseHttpVersion(requestLines[0])

    return httpRequest
}

func main() {
    fmt.Printf("[+] Starting TCP proxy\n")

    var port = flag.Int("port", 1234, "listening port number for proxy")
    flag.Parse()

    clientPort := fmt.Sprintf(":%d", *port)

    fmt.Printf("[*] Resolving TCP address localhost:%s\n", *port)
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
