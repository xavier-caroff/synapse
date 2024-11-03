# Message server

Message server (`msg-server`) is a command line application that sends the content of a file to clients using the TCP protocol.

```text
Usage: msg-server { -h | -v | {options} <file> }

Help options:
  -h [ --help ]              produce help message
  --cli-format arg (=human)  select the format of the CLI output ('human' or
                             'json')

Version options:
  -v [ --version ]           display version information
  --cli-format arg (=human)  select the format of the CLI output ('human' or
                             'json')

Launch the server:
  --address arg (=0.0.0.0)   Address to listen on
  --port arg                 TCP port number
  --delay arg                Delay between to messages (in seconds, eg. 0.1)
  --file arg                 Path to the file to be send
  --block-size arg (=128)    Size of the blocks
  --loop arg (=1)            Loop when the end of file is reached
  --cli-format arg (=human)  select the format of the CLI output ('human' or
                             'json')
```

The TCP server waits for connections and send the content of the file block by block to the connected clients.

The size of the blocks and the delay between the transmisison of two blocks can be specified on the command line.

At the end of the file the application can terminate or loop on the file (default behavior).

The interface to listen on and the TCP port can be specified on the command line (when the port is set to 0 the OS asign the port number automatically).