class IView : public View {
    public:
//        User main_user;

        IView() : View() {
            aside =
            "<aside id=\"aside\" style=\"display:none\"><div class=\"a-wrap\"></div></aside>";
        }

//        void setMainUser(User &_user) {
//            main_user = _user;
//        };
};

class IModel : public Model {
    public:
        User setMainUser(const string& id) {
            query(
                "SELECT SQL_CACHE "
                    "`u`.`first_name`, "
                    "`u`.`last_name`, "
                    "`u`.`avatar` "
                "FROM "
                    "`user` `u` "
                "WHERE "
                    "`u`.`id` = " + id + " AND "
                    "`u`.`activation` = 1"
            );

            User main_user;

            if(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                main_user.id = id;
                main_user.first_name = row[0];
                main_user.last_name = row[1];
                main_user.avatar = row[2];
            }

            mysql_free_result(res);

            return main_user;
        };

        void getData(const string& start, const string& user_id, const string& main_user_id) {
            string str;
            bool user_is_set = user_id.size() > 0;

            if(user_is_set)
                str = ", (SELECT COUNT(l.id_content) FROM `like` l WHERE l.id_user = " + user_id + " AND l.id_content = c.id AND l.is_like = 1 AND l.type_content = 0 ) as `like` ";

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
                "WHERE "
                    "c.id_adder = " + main_user_id + " AND "
                    "c.type_adder = 0 AND "
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

        vector<Data> getDataIndex(const string& start, const string& user_id, const string& main_user_id) {

            vector<Data> arr;

            getData(start, user_id, main_user_id);

            bool user_is_set = user_id.size() > 0;
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

        string getJSON(const string& start, const string& user_id, const string& main_user_id, const bool& canAddNote) {
            getData(start, user_id, main_user_id);

            bool user_is_set = user_id.size() > 0;

            short size = mysql_num_rows(res);
            short size_l = size-1;
            short i = 0;

            string str = "[";

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                str += "{"
                    "\"id\":" + (string)row[0] + ","
                    "\"param\":" + (strlen(row[1]) > 0 ? (string)row[1] : "\"\"") + ","
                    "\"desc\":\"" + (string)row[2] + "\","
                    "\"adder_id\":\"" + (string)row[3] + "\","
                    "\"adder_name\":\"" + (string)row[4] + "\","
                    "\"adder_avatar\":\"" + (string)row[5] + "\","
                    "\"amount_comments\":" + (string)row[6] + ","
                    "\"amount_like\":" + (string)row[7] + ","
                    "\"like\":" + (user_is_set ? row[8] : "0") + ""
                "}" + ((i++ == size_l) ? "" : ",");
            }

            mysql_free_result(res);

            str += "]";

            string obj =
            "{"
                "\"data\":" + str + ","
                "\"user\":" + (user_is_set ? "" + user_id + "" : "\"0\"") + ","
                "\"right\":[" + (canAddNote ? "\"addNote\"" : "") + "]" +
            "}";

            return obj;
        };
};

class IController: public Controller {
    public:
        IController() {
            fillRights();
        }

        string run(map<const char*, const char*, cmp_str>* parms) {
            if(strcmp(setvalc(parms, "REQUEST_METHOD"), "GET") != 0)
                return "";

            addHeader("Content-Type:text/html; charset=UTF-8");

            IModel model;
            IView  view;
            User   user;
            User   main_user;

            map<string, string> GET = parseQuery(setvalc(parms, "QUERY_STRING"));

            string id = setval(&GET, "id");
            if( !id.size() || strspn( id.c_str(), "0123456789" ) != id.size() )
                return "";

            main_user = model.setMainUser(id);

            if(main_user.id.size() == 0) {
                return "";
            }

            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

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

            string ajax_request = setvalc(parms, "HTTP_AJAX_REQUEST");

            bool canAddNote = hasRight(user.role, "addNote");

            if(ajax_request.size() > 0 && ajax_request == "xmlhttprequest")
            {
                return model.getJSON(start, user.id, main_user.id, canAddNote);
            }

            view.setTitle(user.login);
//            view.setMainUser(main_user);
            view.getContent(model.getDataIndex(start, user.id, main_user.id));
            view.setRights(right);

            return view.templ();
        }
};