# Authoritative multiplayer football/soccer server

Hello and welcome to the football server! 
This is a multiplayer football server written in C++20 using the [uWebSockets](https://github.com/uNetworking/uWebSockets) library.

Features:
- Multiplayer
- Authoritative simulation
- Inbound message queue
- Websocket hub for incoming client connections
- Game server thread for processing inbound messages
- Ingress guards
- Snapshots
- Adapter pattern for unit tests
- ...and more!

## Architecture

```mermaid
flowchart LR
    subgraph U["uWS Thread (Networking/IO)"]
        A[onMessage callbacks<br/> onDisconnect callbacks]
        G[Ingress Guards<br/>size limits<br/>queue full drop]
        Q[InBoundQueue<br/>bounded + permits<br/>disconnect reserve]
        A --> G --> Q
    end

    subgraph S["GameServer Thread (Authoritative Simulation)"]
        T[Fixed Tick Loop]
        P[Process inbound messages]
        V[Validate input messages]
        M[Advance Simulation]
        SS[Generate snapshot]
        T --> P --> V --> M --> SS
    end

    Q --> P
```
