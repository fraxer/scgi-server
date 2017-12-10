class PostView : public View {
    public:
        PostView() : View() {
            aside =
            "<aside id=\"aside\" style=\"display:none\"><div class=\"a-wrap\"></div></aside>";
        }
};

class PostModel : public Model {
    public:
        void getData(const string& post_id, const User& user) {
            string str;
            string str2 = " WHERE ";
            bool user_is_set = user.id.size() > 0;

            if(user_is_set) {
                str = ", (SELECT COUNT(l.id_content) FROM `like` l WHERE l.id_user = " + user.id + " AND l.id_content = c.id AND l.is_like = 1 AND l.type_content = 0 ) as `like` ";
            }

            query(
                "SELECT SQL_CACHE "
                    "c.id, "
                    "c.param, "
                    "c.`desc`, "
                    "c.`title`, "
                    "c.`description`, "
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
                    "c.visibility = 1 AND "
                    "c.id = " + post_id
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

        vector<Data> getDataPost(const string& post_id, const User& user) {

            vector<Data> arr;
            getData(post_id, user);
            bool user_is_set = user.id.size() > 0;
            string desc;
            string title;
            string description;

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                desc = row[2];
                title = row[3];
                description = row[4];

                Data d;
                     d.id = row[0];
                     d.param = row[1];
                     d.desc = clearSlashing(desc);
                     d.title = clearSlashing(title);
                     d.description = clearSlashing(description);
                     d.adder_id = row[5];
                     d.adder_name = row[6];
                     d.adder_avatar = row[7];
                     d.amount_comments = row[8];
                     d.amount_like = row[9];
                     d.like = user_is_set ? row[10] : "0";

                arr.push_back(d);
            }

            mysql_free_result(res);

            return arr;

        };

       string getJSON(const string& start, const User& user, const bool& canAddNote) {
           getData(start, user);

           bool user_is_set = user.id.size() > 0;

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
               "\"user\":" + (user_is_set ? "" + user.id + "" : "\"0\"") + ","
               "\"right\":[" + (canAddNote ? "\"addNote\"" : "") + "]" +
           "}";

           return obj;
       };
};

class PostController: public Controller {
    public:
        PostController() {
            fillRights();
        }

        string run(map<const char*, const char*, cmp_str>* parms) {
            if(strcmp(setvalc(parms, "REQUEST_METHOD"), "GET") != 0)
                return "";

            addHeader("Content-Type:text/html; charset=UTF-8");

            PostModel model;
            PostView view;
            User user;
//            addHeader("Set-Cookie: name=q; path=/;");

            map<string, string> GET = parseQuery(setvalc(parms, "QUERY_STRING"));
            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

            string post_id = setval(&GET, "id");
            if( !post_id.size() || strspn( post_id.c_str(), "0123456789" ) != post_id.size() )
                return redirect(301, "MLG Theme", "/index.d", "/");

            string user_id = setval(&COOKIE, "id");
            if( strspn( user_id.c_str(), "0123456789" ) != user_id.size() )
                user_id = "";

            string user_token = setval(&COOKIE, "token");
            if( strspn( user_token.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz" ) != user_token.size() )
                user_token = "";

            // TODO fix user access
            if(user_id.size() > 0 && user_token.size() > 0) {
                user = model.getDataUser(user_id, user_token);
                view.setUser(user);
            }

            string ajax_request = setvalc(parms, "HTTP_AJAX_REQUEST");

            bool canAddNote = hasRight(user.role, "addNote");

            string start = "0";

            if(ajax_request.size() > 0 && ajax_request == "xmlhttprequest") {
                return model.getJSON(start, user, canAddNote);
            }

            vector<Data> arr(model.getDataPost(post_id, user));


            if(arr.size() != 0) {
                view.setTitle(arr[0].title);
                view.setDescription(arr[0].description);
            }
            view.getContent(arr);
            view.setRights(right);

            return view.templ();
        }
};