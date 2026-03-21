
```
AnimeCore
├─ include
│  ├─ common
│  │  ├─ config
│  │  │  └─ Config.h
│  │  ├─ ErrorCode.h
│  │  ├─ logger
│  │  │  └─ Logger.h
│  │  └─ thread
│  │     ├─ GlobalThreadPool.h
│  │     └─ ThreadPool.h
│  └─ network
│     ├─ asio
│     │  └─ AsioContextPool.h
│     ├─ buffer
│     │  └─ RecvBuffer.h
│     ├─ Connection.h
│     ├─ protocol
│     │  ├─ IMessage.h
│     │  ├─ MessageId.h
│     │  ├─ Packet.h
│     │  ├─ PacketParser.h
│     │  └─ ProtoMessage.h
│     └─ TcpServer.h
├─ src
│  ├─ common
│  │  ├─ config
│  │  │  └─ Config.cpp
│  │  ├─ logger
│  │  │  └─ Logger.cpp
│  │  └─ thread
│  │     ├─ GlobalThreadPool.cpp
│  │     └─ ThreadPool.cpp
│  └─ network
│     ├─ asio
│     │  └─ AsioContextPool.cpp
│     ├─ buffer
│     │  └─ RecvBuffer.cpp
│     ├─ Connection.cpp
│     ├─ protocol
│     │  ├─ Packet.cpp
│     │  └─ PacketParser.cpp
│     └─ TcpServer.cpp
└─ Structure.md

```