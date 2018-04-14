---
layout: class
generate: false
typeInfo:
    namespace: ThorsAnvil::Nisse::Core::Socket
    header:    ThorsNisseCoreSocket/SocketStream.h
    classname: SocketStreamBuffer
    parent:    std::streambuf
    description:  |
        This is a wrapper class for a <code>DataSocket</code> that allows the socket to be treated like <code>std::streambuf</code>.
        This class overrides just enough virtual functions to make the <code>ISocketStream</code> and <code>OSocketStream</code> useful.
        This class provides no public API and is designed to be used solely with the following stream objects.
    methods: []
    protected: []
    virtual: []
children: []
---
