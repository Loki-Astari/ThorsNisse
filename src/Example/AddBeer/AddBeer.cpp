#include <ThorsExpress/all.h>

namespace Express = ThorsAnvil::Nisse::ProtocolHTTP;
namespace SQL     = ThorsAnvil::SQL;

static SQL::Connection connection("mysqlNB://test.com", "test", "testPassword", "test");

void addSite(Express::Binder& binder)
{
    Express::Site   site;
    site.get("/listBeer", [](Express::Request& /*request*/, Express::Response& response)
        {
            response.body << "<html>"
                          << "<head><title>Beer List</title></head>"
                          << "<body>"
                          << "<h1>Beer List</h1>"
                          << "<ol>";

            SQL::Statement  listBeers(connection, "SELECT Name, Age FROM Beers");

            listBeers.execute([&response](std::string const& name, int age)
                {
                    response.body << "<li>" << name << " : " << age << "</li>";
                }
            );

            response.body << "</ol></body></html>";
        }
    );
    site.post("/addBeer", [](Express::Request& /*request*/, Express::Response& /*response*/){std::cerr << "AddBeer\n";});

    binder.addSite("test.com", std::move(site));
}
