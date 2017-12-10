class LoginView : public View {
    public:
        inline string templ() {
            return
                "<!doctype html>"
                "<html>"
                    "<head>"
                        "<title>Вход</title>"
                        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=0.8, maximum-scale=1, user-scalable=no\">"
                        "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
                        "<link rel=\"icon\" href=\"/favicon.ico\" type=\"image/x-icon\">"
                        "<link type=\"text/css\" rel=\"StyleSheet\" href=\"/templates/" + theme + "/css/style_l.css\">"
                    "</head>"
                    "<body>"
                        "<a href=\"/\" class=\"a l\">Главная</a>"
                        "<a href=\"/reg\" class=\"a r\">Еще не зарегистрированы? Зарегистрироваться</a>"
                        "<div class=\"wrap\">"
                            "<form method=\"post\" action=\"/login\" class=\"form\">"
                                "<h1 class=\"header\">Вход в DigInterest</h1>"
                                "<div class=\"content\">"
                                    "<div class=\"wri\">"
                                        "<input type=\"text\" name=\"login\" size=\"15\" maxlength=\"15\" placeholder=\"Имя или E-mail\">"
                                    "</div>"
                                    "<div class=\"wri\">"
                                        "<input type=\"password\" name=\"pass\" size=\"15\" maxlength=\"15\" placeholder=\"Пароль\">"
                                    "</div>"
                                "</div>"
                                "<input type=\"submit\" value=\"Войти\">"
                                "<label class=\"r\">Чужой компьютер"
                                    "<input type=\"checkbox\" name=\"foreign\" value=\"1\">"
                                "</label>"
                            "</form>"
                        "</div>"
                    "</body>"
                "</html>";
        };
};

class LoginModel : public Model {
    public:
        void slashing(string& str) {
            int g = 0;
            int l = str.size();

            while(g < l)
            {
                if(str[g] == '\'')
                {
                    str.replace(g, 1, "&#39;");
                    l = str.size();
                    g += 4;
                }
                if(str[g] == '\"')
                {
                    str.replace(g, 1, "&#34;");
                    l = str.size();
                    g += 4;
                }

                if(str[g] == '\n')
                {

                    str.replace(g, 1, "\\\\n");
                    l = str.size();
                    g += 2;
                }


                if(str[g] == '\r')
                {
                    str.replace(g, 1, "&#92;r");
                    l = str.size();
                    g += 5;
                }



                if(str[g] == '\a')
                {
                    str.replace(g, 1, "&#92;a");
                    l = str.size();
                    g += 5;
                }


                if(str[g] == '\t')
                {
                    str.replace(g, 1, "\\\\t");
                    l = str.size();
                    g += 2;
                }


                if(str[g] == '\v')
                {
                    str.replace(g, 1, "&#92;v");
                    l = str.size();
                    g += 5;
                }


                if(str[g] == '/')
                {
                    str.replace(g, 1, "\\/");
                    l = str.size();
                    ++g;
                }

                if(str[g] == '\\')
                {
                    str.replace(g, 1, "&#92;");
                    l = str.size();
                    g += 4;
                }
                if(str[g] == '<')
                {
                    str.replace(g, 1, "&#60;");
                    l = str.size();
                    g += 4;
                }
                if(str[g] == '>')
                {
                    str.replace(g, 1, "&#62;");
                    l = str.size();
                    g += 4;
                }

                if(str[g] == '\x00'
                || str[g] == '\x01'
                || str[g] == '\x02'
                || str[g] == '\x03'
                || str[g] == '\x04'
                || str[g] == '\x05'
                || str[g] == '\x06'
                || str[g] == '\x07'
                || str[g] == '\x08'
                || str[g] == '\x09'
                || str[g] == '\x0A'
                || str[g] == '\x0B'
                || str[g] == '\x0C'
                || str[g] == '\x0D'
                || str[g] == '\x0E'
                || str[g] == '\x0F'
                || str[g] == '\x10'
                || str[g] == '\x11'
                || str[g] == '\x12'
                || str[g] == '\x13'
                || str[g] == '\x14'
                || str[g] == '\x15'
                || str[g] == '\x16'
                || str[g] == '\x17'
                || str[g] == '\x18'
                || str[g] == '\x19'
                || str[g] == '\x1A'
                || str[g] == '\x1B'
                || str[g] == '\x1C'
                || str[g] == '\x1D'
                || str[g] == '\x1E'
                || str[g] == '\x1F'
                || str[g] == '\x20'
                || str[g] == '\x7F'
                )
                {
                    str.replace(g, 1, " ");
                    l = str.size();
                }

                g++;
            }
            return;
        };

        bool authentication(string& login, string& password, string& id, string& token) {
            slashing(login);
            login = trim(login);

            wstring w_login(login.size(), L' ');
            w_login.resize(mbstowcs(&w_login[0], login.c_str(), login.size()));

            if(w_login.size() == 0)
                return false;

            slashing(password);
            password = trim(password);

            wstring w_password(password.size(), L' ');
            w_password.resize(mbstowcs(&w_password[0], password.c_str(), password.size()));

            if(w_password.size() == 0 || w_password.size() < 6)
                return false;

            password = "izfbiro35;#@f6$%7^*#42c" + password + "fuck_you;";

            unsigned char* str = (unsigned char*)(password.c_str());
            unsigned char result[64];

            WHIRLPOOL(str, password.size(), result);

            stringstream buff("");

            for(int i = 0; i < 64; i++)
                buff << setfill('0') << setw(2) << hex << (result[i] & 0xFF);

            password = buff.str();
            reverse(password.begin(), password.end());
            password = password + "6s452d9";

            return authorization(login, password, id, token);
        };

        bool authorization(const string& login, const string& password, string& id, string& token) {

            query(
                "SELECT "
                    "`id`, "
                    "`token` "
                "FROM "
                    "`user` "
                "WHERE "
                    "(`login` = '" + login + "' OR `email` = '" + login + "') AND "
                    "`password` = '" + password + "' AND "
                    "`activation` = 1"
            );

            if(res != NULL && (row = mysql_fetch_row(res)) != NULL) {
                id    = row[0];
                token = row[1];
                mysql_free_result(res);
                return true;
            } else {
                mysql_free_result(res);
                return false;
            }
        };
};

class LoginController: public Controller {
    public:
        string getResForGET(map<const char*, const char*, cmp_str>* parms) {

            addHeader("Content-Type:text/html; charset=UTF-8");

            LoginView view;

            return view.templ();
        }

        string getResForPOST(map<const char*, const char*, cmp_str>* parms) {

            map<string, string> data = parseQuery(setvalc(parms, "DATA"));

            string login    = urldecode(setval(&data, "login"));
            string password = urldecode(setval(&data, "pass"));
            string foreign  = urldecode(setval(&data, "foreign"));
            string id;
            string token;

            LoginModel model;

            if(login.size() > 0 && password.size() > 0 && model.authentication(login, password, id, token)) {
                vector<string> days {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
                vector<string> months {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

                time_t rawtime;
                struct tm* timeinfo;
                char buffer [80];

                time (&rawtime);

                rawtime += 2419200;

                timeinfo = localtime(&rawtime);

                string gmt = days[timeinfo->tm_wday] + ", %d " + months[timeinfo->tm_mon] + " %Y %T GMT";

                strftime(buffer, 80, gmt.c_str(), timeinfo);

                if(foreign.size() > 0 && foreign == "1") {
                    addHeader("Set-Cookie: id=" + id + "; path=/; expires=0; HttpOnly");
                    addHeader("Set-Cookie: token=" + token + "; path=/; expires=0; HttpOnly");
                } else {
                    addHeader("Set-Cookie: id=" + id + "; path=/; expires=" + buffer + "; HttpOnly");
                    addHeader("Set-Cookie: token=" + token + "; path=/; expires=" + buffer + "; HttpOnly");
                }

                return redirect(301, "MLG Theme", "/index.d", "/");
            }

            addHeader("Content-Type:text/html; charset=UTF-8");

            LoginView view;

            return view.templ();
        }

        string run(map<const char*, const char*, cmp_str>* parms) {
            string method = setvalc(parms, "REQUEST_METHOD");

            if(method == "GET")
                return getResForGET(parms);
            if(method == "POST")
                return getResForPOST(parms);

            return getResForGET(parms);
        }
};