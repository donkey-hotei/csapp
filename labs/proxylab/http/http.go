package http

/*
 * A reduced re-write of https://golang.org/src/net/http/request.go
 * for use in the CMU proxylab. Done in the spirit of understanding
 * how go's stdlib implementation of it's HTTP request parser works.
 */

import (
    "bufio"
    "fmt"
    "io"
    "net/url"
    "net/textproto"
    "strings"
)

/*
 * HttpRequest represents an HTTP request to be received
 * by a server or sent by a client.
 */
type HTTPRequest struct {
    // Method specifies the HTTP method (GET, POST, etc.)
    Method     string
    RequestURI string

    //
    URL *url.URL
    // Host specifies the host on which the URL is sought.
    Host string

    // The protocol version for incoming server requests.
    Proto      string // "HTTP/1.0"
    ProtoMajor int    // 1
    ProtoMinor int    // 0

    // Header contains the request header fields received by
    // server or sent by client. For example if the headers are:
    //
    // Host: example.com
    // accept-encoding: gzip, deflate
    // Accept-Language: en-us
    //
    // then Header = map[string][]string {
    //          "Accept-Encoding": {"gzip", "deflate"}
    //          "Accept-Language": {"en-us"}
    //      }
    Header map[string][]string
}

type parseError struct {
    what string
    str  string
}

func (e *parseError) Error() string {
    return fmt.Sprintf("%s %q", e.what, e.str)
}

/*
 * Write writes an HTTP/1.0 request, which is the header and body,
 * in wire format.
 */
func (r *HTTPRequest) Write(w io.Writer) error {
    // Request line
    _, err := fmt.Fprintf(
        w,
        "%s %s HTTP/1.0\r\n",
        r.Method,
        r.RequestURI,
    )

    if err != nil {
        return err
    }

    // Header lines
    _, err = fmt.Fprintf(w, "Host: %s\r\n", r.Host)

    if err != nil {
        return err
    }

    _, err = fmt.Fprintf(w, "User-Agent: %s\r\n", r.Header["User-Agent"])

    if err != nil {
        return err
    }

    return nil
}

/*
 * Parses "GET /index.html HTTP/1.1 into it's three parts.
 */
func parseRequestLine(line string) (method, requestURI, proto string, ok bool) {
    s1 := strings.Index(line, " ")
    s2 := strings.Index(line[s1 + 1:], " ")
    s2 += s1 + 1
    if s1 < 0 || s2 < 0 {
        return
    }
    return line[:s1], line[s1 + 1: s2], line[s2 + 1:], true
}

/*
 * Only GET and POST requests are supported.
 */
func validMethod(method string) bool {
    /*
        Method      = "GET"  ; Section 9.3
                    | "POST" ; Section 9.5
    */
    switch method {
    case "GET", "POST":
        return true
    }
    return false
}

func parseHTTPVersion(vers string) (major, minor int, ok bool) {
    vers = strings.TrimLeft(vers, " ")
    switch vers {
    case "HTTP/1.1":
        return 1, 1, true
    case "HTTP/1.0":
        return 1, 0, true
    }
    return 0, 0, false
}

/*
 * Parses an HTTP request according to RFC 1945
 * with the exception of multiline request fields.
 */
func ReadRequest(b *bufio.Reader) (req *HTTPRequest, err error) {
    // wrap our buffered I/O reader with a textproto
    // reader to provide convience methods for reading
    // requests from the proxied HTTP client
    tp := textproto.NewReader(b)
    req = new(HTTPRequest)

    // First line: GET /index.html HTML/1.1
    var s string
    s, err = tp.ReadLine()
    if err != nil {
        return nil, err
    }

    var ok bool
    req.Method, req.RequestURI, req.Proto, ok = parseRequestLine(s)
    if !ok {
        return nil, &parseError{"malformed HTTP request", s}
    }
    if !validMethod(req.Method) {
        return nil, &parseError{"unsupposeted HTTP method", req.Method}
    }
    req.ProtoMajor, req.ProtoMinor, ok = parseHTTPVersion(req.Proto)
    if !ok {
        return nil, &parseError{"invalid HTTP version", req.Proto}
    }
    req.URL, err = url.ParseRequestURI(req.RequestURI)
    if err != nil {
        return nil, err
    }
    // read subsquent header values using our textproto.Reader,
    // which can read MIME-style headers. A sequence of Key: Value
    // lines ending in a blank line.
    //
    // For example the input:
    //
    //     Key-One: Value 1
    //     Long-Key: Even Longer Value
    //     Key-Two: Value 2
    //
    // ReadMIMEHeader will return the map
    //
    //     map[string][]string{
    //         "My-Key": {"Value-1", "Value-2"},
    //         "Long-Key": {"Even longer value"}
    //     }
    mimeHeader, err := tp.ReadMIMEHeader()
    if err != nil {
        return nil, err
    }
    req.Header = mimeHeader

    // RFC 2616: Must treat
    //     GET /index.html HTTP/1.1
    //     Host: www.example.com
    // and
    //     GET http://www.example.com/index.html
    //     Host: foobar
    // the same. In the second case, any Host line is ignored.
    req.Host = req.URL.Host
    if req.Host == "" {
        req.Host = req.Header["Host"][0]
    } else {
        delete(req.Header, "Host")
    }

    return req, nil
}
