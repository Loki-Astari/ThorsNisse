#ifndef THORSANVIL_NISSE_EXPRESS_ALL_H
#define THORSANVIL_NISSE_EXPRESS_ALL_H

#include <ThorsNisseProtocolHTTP/Binder.h>
#include <ThorsNisseProtocolHTTP/Types.h>
#include <ThorsDB/Connection.h>
#include <ThorsDB/Statement.h>

using AddSiteFunction =  void (*)(ThorsAnvil::Nisse::Protocol::HTTP::Site& binder);
void addSite(ThorsAnvil::Nisse::Protocol::HTTP::Site& binder);

extern "C" AddSiteFunction getSiteAdder()
{
    return &addSite;
}
#endif
