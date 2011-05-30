# About

Simple event based plain text chat server written in c++ in the university. Server and client are also available in java at: http://github.com/saulusg/talkr-java

# Usage

    cd talkr-server
    make
    ./talkr -p 9999

    cd talkr-client
    make
    ./talkr localhost 9999 client

# Commands

* SVERSION - returns server version.
* QUIT <msg> - disconnect from server.
* HANDSHAKE <nickas> - handshake with the server.
* PMSG <user> <msg> - sends private message <msg> to the <user>.
* CMSG <chan> <msg> - sends <msg> to the <chan>.
* JOIN <chan> - join channel <chan>.
* PART <chan> - part channel <chan>.

