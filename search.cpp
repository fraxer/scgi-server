class SearchView : public View {
    public:
        SearchView() : View() {
            aside =
            "<aside id=\"aside\" style=\"display:none\"><div class=\"a-wrap\"></div></aside>";
        }
};

class SearchModel : public Model {
    public:
        void getData(vector<string> words, string& start, User& user) {
            string str;
            string str2;
            bool user_is_set = user.id.size() > 0;

            if(user_is_set) {
                str = ", (SELECT COUNT(l.id_content) FROM `like` l WHERE l.id_user = " + user.id + " AND l.id_content = c.id AND l.is_like = 1 AND l.type_content = 0 ) as `like` ";
            }

            short size = words.size();

            if(size) {
                short size_l = size - 1;
                string desc;
                string _s;

                for(short i = 0; i < size; i++) {
                    _s = (i == size_l) ? "" : "AND ";

                    desc += "(c.`desc` LIKE '%" + words[i] + "%') " + _s;
                }

                str2 = "(" + desc + ") AND ";
            }

            query(
                "SELECT "
                    "c.id, "
                    "c.param, "
                    "c.`desc`, "
                    "IF(c.type_adder = 0, u.id, cm.id) as id, "
                    "IF(c.type_adder = 0, u.login, cm.name) as name, "
                    "IF(c.type_adder = 0, u.avatar, cm.avatar) as avatar, "
                    "(SELECT COUNT(cm.id) FROM comment cm WHERE c.id = cm.id_content AND cm.type_content = 0 ) as amount_comments, "
                    "(SELECT COUNT(l.id_content) FROM `like` l WHERE l.id_content = c.id AND l.is_like = 1 AND l.type_content = 0 ) as `amount_like` "
                 + str +
                "FROM "
                    "post c "
                "LEFT JOIN "
                   "`user` u "
                "ON "
                    "c.id_adder = u.id "
                "LEFT JOIN "
                   "`community` cm "
                "ON "
                    "c.id_adder = cm.id "
                "WHERE "
                     + str2 +
                    "c.visibility = 1 "
                "ORDER BY c.id DESC "
                "LIMIT " + start + ", 20"
            );
        };

        const string& clearSlashing(string& str) {
            int g = 0;
            int l = str.size();

            while(g < l)
            {
                if(str[g] == '\\' && str[g+1] == 'n')
                {

                    str.replace(g, 2, "\n");
                    l = str.size();
                    g--;
                    continue;
                }

                if(str[g] == '\\' && str[g+1] == 't')
                {
                    str.replace(g, 2, "\t");
                    l = str.size();
                    g--;
                }

                g++;
            }

            return str;
        };

        vector<Data> getDataSearch(vector<string> words, string& start, User& user) {
            vector<Data> arr;
            getData(words, start, user);
            bool user_is_set = user.id.size() > 0;
            string desc;

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                desc = row[2];

                Data d;
                     d.id = row[0];
                     d.param = row[1];
                     d.desc = clearSlashing(desc);
                     d.adder_id = row[3];
                     d.adder_name = row[4];
                     d.adder_avatar = row[5];
                     d.amount_comments = row[6];
                     d.amount_like = row[7];
                     d.like = user_is_set ? row[8] : "0";

                arr.push_back(d);
            }

            mysql_free_result(res);

            return arr;

        };

        string getJSON(vector<string> words, string& start, User& user, const bool& canAddNote) {
            getData(words, start, user);

            bool user_is_set = user.id.size() > 0;

            int size = mysql_num_rows(res);
            int size_l = size-1;
            int i = 0;

            string str = "[";
            string _s;
            string param;
            string desc;

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                _s = (i == size_l) ? "" : ",";

                param = row[1];
                desc = row[2];

                str += "{"
                    "\"id\":" + string(row[0]) + ","
                    "\"param\":" + (param.size() > 0 ? param : "\"\"") + ","
                    "\"desc\":\"" + (desc.size() > 0 ? desc : "") + "\","
                    "\"adder_id\":\"" + (string)row[3] + "\","
                    "\"adder_name\":\"" + (string)row[4] + "\","
                    "\"adder_avatar\":\"" + (string)row[5] + "\","
                    "\"amount_comments\":" + string(row[6]) + ","
                    "\"amount_like\":" + string(row[7]) + ","
                    "\"like\":" + (user_is_set ? row[8] : "0") + ""
                "}" + _s;

                i++;
            }

            mysql_free_result(res);

            str += "]";

            string obj =
            "{"
                "\"data\":" + str + ","
                "\"online\":" + (user_is_set ? "" + user.id + "" : "\"0\"") + ","
                "\"right\":[" + (canAddNote ? "\"addNote\"" : "") + "]" +
            "}";

            return obj;
        };
};

class SearchController: public Controller {
    public:
        SearchController() {
            fillRights();
        }

        string run(map<const char*, const char*, cmp_str>* parms) {
            if(strcmp(setvalc(parms, "REQUEST_METHOD"), "GET") != 0)
                return "";

            addHeader("Content-Type:text/html; charset=UTF-8");

            SearchModel model;
            SearchView view;
            User user;

            map<string, string> GET = parseQuery(setvalc(parms, "QUERY_STRING"));
            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

            string start = setval(&GET, "start");

            if( !start.size() || strspn( start.c_str(), "0123456789" ) != start.size() )
                start = "0";

            string query = setval(&GET, "query");

            if(!query.size())
                query = "";
            else {
                query = urldecode(query);
                query = trim(query);
                query = slashing(query);
            }

            vector<string> words(prepareArrayOfWords(query));

            string user_id = setval(&COOKIE, "id");

            if( strspn( user_id.c_str(), "0123456789" ) != user_id.size() )
                user_id = "";

            string user_token = setval(&COOKIE, "token");

            if( strspn( user_token.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz" ) != user_token.size() )
                user_token = "";

            // TODO fix user access
            if(user_id.size() > 0 && user_token.size() > 0)
            {
                user = model.getDataUser(user_id, user_token);
                view.setUser(user);
            }

            string ajax_request = setvalc(parms, "HTTP_AJAX_REQUEST");

            bool canAddNote = hasRight(user.role, "addNote");

            if(ajax_request.size() > 0 && ajax_request == "xmlhttprequest")
            {
                return model.getJSON(words, start, user, canAddNote);
            }

            view.setRights(right);

            if(words.size() > 0)
                view.getContent(model.getDataSearch(words, start, user));

            return view.templ();
        }

        const string& slashing(string& str) {
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

            return str;
        }

        vector<string> prepareArrayOfWords(string& query) {
            vector<string> short_words;

            if(query.size() < 2)
                return short_words;

            query = trim(query);

            vector<string> words(split(query));
            short size = words.size();

            for(short i = 0; i < size; i++) {
                short l = words[i].size();
                if(l >= 2) {
                    wstring ws(words[i].size(), L' ');
                    ws.resize(mbstowcs(&ws[0], words[i].c_str(), words[i].size()));

                    short ld = (floor(ws.size() / 2) + 2) * (l / ws.size()); // Ищем слова "без окончаний" TODO посмотреть начсет кодировок
                    short_words.push_back(words[i].substr(0, ld));
                }
            }

            return short_words;
        }

        vector<string> split(string& str) {
            vector<string> v;

            if(!str.size())
                return v;

            int g = 0;
            int l = str.size();

            while(l > 0 && g < l)
            {
                if(g == 0 && str[g] == ' ') {
                    str.replace(0, g+1, "");
                    l = str.size();
                    g = -1;
                }
                if(str[g+1] == '\0')
                {
                    v.push_back(str);
                    break;
                }
                if(str[g] == ' ')
                {
                    v.push_back(str.substr(0, g));
                    str.replace(0, g+1, "");
                    l = str.size();
                    g = -1;
                }

                g++;
            }

            return v;
        }
};