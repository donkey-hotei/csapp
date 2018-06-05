package http

import (
    "regexp"
    "strings"
)

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
func ParseHttpVersion(request string) (string, int, int) {
    versionRe   := regexp.MustCompile("HTTP/[\\d].[\\d]")
    protocolRe  := regexp.MustCompile("HTTP")

    httpVersion := versionRe.FindString(request)
    protocol := protocolRe.FindString(httpVersion)

    return protocol, 1, 0
}

/*
 * Parses out the requested resource path.
 */
func ParseHttpPath(request string) string {
    re := regexp.MustCompile("/[a-zA-Z]*/{0,1}")
    return re.FindString(request)
}

/*
 * Parses out HTTP method (supports only GET or POST requests).
 */
func ParseHttpMethod(request string) string {
    re := regexp.MustCompile("GET|POST|PUT|DELETE")
    return re.FindString(request)
}

/*
 * Parses out the HTTP request headers into map[string][]string
 */
func ParseHttpHeaderFields(headerLines []string) map[string]string {
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
func ParseHttpHostField(request []string) string {
    return request[1]
}

/*
 * Parses an HTTP request according to RFC 1945
 * with the exception of multiline request fields.
 */
func ParseHttpRequest(request string) HttpRequest {
    httpRequest  := HttpRequest{}
    splitRequest := strings.Split(request, "\n")

    requestLine := splitRequest[0]
    restOfRequest := splitRequest[1:]

    httpRequest.Method     = ParseHttpMethod(requestLine)
    httpRequest.Path       = ParseHttpPath(requestLine)
    protocol, major, minor := ParseHttpVersion(requestLine)
    httpRequest.Proto      = protocol
    httpRequest.ProtoMajor = major
    httpRequest.ProtoMinor = minor
    httpRequest.Host       = ParseHttpHostField(restOfRequest)
    httpRequest.Header     = ParseHttpHeaderFields(restOfRequest)

    return httpRequest
}


