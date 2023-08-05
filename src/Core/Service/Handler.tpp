#ifndef THORSANVIL_NISSE_CORE_SERVICE_HANDLER_TPP
#define THORSANVIL_NISSE_CORE_SERVICE_HANDLER_TPP

#ifndef THORSANVIL_NISSE_CORE_SERVICE_SERVICE_H
#error "Please do not include >ThorsNisseCoreService/Handler.tpp< directly. Include the >ThorsNisseCoreService/Service.h< file instead."
#endif

namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Core
        {
            namespace Service
            {

template<typename H, typename... Args>
inline void HandlerBase::addHandler(Args&&... args)
{
    /** MethodDesc:
    If the handler wants to create other handlers.
    An example of this is when the SQL handlers are added. They create connections to the SQL server that require their own event handlers.
    @ param args Arguments passed to the constructor of the new handler
    */
    parent.addHandler<H>(std::forward<Args>(args)...);
}

template<typename H, typename... Args>
inline void HandlerBase::moveHandler(Args&&... args)
{
    /** MethodDesc:
    This is similar to `addHandler()`.
    The difference is that the current handler will immediately suspend until the created handler complets.
    When the added handler calles `dropHandler()` control will be returned to the current handler at the point it suspended.

    Note: The current handler must be suspendable (otherwise an exception is thrown).
    See:  <a href=\"HandlerBase::suspendable\">HandlerBase::suspendable</a>
    @ param args Arguments passed to the constructor of the new handler
    */
    doDropHandler(false);
    parent.addHandler<H>(std::forward<Args>(args)...);
}

template<typename Stream>
inline HandlerStream<Stream>::HandlerStream(Server& parent, Stream&& stream, short eventType, double timeout)
    : HandlerBase(parent, getSocketId(stream), eventType, timeout)
    , stream(std::move(stream))
{}

template<typename Stream>
inline void HandlerStream<Stream>::close()
{
    closeStream(stream);
}

template<typename Stream>
HandlerSuspendable<Stream>::HandlerSuspendable(Server& parent, Stream&& stream, short eventType)
    : HandlerSuspendable(parent, std::move(stream), eventType, eventType)
{}

template<typename Stream>
HandlerSuspendable<Stream>::HandlerSuspendable(Server& parent, Stream&& stream, short eventType, short firstEvent)
    : HandlerStream<Stream>(parent, std::move(stream), eventType, 0)
    , yield(nullptr)
    , firstEvent(firstEvent)
{}

template<typename Stream>
inline short HandlerSuspendable<Stream>::eventActivate(LibSocketId /*sockId*/, short /*eventType*/)
{
    /** MethodDesc:
    On first call will start the method `eventActivateNonBlocking()`. This method may suspend itself by calling `suspend()`.
    On subsequent calls will resume `eventActivateNonBlocking()` at the point where `suspend()` was called.
    If `eventActivateNonBlocking()` returns true then `dropHandler()` is called to remove this handler as processing is complete.
    @ return This method returns the type of socket event that should be listend for in the main event loop.
    */
    bool dropHandler;
    if (worker == nullptr)
    {
        worker.reset(new CoRoutine([&dropHandler, &parentYield = this->yield, &parent = *this, firstEvent = this->firstEvent](Yield& yield)
            {
                parentYield = &yield;
                yield(firstEvent);
                dropHandler = parent.eventActivateNonBlocking();
                return 0;
            }));
    }
    if (!(*worker)())
    {
        if (dropHandler)
        {
            this->dropHandler();
        }
        return 0;
    }
    return worker->get();
}

            }
        }
    }
}

#endif
