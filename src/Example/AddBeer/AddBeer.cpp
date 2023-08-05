#include "ThorsNisseExpressAddSite/AddSite.h"

namespace Express = ThorsAnvil::Nisse::Protocol::HTTP;
namespace DB     = ThorsAnvil::DB::Access;

static DB::Connection connection("mysqlNB://test.com", "test", "testPassword", "test");
static DB::Statement  listBeers(connection, "SELECT Name, Age FROM Beers");

void addSite(Express::Site& site)
{
    site.get("/listBeer", [](Express::Request& /*request*/, Express::Response& response)
        {
            response.body << "<html>"
                          << "<head><title>Beer List</title></head>"
                          << "<body>"
                          << "<h1>Beer List</h1>"
                          << "<ol>";

            listBeers.execute([&response](std::string const& name, int age)
                {
                    response.body << "<li>" << name << " : " << age << "</li>";
                }
            );

            response.body << "</ol></body></html>";
        }
    );
    site.post("/addBeer", [](Express::Request& /*request*/, Express::Response& /*response*/){std::cerr << "AddBeer\n";});
}
