package main

import (
    "fmt"
    "flag"
    "net"
    "os"
    "regexp"
    "strings"
)

const max_cache_size int = 1049000  // 1 MiB
const max_object_size int = 102400  // 100 KiB

/*
 * HttpRequest represents an HTTP request to be received
 * by a server or sent by a client.
 */
type HttpRequest struct {
    // Method specifies the HTTP method (GET, POST, etc.)
    Method  string
    Path    string

    // Host specifies the host on which the URL is sought.
    Host string

    // The protocol version for incoming server requests.
    Proto   string  // "HTTP/1.0"
    ProtoMajor int  // 1
    ProtoMinor int  // 0

    // Header contains the request header fields received by
    // server or sent by client. For example if the headers are:
    //
    // Host: example.com
    // accept-encoding: gzip, deflate
    // Accept-Language: en-us
    //
    // then Header = map[string]string {
    //          "Accept-Encoding": "gzip, deflate"
    //          "Accept-Language": "en-us"
    //      }
    Header map[string]string
}

/*
 * Parses out the HTTP version.
 */
func parseHTTPVersion(request string) (string, int, int) {
    versionRe   := regexp.MustCompile("HTTP/[\\d].[\\d]")
    protocolRe  := regexp.MustCompile("HTTP")

    httpVersion := versionRe.FindString(request)
    protocol := protocolRe.FindString(httpVersion)

    return protocol, 1, 0
}

/*
 * Parses out the requested resource path.
 */
func parseHTTPPath(request string) string {
    re := regexp.MustCompile("/[a-zA-Z]*/{0,1}")
    return re.FindString(request)
}

/*
 * Parses out HTTP method (supports only GET or POST requests).
 */
func parseHTTPMethod(request string) string {
    re := regexp.MustCompile("GET|POST|PUT|DELETE")
    return re.FindString(request)
}

/*
 * Parses out the HTTP request headers into map[string][]string
 */
func parseHTTPHeaderFields(headerLines []string) map[string]string {
    headers := make(map[string]string)

    for line := range headerLines {
        header := strings.Split(headerLines[line], ":")
        if len(header) < 2 {
            continue
        }
        headers[header[0]] = header[1]
    }

    return headers
}

/*
 * Parses out the Host field.
 */
func parseHTTPHostField(request []string) string {
    return request[1]
}

/*
 * Parses an HTTP request according to RFC 1945
 * with the exception of multiline request fields.
 */
func parseHTTPRequest(request string) HttpRequest {
    httpRequest  := HttpRequest{}
    splitRequest := strings.Split(request, "\n")

    requestLine := splitRequest[0]
    restOfRequest := splitRequest[1:]

    httpRequest.Method     = parseHTTPMethod(requestLine)
    httpRequest.Path       = parseHTTPPath(requestLine)
    protocol, major, minor := parseHTTPVersion(requestLine)
    httpRequest.Proto      = protocol
    httpRequest.ProtoMajor = major
    httpRequest.ProtoMinor = minor
    httpRequest.Host       = parseHTTPHostField(restOfRequest)
    httpRequest.Header     = parseHTTPHeaderFields(restOfRequest)

    return httpRequest
}

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

        parseHTTPRequest(string(buf[0:]))

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
