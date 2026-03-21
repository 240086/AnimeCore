
```
AnimeCore
├─ include
│  ├─ common
│  │  ├─ config
│  │  │  └─ Config.h
│  │  ├─ ErrorCode.h
│  │  ├─ logger
│  │  │  └─ Logger.h
│  │  ├─ random
│  │  │  └─ RandomEngine.h
│  │  └─ thread
│  │     ├─ GlobalThreadPool.h
│  │     └─ ThreadPool.h
│  └─ network
│     ├─ asio
│     │  └─ AsioContextPool.h
│     ├─ buffer
│     │  └─ RecvBuffer.h
│     ├─ Connection.h
│     ├─ manager
│     │  └─ ConnectionManager.h
│     ├─ protocol
│     │  ├─ generated
│     │  ├─ IMessage.h
│     │  ├─ MessageDecoder.h
│     │  ├─ MessageId.h
│     │  ├─ MessageMacro.h
│     │  ├─ MessageRegistry.h
│     │  ├─ messages
│     │  │  └─ LoginMessage.h
│     │  ├─ Packet.h
│     │  ├─ PacketParser.h
│     │  ├─ proto
│     │  │  ├─ common.proto
│     │  │  ├─ gacha.proto
│     │  │  ├─ heartbeat.proto
│     │  │  └─ login.proto
│     │  ├─ ProtocolRegistry.h
│     │  ├─ ProtoMessage.h
│     │  └─ ResponseSender.h
│     ├─ session
│     │  ├─ Session.h
│     │  └─ SessionManager.h
│     └─ TcpServer.h
└─ src
   ├─ common
   │  ├─ config
   │  │  └─ Config.cpp
   │  ├─ logger
   │  │  └─ Logger.cpp
   │  ├─ random
   │  │  └─ RandomEngine.cpp
   │  └─ thread
   │     ├─ GlobalThreadPool.cpp
   │     └─ ThreadPool.cpp
   └─ network
      ├─ asio
      │  └─ AsioContextPool.cpp
      ├─ buffer
      │  └─ RecvBuffer.cpp
      ├─ Connection.cpp
      ├─ manager
      │  └─ ConnectionManager.cpp
      ├─ protocol
      │  ├─ Packet.cpp
      │  ├─ PacketParser.cpp
      │  └─ ProtocolRegistry.cpp
      ├─ session
      │  ├─ Session.cpp
      │  └─ SessionManager.cpp
      └─ TcpServer.cpp

```