#ifndef THORSANVIL_NISSE_EXPRESS_ADDSITE_ADDSITE_H
#define THORSANVIL_NISSE_EXPRESS_ADDSITE_ADDSITE_H

#include <ThorsNisseExpressHTTP/Binder.h>
#include <ThorsNisseExpressHTTP/Types.h>
#include <ThorsDB/Connection.h>
#include <ThorsDB/Statement.h>

int addSiteVersion();
using AddSiteFunction =  void (*)(ThorsAnvil::Nisse::Protocol::HTTP::Site& binder);
void addSite(ThorsAnvil::Nisse::Protocol::HTTP::Site& binder);

extern "C" AddSiteFunction getSiteAdder()
{
    return &addSite;
}

#endif
