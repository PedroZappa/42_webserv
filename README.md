# 42_WebServ


<!-- mtoc-start -->

* [Webserv TODOS](#webserv-todos)
  * [Classes to Implemented](#classes-to-implemented)
  * [Configuration File Context/Directives Implemented](#configuration-file-contextdirectives-implemented)

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

