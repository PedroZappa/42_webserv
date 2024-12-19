# 42_WebServ


<!-- mtoc-start -->

* [Webserv TODOS](#webserv-todos)
  * [Classes to Implemented](#classes-to-implemented)
  * [Configuration File Context/Directives Implemented](#configuration-file-contextdirectives-implemented)
* [Resources](#resources)
  * [C++](#c)
  * [NGINX](#nginx)
  * [Webserv](#webserv)

<!-- mtoc-end -->
___

## Webserv TODOS

- [ ] Configuration file Parser
- [ ] Server setup
    - [ ] Sockets
    - [ ] Listen loop
- [ ] Request parser
    - [ ] URL decoding
    - [ ] HTTP parser into a struct
    - [ ] Check for common errors
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

### Webserv

- [damachad's Notion](https://spicy-dirigible-2b6.notion.site/Webserver-2b27a84f4c5841dd80fb25229912e953)
- [cclaude42's webserv](https://github.com/cclaude42/webserv?tab=readme-ov-file)
