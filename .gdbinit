### GDB Settings
set prompt (zdb) 
set editing on
# set verbose on
set tui border-kind acs
set tui active-border-mode bold-standout
set tui border-mode reverse
set trace-commands on
set logging enabled on
set follow-fork-mode child
# set detach-on-fork off

show follow-fork-mode

# Enable C++ virtual table debugging
set print vtbl on

set print pretty on
# set print elements 2
set print object on
set print static-members on
set print array on
set print array-indexes on

# https://sourceware.org/elfutils/Debuginfod.html
# set debuginfod enabled on
# set debuginfod urls "https://debuginfod.ubuntu.com/"

### 000_main.cpp
define main
  display argc
  display *argv@argc
end

### ConfParser.cpp

define loadConf
  display _confFile
end

define tokenizer
  display line
  display tks
  display ss
  display val
end

### Server.cpp
define setDirective
  display directive
  display tks
  display it
end

define setListen
  display tks
  display socket
  display val
  display sep
end


### Go GDB Go! I Choose YOU! 

# STOP @ main
# break main
# run "conf/default.conf"
# main
# fs cmd
# rfr

# STOP @ loadConf
# break loadConf
# run "conf/default.conf"
# loadConf
# fs cmd
# rfr

# Stop @ getServer
# break getServers
# run "conf/default.conf"
# fs cmd
# rfr

# Stop @ setDirective
break setDirective
run "conf/default.conf"
fs cmd
rfr

