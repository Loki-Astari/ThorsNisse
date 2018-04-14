---
layout: package
generate: false
nameSpace:  ThorsAnvil::Nisse::Protocol::HTTP
headers:
    base: ThorsNisseProtocolHTTP/
    files:
        - name:   Binder.h
          functions: []
          classes:
            - name: Site
              parent:
            - name: Binder
              parent:
        - name:   DeveloperHandler.h
          functions: []
          classes:
            - name: DeveloperHandler
              parent: Core::Service::HandlerNonSuspendable
        - name:   DynamicSiteLoader.h
          functions: []
          classes:
            - name: DynamicSiteLoader
              parent: 
        - name:   HTTPProtocol.h
          functions: []
          classes:
            - name: ReadRequestHandler
              parent: Core::Service::HandlerSuspendable
        - name:   HttpScanner.h
          functions: []
          classes:
            - name: HttpParserData
              parent: 
            - name: HttpParserData
              parent: 
        - name:   Route.h
          functions: []
          classes:
            - name: Route
              parent: 
            - name: RouteTester
              parent: 
        - name:   Types.h
          functions: []
          classes:
            - name: Headers
              parent: 
            - name: URI
              parent: 
            - name: Request
              parent: 
            - name: Response
              parent: 
---

```cpp--DeepDive
class Site
{
    public:
        Site();
        Site(Site&&) noexcept;
        Site& operator=(Site&&) noexcept;
        void swap(Site&) noexcept;

        void get(std::string&& path, Action&& action)    {add(Method::Get,    std::move(path), std::move(action));}
        void put(std::string&& path, Action&& action)    {add(Method::Put,    std::move(path), std::move(action));}
        void del(std::string&& path, Action&& action)    {add(Method::Delete, std::move(path), std::move(action));}
        void post(std::string&& path, Action&& action)   {add(Method::Post,   std::move(path), std::move(action));}
        void all(std::string&& path, Action&& action)    {add(4,              std::move(path), std::move(action));}
        std::pair<bool, Action> find(Method method, std::string const& path) const;
};

class Binder
{
    public:
        Binder();
        void setCustome404Action(Action&& action);
        void addSite(std::string const& host, std::string const& base, Site&& site);
        std::pair<bool, int> remSite(std::string const& host, std::string const& base);

        Action find(Method method, std::string const& host, std::string const& path) const;
};

class DeveloperHandler: public Core::Service::HandlerNonSuspendable<Core::Socket::DataSocket>
{
    public:
        DeveloperHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, DynamicSiteLoader& loader);
        virtual short eventActivate(Core::Service::LibSocketId sockId, short eventType) override;
};

class DynamicSiteLoader
{
    public:
        DynamicSiteLoader(Core::Service::Server& server);
        std::tuple<bool, int>      load(std::string const& site, int port, std::string const& host, std::string const& base);
        std::tuple<bool, int, int> unload(int port, std::string const& host, std::string const& base);
        void                       setMaxWaitingConnections(int max);
};

class ReadRequestHandler: public Core::Service::HandlerSuspendable<Core::Socket::DataSocket>
{
    public:
        ReadRequestHandler(Core::Service::Server& parent, Core::Socket::DataSocket&& socket, Binder const& binder);
        virtual bool eventActivateNonBlocking() override;
        void setFlusher(Response* f){flusher = f;}
        void flushing()             {if (flusher){flusher->flushing();}}
};

struct HttpParserData
{
    HttpParserData();
    void addCurrentHeader();

        Headers                 headers;
        std::string             currentHead;
        std::string             currentValue;
        std::string             uri;
        char const*             bodyBegin;
        char const*             bodyEnd;
        Method                  method;
        bool                    messageComplete;
        bool                    gotValue;
};

class HttpScanner
{
    public:
        HttpParserData          data;

        HttpScanner();
        void scan(char const* buffer, std::size_t size);
};

class Route
{
    public:
        Route(std::string&& fullRoute);
        Route(Route const&)            = delete;
        Route& operator=(Route const&) = delete;
        Route(Route&&) = default;

        bool operator<(Route const& rhs) const;
        bool operator<(std::string const& rhs) const;
        bool operator<=(std::string const& rhs) const;
        bool operator==(std::string const& rhs) const;
};

struct RouteTester
{
    using is_transparent = std::true_type;

    bool operator()(std::string const& lhs, Route const& rhs) const {return !(rhs <= lhs);}
    bool operator()(Route const& lhs, std::string const& rhs) const {return lhs < rhs;}
    bool operator()(Route const& lhs, Route const& rhs)       const {return lhs < rhs;}
};

class Headers
{
    public:
        typedef ConstIterator const_iterator;
        class Inserter
        {
            ValueStore&  valueStore;
            public:
                Inserter(ValueStore& valueStore);
                void operator=(std::string&& value);
                void operator=(std::string const& value);
        };

        ConstIterator begin() const {return std::cbegin(data);}
        ConstIterator end()   const {return std::cend(data);}
        Inserter operator[](std::string const& key)    {return data[key];}

        std::size_t        getVersions(std::string const& key) const;
        std::string const& get(std::string const& key, std::size_t version = 0) const;
};

class URI
{
    public:
        std::string original;
        std::string normalized;

        std::string schema;
        std::string host;
        std::string path;
        std::string query;
        std::string fragment;
        short       port;

        Headers     queryParam;

        URI(std::string const& hostAndPort, std::string&& pathAndQuery);
};

class Request
{
    public:
        const Method            method;
        const URI               uri;
        const Headers&          headers;
        std::istream&           body;

        Request(Method method,
                URI&& uri,
                Headers& headers,
                std::istream& body);
};

class Response
{
    public:
        short                   resultCode;
        std::string             resultMessage;
        Headers                 headers;
        std::ostream&           body;

        Response(std::ostream& body);
        Response(ReadRequestHandler& flusher,
                 DataSocket& socket,
                 std::ostream& body,
                 short resultCode = 200,
                 std::string const& resultMessage = "OK");
        ~Response();
        void flushing(bool allDone = false);
};

```

An implementation of the HTTP protocol

