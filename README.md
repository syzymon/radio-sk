# radio-sk

## Radio task
The task consists of three parts.

### Part A
Implement a radio-proxy program that allows you to play Internet radio or save the received sound to a file. After starting the program, it connects to the indicated server and starts downloading audio stream and possibly metadata about the content currently being played. The program writes the received audio data to the standard output. The received metadata program writes to the standard diagnostic output. The program ends when a signal is sent using the Ctrl-C key combination. The program should then close the connection to the server.

#### Program call:

`./radio-proxy parameters`

#### parameters:

-h host - the name of the server providing the audio stream, mandatory;

-r resource - the resource name on the server, usually the slash itself, mandatory;

-p port - port number on which the server providing the audio stream, decimal number, mandatory;

-m yes | no - yes, if the program should request metadata from the server, by default no means that the program should not request metadata from the server, optional;

-t timeout - time in seconds after which, if the server did not send anything, the intermediary recognizes that the server has stopped working, default value 5 seconds, optional.

#### Examples of use:
```bash
./radio-proxy -h waw02-03.ic.smcdn.pl -r /t050-1.mp3 -p 8000> waw.mp3
./radio-proxy -h ant-waw-01.cdn.eurozet.pl -p 8602 -r / -m yes> ant.mp3 2> meta.txt
./radio-proxy -p 8000 -h waw02-03.ic.smcdn.pl -r /t043-1.mp3 -m no | mplayer -cache 2048 -
```

### Part B
Add to the radio-proxy program the functionality of an intermediary who, instead of writing data to the standard data output and the standard diagnostic output, sends them using UDP. The broker serves only one radio, but should enable serving many clients at the same time.

#### Additional parameters:

-P port - the port number on which the program should listen for UDP messages from clients and from which it should send them, also using UDP, data, decimal number, mandatory;

-B multi - the multicast address on which the program is to listen, optional;

-T timeout - time in seconds, after which, if the client does not speak, the agent stops sending him anything, default value 5 seconds, optional.

In addition to closing the connection to the server, the program should also free the port on which it is listening before completion.

#### Examples of use:
```bash
./radio-proxy -h waw02-03.ic.smcdn.pl -r /t050-1.mp3 -p 8000 -P 10,000 -t 10
./radio-proxy -h ant-waw-01.cdn.eurozet.pl -p 8602 -r / -B 239.10.11.12 -P 54321
```
### Part C
Implement a radio-client program that uses UDP broadcasts to recognize which radio-proxy programs are running and receives data from the agent when it detects. The client sends the received audio stream to the standard output. The display of metadata is described below.

#### Program call:
`./radio-client parameters`

#### parameters:

-H host - the address on which the radio-proxy is listening or the broadcast address or the multicast address, mandatory;

-P port - UDP port number on which the radio-proxy is listening, decimal number, mandatory;

-p port - the number of TCP port to which you can connect with the telnet program to control the client, mandatory;

-T timeout - time in seconds after which, if the broker stopped sending data, the client acknowledges that the broker has stopped working, default value 5 seconds, optional.

#### Examples of use:

```bash
./radio-client -H 127.0.0.1 -P 10000 -p 20,000> abc.mp3
./radio-client -H 255.255.255.255 -P 10,000 -p 20,000 -t 10> / dev / null
./radio-client -H 239.10.11.12 -P 10000 -p 20,000 | mplayer -cache 2048
```

After starting, the client waits for the telnet program to connect to it, and after connecting it provides a menu that manages its work. The client only communicates with one intermediary at a time, but should allow the agent to be changed. The client only supports one TCP control connection at a time, but this connection can be closed and reopened many times.

#### Protocol between the broker and the internet radio server
The radio-proxy program downloads MP3 audio stream using the Shoutcast protocol, also known as ICY. Communication looks like HTTP. The client (in this case the radio-proxy) connects to the radio server via TCP and sends a GET request. The server response consists (as in HTTP) of: status line, string of header text fields and empty line. Then the server starts sending a binary audio stream possibly multiplexed with textual metadata. Here are some documents explaining what the protocol looks like:
It is also worth noting that instead of ICY 200 OK, the server may respond with HTTP / 1.0 200 OK or HTTP / 1.1 200 OK, which should be considered as correct behavior.

#### Protocol between the client and the agent
Communication between the intermediary and the client is done using UDP. The message consists of a header that has four octets and variable-length data fields. The header contains two fields successively: type and length, each occupying two octets. The type field encodes the type of message. The length field encodes the length of the data field. The numbers are stored in network byte order.

Message types, type field values:

1 - DISCOVER - the client sends this message to detect working intermediaries, there is no data field, the length field is 0;

2 - IAM - intermediary response, data contains information about the radio being served

3 - KEEPALIVE - the client sends information to the intermediary that it is still working, there is no data field, the length field is 0;

4 - AUDIO - the intermediary sends a portion of audio data;

6 - METADATA - the intermediary sends metadata.

#### Client control protocol
In the control connection the client acts as a server and the client is telnet. In the telnet program, use the up and down arrows to navigate the menu, and enter with the enter key.

After accepting the TCP connection, the client prints the initial menu:

```bash
Search for a broker
The end
```

Selecting the End option terminates the client, which of course closes the telnet connection. The program should also free all used ports then.

Choosing the Search agent option causes the client to send the DISCOVER message to the address given in the -H parameter and the port specified in the -P parameter.

If the client receives any IAM messages from intermediaries, it adds options to the menu to select one of them, for example:

```
Search for a broker
Radioactive Broker
Broker of Dobreradio
The end
```
After selecting the agent, the client sends a DISCOVER message directly to him and starts receiving AUDIO and META messages from him, and sends KEEPALIVE messages at regular intervals of three and a half seconds.

In the menu, the client marks the selected intermediary with an asterisk, and under the menu displays the status line in which the received metadata appears, for example:

```
Search for a broker
Radioactive Broker
Broker of Dobreradio *
The end
Maanam - Nothing twice
```
You can change the broker at any time, start searching for new brokers or terminate the client's operation. If the client decides that the broker has stopped sending data, he removes it from the menu.

Additional information
Programs should be as resistant as possible to any errors that may occur. They should tolerate errors that do not prevent you from continuing to work. In matters not described in the content of the task should be guided by the conventions adopted in Linux and common sense.

Programs should return code 0 when successful. An incorrect start of the program or the occurrence of an error that cannot be tolerated should result in the program terminating with code 1. If the radio-proxy program does not support the intermediary function, it should end with a code when attempting to call it as an intermediary 2.

We only require IP4 support.