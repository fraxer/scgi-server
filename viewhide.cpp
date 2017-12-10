class ViewHideModel : public Model {
    public:
        bool getData(const string& id, const string& type, const string& start, const string& count, const User& user) {
            string str;
            string str2;
            string table = "";
            bool user_is_set = user.id.size() > 0;

            if(type == "1")
                table = "`content` ";
            if(type == "3")
                table = "`product_content` ";
            if(table == "")
                return false;

            if(user_is_set) {
                str = ", (SELECT COUNT(l.id_content) FROM `like` l WHERE l.id_user = " + user.id + " AND l.id_content = c.id AND l.is_like = 1 AND l.type_content = " + type + " ) as `like`";
                str2 = " AND c.id_adder = " + user.id;
            }
            else
                return false;

            //TODO пофиксить запрос в условии (проверка на visible)
            query(
                "SELECT SQL_CACHE "
                    "c.id, "
                    "c.type, "
                    "c.param, "
                    "c.`desc`, "
                    "IF(c.type_adder = 0, u.login, cm.name), "
                    "IF(c.type_adder = 0, u.avatar, cm.avatar), "
                    "(SELECT COUNT(cm.id) FROM comment cm WHERE c.id = cm.id_content AND cm.type_content = " + type + " ) as amount_comments, "
                    "(SELECT COUNT(l.id_content) FROM `like` l WHERE l.id_content = c.id AND l.is_like = 1 AND l.type_content = " + type + " ) as `amount_like` "
                 + str +
                "FROM "
                    + table + " as c "
                "LEFT JOIN "
                   "`user` u "
                "ON "
                    "c.id_adder = u.id AND c.type_adder = 0 "
                "LEFT JOIN "
                   "`community` cm "
                "ON "
                    "c.id_adder = cm.id AND c.type_adder = 1 "
                "WHERE "
                    "c.id = " + id + " AND c.type <> 1  AND c.type <> 0 " + str2 + " "
                //"ORDER BY c.id ASC"
            );

            return true;
        };

        void getListComments(const string& id, const string& type) {
            query(
                "SELECT SQL_CACHE "
                    "c.text, "
                    "u.avatar, "
                    "u.login "
                "FROM "
                    "comment c "
                "LEFT JOIN "
                    "`user` u "
                "ON "
                    "c.id_user = u.id "
                "WHERE "
                    "c.id_content = " + id + " AND "
                    "c.type_content = " + type + " "
                "ORDER BY c.id"
            );
        };

        string& slashing(string& str) {
            int g = 0;

            while(str[g] != '\0')
            {
                g++;

//                if(str[g] == '\n')
//                {
//                    str.replace(g, 1, "\\n");
//                    g++;
//                }
//                if(str[g] == '\t')
//                {
//                    str.replace(g, 1, "\\t");
//                    g++;
//                }
                if(str[g] == '\\' && str[g+1] == 'n')
                {
                    str.insert(g, "\\");
                    g++;
                }
//                if(str[g] == '\\' && str[g+1] == 't')
//                {
//                    str.insert(g, "\\");
//                    g++;
//                }
            }

            return str;
        };

        string getJSON(const string& id, const string& type, const string& start, const string& count, const User& user) {
            if(!getData(id, type, start, count, user)) {
                return "[[],[]]";
            }

            bool user_is_set = user.id.size() > 0;
            int size = mysql_num_rows(res);
            int size_l = size-1;
            string _s;
            int i = 0;
            vector<string> ids;

            string str = "[";

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                 _s = (i == size_l) ? "" : ",";

                 string param = row[2];
                 string desc = row[3];

                 ids.push_back(row[0]);

                 str += "{"
                     "\"id\":" + string(row[0]) + ","
                     "\"type\":" + string(row[1]) + ","
                     "\"param\":" + (param.size() > 0 ? param : "\"\"")+ ","
                     "\"desc\":\"" + (desc.size() > 0 ? back_slash(desc) : "") + "\","
                     "\"login\":\"" + string(row[4]) + "\","
                     "\"avatar\":\"" + string(row[5]) + "\","
                     "\"amount_comments\":" + string(row[6]) + ","
                     "\"amount_like\":" + string(row[7]) + ","
                     "\"like\":" + (user_is_set ? row[8] : "0") + ""
                 "}" + _s;

                i++;
            }

            str += "]";

            mysql_free_result(res);

            string comments = "[";

            for(i = 0; i < size; i++)
            {
                getListComments(ids[i], type);

                comments += "{\"data\":[";

                int size_c = mysql_num_rows(res) - 1;
                int j = 0;

                while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
                {
                    _s = (j == size_c) ? "" : ",";

                    string text = row[0];
                    string login = row[2];

//                    string t = back_slash(text);

                    comments += "{"
                        "\"text\":\"" + back_slash(text) + "\","
                        "\"avatar\":\"" + string(row[1]) + "\","
                        "\"login\":\"" + back_slash(login) + "\""
                    "}" + _s;

                    j++;
                }

                _s = (i == size_l) ? "" : ",";

                comments += "]}" + _s;

                mysql_free_result(res);
            }

            comments += "]";

            string json =
            "["
                + str + ","
                + comments
                + (user_is_set ? ",{\"user\":" + user.id + "}" : "") +
            "]";

            return json;
        };
};

class ViewHideController: public Controller {
    public:
        string run(map<const char*, const char*, cmp_str>* parms) {
            if(strcmp(setvalc(parms, "REQUEST_METHOD"), "GET") != 0)
                return "";

            addHeader("Content-Type:text/html; charset=UTF-8");

            map<string, string> GET = parseQuery(setvalc(parms, "QUERY_STRING"));
            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

            string start = setval(&GET, "start");

            if( !start.size() || strspn( start.c_str(), "0123456789" ) != start.size() )
                start = "0";

            string post_id = setval(&GET, "id");
            if( !post_id.size() || strspn( post_id.c_str(), "0123456789" ) != post_id.size() || stoi(post_id) == 0 )
                return "";

            string post_type = setval(&GET, "type");
            if( !post_type.size() || strspn( post_type.c_str(), "0123456789" ) != post_type.size() )
                return "";

            string user_id = setval(&COOKIE, "id");

            if( strspn( user_id.c_str(), "0123456789" ) != user_id.size() )
                user_id = "";

            string user_token = setval(&COOKIE, "token");

            if( strspn( user_token.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz" ) != user_token.size() )
                user_token = "";

            ViewHideModel model;
            User user;

            if(user_id.size() > 0 && user_token.size() > 0)
                user = model.getDataUser(user_id, user_token);
            else
                return "";

            return model.getJSON(post_id, post_type, start, amount_posts, user);
        }
};