class GetCommentsModel : public Model {
    public:
        string getCommentsForPost(const string& post_id, const string& post_type, User& user) {

            query(
                "SELECT "
                    "c.text, "
                    "u.avatar, "
                    "u.login "
                "FROM "
                    "comment c "
                "LEFT JOIN"
                    "`user` u "
                "ON "
                    "c.id_user = u.id "
                "WHERE "
                    "c.id_content = " + post_id + " AND "
                    "c.type_content = " + post_type + " "
//                "ORDER BY c.id"
            );

            int size_l = mysql_num_rows(res) - 1;
            int i = 0;
            string _s;
            string result = "{\"data\":[";

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                _s = (i == size_l) ? "" : ",";
                string text = row[0];
                string avatar = row[1];
                string login = row[2];

                result += "{\"text\":\"" + text + "\",\"avatar\":\"" + avatar + "\",\"login\":\"" + login + "\"}" + _s;

                i++;
            }

            result += "]}";

            mysql_free_result(res);

            return result;
        };
};

class GetCommentsController: public Controller {
    public:
        string run(map<const char*, const char*, cmp_str>* parms) {
            if(strcmp(setvalc(parms, "REQUEST_METHOD"), "GET") != 0)
                return "";

            map<string, string> GET = parseQuery(setvalc(parms, "QUERY_STRING"));
            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

            addHeader("Content-Type:application/json; charset=UTF-8");

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

            GetCommentsModel model;
            User user;

            if(user_id.size() > 0 && user_token.size() > 0)
                user = model.getDataUser(user_id, user_token);

            return model.getCommentsForPost(post_id, post_type, user);
        }
};