# Synapse engine

Synapse engine is a command line application that runs a block layout described in a configuration file.

```json
Usage: synsapse-engine { -h | -v | {options} config }

Help options:
  -h [ --help ]              produce help message
  --cli-format arg (=human)  select the format of the CLI output ('human' or
                             'json')

Version options:
  -v [ --version ]           display version information
  --cli-format arg (=human)  select the format of the CLI output ('human' or
                             'json')

Launch the synapse application:
  --config arg               configuration filename
  --cli-format arg (=human)  select the format of the CLI output ('human' or
                             'json')
```

## Configuration file

The configuration file is a text file in the JSON format as described below:

```json
{
    // List of folder where additional modules implementing blocks (.dll or.so) can be found.
    "additionalPackageFolders": [
        "./modules"
    ],
    // Description of the blocks.
    "blocks": [
        {
            // A block name shall contains letter, digits and '-'' only.
            "name": "tcp-reader",
            // The full qualified name of the C++ class that implements the block.
            "className": "synapse::modules::io::TcpClientSource",
            // Configuration data passed to the block at initialization.
            "config": {
                "host": "127.0.0.1",
                "port": 8080,
                "retryDelay": 5
            }
        },
        {
            "name": "raw-file-logger",
            "className": "synapse::modules::io::FileLoggerSink",
            "config": {
                "folder": "C:\\TEMP\\synapse-log",
                "fileNameTemplate": "tcp-raw-{:%Y-%m-%dT%H-%M-%S}.log",
                "rotation": {
                    "strategy": "size",
                    "size": 1048576
                }
            }
        },
        {
            "name": "nmea0183-framer",
            "className": "synapse::modules::marine::Nmea0183FramerFiber",
            "config": {
                "bufferSize": 1024
            }
        },
        {
            "name": "nmea0183-file-logger",
            "className": "synapse::modules::io::FileLoggerSink",
            "config": {
                "folder": "C:\\TEMP\\synapse-log",
                "fileNameTemplate": "tcp-nmea0183-framed-{:%Y-%m-%dT%H-%M-%S}.log",
                "rotation": {
                    "strategy": "time",
                    "delay": 15
                }
            }
        }
    ],
    // Description of the routes used to convey messages.
    "routes": [
        {
            // List of blocks that forward messages on the route.
            // When a block has several output ports the name of the port
            // shall be specified behind the block's name.
            "sources": ["tcp-reader.default"],
            // List of end point blocks.
            "destinations": ["nmea0183-framer", "raw-file-logger"]
        },
        {
            "sources": ["nmea0183-framer"],
            "destinations": ["nmea0183-file-logger"]
        }
    ]
}
```
