class IndexView : public View {
    public:
        IndexView() : View() {
            aside =
            "<aside id=\"aside\" style=\"display:none\"><div class=\"a-wrap\"></div></aside>";
        }
};

class IndexModel : public Model {
    public:
        void getData(const string& start, const User& user) {
            string str;
            string str2 = " WHERE ";
            bool user_is_set = user.id.size() > 0;

            if(user_is_set)
            {
                str = ", (SELECT COUNT(l.id_content) FROM `like` l WHERE l.id_user = " + user.id + " AND l.id_content = c.id AND l.is_like = 1 AND l.type_content = 0 ) as `like` ";

                str2 =
                "LEFT JOIN "
                    "follow f "
                "ON "
                    "f.id_follower = " + user.id + " "
                "WHERE "
                    "c.id_adder = f.id_target AND "
                    "c.type_adder = f.type_adder AND ";
            }

            query(
                "SELECT SQL_CACHE "
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
        }

        vector<Data> getDataIndex(const string& start, const User& user) {

            vector<Data> arr;
            getData(start, user);
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

        string& getJSON(const string& start, const User& user, const bool& canAddNote) {
            getData(start, user);

            bool user_is_set = user.id.size() > 0;

            short size = mysql_num_rows(res);
            short size_l = size-1;
            short i = 0;

            string str = "[";

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                str += "{"
                    "\"id\":" + string(row[0]) + ","
                    "\"param\":" + (strlen(row[1]) > 0 ? row[1] : "\"\"") + ","
                    "\"desc\":\"" + row[2] + "\","
                    "\"adder_id\":\"" + row[3] + "\","
                    "\"adder_name\":\"" + row[4] + "\","
                    "\"adder_avatar\":\"" + row[5] + "\","
                    "\"amount_comments\":" + row[6] + ","
                    "\"amount_like\":" + row[7] + ","
                    "\"like\":" + (user_is_set ? row[8] : "0") + ""
                "}" + ((i++ == size_l) ? "" : ",");
            }

            mysql_free_result(res);

            str += "]";

            string *obj = new string(
            "{"
                "\"data\":" + str + ","
                "\"user\":" + (user_is_set ? "" + user.id + "" : "\"0\"") + ","
                "\"right\":[" + (canAddNote ? "\"addNote\"" : "") + "]" +
            "}");

            return *obj;
        };
};

class IndexController: public Controller {
    public:
        IndexController() {
            fillRights();
        }

        string& run(map<const char*, const char*>& parms) {
            if(strcmp(getValueFromMap(parms, "REQUEST_METHOD"), "GET") != 0)
                return *(new string);

            addHeader("Content-Type:text/html; charset=UTF-8");

            IndexModel model;
            IndexView view;
            User user;
//            addHeader("Set-Cookie: name=q; path=/;");

            map<string, string> GET = parseQuery(getValueFromMap(parms, "QUERY_STRING"));
            map<string, string> COOKIE = parseCookie(getValueFromMap(parms, "HTTP_COOKIE"));

            string start = setval(&GET, "start");
            if( !start.size() || strspn( start.c_str(), "0123456789" ) != start.size() )
                start = "0";

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

            const char* har = getValueFromMap(parms, "HTTP_AJAX_REQUEST");

            if(strlen(har) > 0 && strcmp(har, "xmlhttprequest") == 0) {
                bool canAddNote = hasRight(user.role, "addNote");

                return model.getJSON(start, user, canAddNote);
            }

            

            view.setTitle("Новостная лента");
            view.setDescription("IT новости, статьи, идеи, макеты, работы.");
            view.getContent(model.getDataIndex(start, user));
            view.setRights(right);

            return view.templ();
        }
};