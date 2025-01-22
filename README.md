# Network-Routing-Communication-with-CRC

## Description
This project simulates **network routing communication** between a **client** and a **server** through a series of **5 routers**.
The client sends binary messages to the server via the network composed of these routers. 
A **Cyclic Redundancy Check (CRC)** is used to verify the integrity of the transmitted data across each router.

The project was developed on **Ubuntu** . 
The communication involves routing through multiple devices, ensuring data integrity by appending a CRC to the transmitted messages. 
The client sends a binary message, and the server responds after verifying the CRC.

## Features
- Communication between a **client** and a **server** through **5 routers**.
- **Cyclic Redundancy Check (CRC)** for data integrity verification.
- Error handling for connection and message transmission.
- Ability to send a binary message from the client and receive a response from the server after verification.

## Prerequisites
- **Ubuntu** or a system compatible with Unix sockets.
- Compile with `gcc` or a compatible C compiler.
- If you're using **Windows**, you'll need to adapt the code to use **Winsock** (for socket programming on Windows).




