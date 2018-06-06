package main

import (
    "./http"
    "flag"
    "io"
    "log"
    "fmt"
    "net"
    "os"
)

const max_cache_size int = 1049000 // 1 MiB
const max_object_size int = 102400 // 100 KiB

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

func Proxy(srcConn, dstConn *net.TCPConn) {
    // Channels to wait for close events on source
    // and destination connections.
    srcClosed := make(chan struct{}, 1)
    dstClosed := make(chan struct{}, 1)

    // Broker data between both connections.
    go broker(srcConn, dstConn, dstClosed)
    go broker(dstConn, srcConn, srcClosed)

    /*
     * Wait for one half of the proxy to exit first
     * before triggering shutdown of the other connection
     * via CloseRead. This basically breaks the read loop
     * in the broker and allows us to fully close the
     * connection cleanly without a "use of closed network"
     * error.
     */
    var waitFor chan struct{}
    select {
    case <-srcClosed:
        dstConn.SetLinger(0) // to recycle port faster, discard any data
        dstConn.CloseRead()
        waitFor = dstClosed
    case <-dstClosed:
        srcConn.CloseRead()
        waitFor = srcClosed
    }
    <-waitFor
}

func broker(dst, src net.Conn, chanClosed chan struct{}) {
    _, err := io.Copy(dst, src)

    if err != nil {
        log.Printf("Copy error: %s", err)
    }

    err = src.Close()

    if err != nil {
        log.Printf("Close error: %s", err)
    }

    chanClosed <- struct{}{}
}

func main() {
    log.Println("[+] Starting TCP proxy\n")

    var srcPort = flag.Int(
        "client-port",
        1234,
        "listening port number for client",
    )
    var dstPort = flag.Int(
        "server-port",
        4321,
        "listening port number for server",
    )

    flag.Parse()

    clientPort := fmt.Sprintf(":%d", *srcPort)
    serverPort := fmt.Sprintf(":%d", *dstPort)

    /* client */
    laddr, err := net.ResolveTCPAddr("tcp4", clientPort)
    checkError(err)

    clientListener, err := net.ListenTCP("tcp", laddr)
    checkError(err)

    defer clientListener.Close()

    for {
        clientConn, err := clientListener.AcceptTCP()

        defer clientConn.Close()

        /*
         * Wait until we receive a connection from the client and,
         * when we do, open a connection to the requested server to
         * proxy communcation between the two.
         */
        if err != nil {
            continue
        }

        /* server */
        raddr, err := net.ResolveTCPAddr("tcp4", serverPort)
        checkError(err)

        serverConn, err := net.DialTCP("tcp", nil, raddr)
        checkError(err)

        defer serverConn.Close()

        Proxy(clientConn, serverConn)
    }
}
