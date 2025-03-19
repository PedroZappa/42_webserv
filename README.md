# 42_WebServ

<!-- mtoc-start -->

* [Webserv TODOS](#webserv-todos)
  * [Classes to Implemented](#classes-to-implemented)
  * [Configuration File Context/Directives Implemented](#configuration-file-contextdirectives-implemented)
* [Resources](#resources)
  * [C++](#c)
  * [NGINX](#nginx)
  * [RFCs](#rfcs)
  * [Webserv](#webserv)
  * [CGI](#cgi)
  * [Tools](#tools)
  * [Study Articles](#study-articles)

<!-- mtoc-end -->
___

## Webserv TODOS

- [x] Configuration file Parser
- [x] Server setup
    - [x] Sockets
    - [x] Listen loop
- [x] Request parser
    - [x] URL decoding
    - [x] HTTP parser into a struct
    - [x] Check for common errors
- [ ] Generate a Response
    - [ ] Checks and constructs response based on method
- [ ] CGI

### Classes to Implemented

- [ ] ConfParser
- [ ] Cluster
- [ ] Server
- [ ] Location
- [ ] HTTPRequestParser
- [ ] Response
    - [ ] GET
    - [ ] POST
    - [ ] DELETE
    - [ ] ERROR
- [ ] CGI

### Configuration File Context/Directives Implemented

- [ ] server (context)
- [ ] location (context)
- [ ] listen
- [ ] server_name
- [ ] error_page
- [ ] client_max_body_size
- [ ] root
- [ ] limit_except
- [ ] index
- [ ] autoindex
- [ ] return
- [ ] upload_store
- [ ] cgi_ext

___

## Resources

### C++

- [C++ Reference](https://en.cppreference.com/w/)
- [C++ Standard Library](https://en.cppreference.com/w/cpp/header)

### NGINX

- [NGINX Reference](https://nginx.org/en/docs/)
- [NGINX Beginners Guide to Configuration Files](https://medium.com/adrixus/beginners-guide-to-nginx-configuration-files-527fcd6d5efd)
- [NGINX Configuration File Structure](https://www.digitalocean.com/community/tutorials/understanding-the-nginx-configuration-file-structure-and-configuration-contexts)
- [NGINX directives index](https://nginx.org/en/docs/dirindex.html)

### RFCs

- [RFC 2616 - Hypertext Transfer Protocol](https://datatracker.ietf.org/doc/rfc2616/)
- [RFC 6854 - HTTP/1.1](https://datatracker.ietf.org/doc/rfc6854/)

### Webserv

- [damachad's Notion](https://spicy-dirigible-2b6.notion.site/Webserver-2b27a84f4c5841dd80fb25229912e953)
- [cclaude42's webserv](https://github.com/cclaude42/webserv?tab=readme-ov-file)

### CGI

- [1 The Fast Common Gateway Interface](https://fastcgi-archives.github.io/fcgi2/doc/fastcgi-prog-guide/ch1intro.htm)
- [How the web works: HTTP and CGI explained](https://www.garshol.priv.no/download/text/http-tut.html)

### Tools

- [Nginx Config Chcker](https://www.getpagespeed.com/check-nginx-config)

### Study Articles

- [HTTP Protocol](https://www.perplexity.ai/page/hypertext-transfer-protocol-HS9LEhKQTQOQ6BAeBJgWeg)
- [Gateway Server Explained](https://www.perplexity.ai/page/gateway-server-explained-2SKRTegeTR.HuyMbFN8RGA)
- [Proxie Server Explained](https://www.perplexity.ai/page/proxies-intermediary-network-g-xzxUjZ2sRo2Bb2hb9tn.XQ)
- [Network Tunneling Explained](https://www.perplexity.ai/page/network-tunneling-explained-s67CGOFmTriHHnQeoHmAlw)
- [I/O Multiplexing](https://www.perplexity.ai/page/i-o-multiplexing-techniques-oREJI81rRkOcrpuO1M.2IQ)
