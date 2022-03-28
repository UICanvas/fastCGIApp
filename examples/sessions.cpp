//! See https://isatec.ca/fastcgipp/sessions.html
//! [Start]
#include <fastcgi++/request.hpp>

class Sessions: public Fastcgipp::Request<char>
{
    //! [Start]
    //! [session Declaration]
    static Fastcgipp::Http::Sessions<std::string> s_sessions;
    std::shared_ptr<const std::string> m_session;
    Fastcgipp::Http::SessionId m_sid;
    //! [session Declaration]

    //! [Response]
    bool response()
    {
        using Fastcgipp::Encoding;
        const auto command = environment().gets.find("cmd");
        const auto sessionCookie = environment().cookies.find("sid");
        //! [Response]
        //! [With cookie]
        if(sessionCookie != environment().cookies.cend())
        {
            m_sid = sessionCookie->second;
            m_session = s_sessions.get(m_sid);
            if(m_session)
            {
                //! [With cookie]
                //! [logout]
                if(command!=environment().gets.cend()
                        && command->second=="logout")
                {
                    out << "Set-Cookie: sid=deleted; path=/; expires=Thu, "
                        "01-Jan-1970 00:00:00 GMT\n";
                    s_sessions.erase(m_sid);
                    handleNoSession();
                }
                //! [logout]
                //! [normal session]
                else
                {
                    out << "Set-Cookie: sid=" << Encoding::URL
                        << sessionCookie->second << Encoding::NONE
                        << "; path=/; expires=" << s_sessions.expiration() << "\n";
                    handleSession();
                }
                //! [normal session]

                //! [finish session]
                return true;
            }
        }
        //! [finish session]

        //! [login]
        if(command!=environment().gets.cend() && command->second=="login")
        {
            std::shared_ptr<std::string> session(new std::string);
            const auto sessionData = environment().posts.find("data");
            if(sessionData == environment().posts.cend())
                *session = "WTF we weren't given session data!!!";
            else
                *session = std::move(sessionData->second);

            m_session = session;
            m_sid = s_sessions.generate(m_session);

            out << "Set-Cookie: sid=" << Encoding::URL << m_sid
                << Encoding::NONE << "; path=/; expires=" << s_sessions.expiration()
                << "\n";
            handleSession();
        }
        //! [login]
        //! [normal sessionless]
        else
            handleNoSession();

        return true;
    }
    //! [normal sessionless]

    //! [output]
    void header()
    {
        out <<
"Content-Type: text/html; charset=ISO-8859-1\r\n\r\n"
"<!DOCTYPE html>\n"
"<html lang='en'>"
    "<head>"
        "<meta charset='ISO-8859-1' />"
        "<title>fastcgi++: Sessions</title>"
    "</head>"
    "<body>";
    }

    void footer()
    {
        out <<
        "<p>There are " << s_sessions.size() << " sessions open</p>"
    "</body>"
"</html>";
    }

    void handleSession()
    {
        using Fastcgipp::Encoding;
        header();
        out <<
        "<p>We are currently in a session. The session id is "
            << m_sid << " and the session data is \"" << Encoding::HTML
            << *m_session << Encoding::NONE << "\"."
        "<p>Click <a href='?cmd=logout'>here</a> to logout</p>";
        footer();
    }

    void handleNoSession()
    {
        header();
        out <<
        "<p>We are currently not in a session.</p>"
        "<form action='?cmd=login' method='post' "
                "enctype='application/x-www-form-urlencoded' "
                "accept-charset='ISO-8859-1'>"
            "<div>"
                "Text: <input type='text' name='data' value='Hola se" "\xf1" "or, usted "
                    "me almacen" "\xf3" " en una sesi" "\xf3" "n' />"
                "<input type='submit' name='submit' value='submit' />"
            "</div>"
        "</form>";
        footer();
    }
public:
    //! [output]
    //! [construct]
    Sessions():
        Fastcgipp::Request<char>(256)
    {}
};
//! [construct]

//! [session Definition]
Fastcgipp::Http::Sessions<std::string> Sessions::s_sessions(3600);
//! [session Definition]

//! [manager]
#include <fastcgi++/manager.hpp>

int main()
{
    Fastcgipp::Manager<Sessions> manager;
    manager.setupSignals();
    manager.listen();
    manager.start();
    manager.join();

    return 0;
}
//! [manager]
