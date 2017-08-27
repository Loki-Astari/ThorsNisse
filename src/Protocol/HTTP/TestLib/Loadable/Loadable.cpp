
namespace ThorsAnvil
{
    namespace Nisse
    {
        namespace Protocol
        {
            namespace HTTP
            {
class Site;
            }
        }
    }
}

void addSite(ThorsAnvil::Nisse::Protocol::HTTP::Site&)
{}

/*
 * The following lines are taken from Express/all.h
 * Can't include it directly as we have a chicken and egg situation.
 */
using AddSiteFunction =  void (*)(ThorsAnvil::Nisse::Protocol::HTTP::Site& binder);
extern "C" AddSiteFunction getSiteAdder()
{
    return &addSite;
}
