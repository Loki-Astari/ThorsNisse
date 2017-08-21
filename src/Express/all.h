#ifndef THORSANVIL_NISSE_EXPRESS_ALL_H
#define THORSANVIL_NISSE_EXPRESS_ALL_H

#include <ThorsNisseProtocolHTTP/Binder.h>
#include <ThorsNisseProtocolHTTP/Types.h>
#include <ThorSQL/Connection.h>
#include <ThorSQL/Statement.h>

using AddSiteFunction =  void (*)(ThorsAnvil::Nisse::Protocol::HTTP::BinderProxy& binder);
void addSite(ThorsAnvil::Nisse::Protocol::HTTP::BinderProxy& binder);

extern "C" AddSiteFunction getSiteAdder()
{
    return &addSite;
}
#endif
