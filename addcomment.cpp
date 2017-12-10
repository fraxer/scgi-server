class AddCommentModel : public Model {
    public:
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

        string addCommentToPost(const string& content_id, const string& content_type, string& text, User& user) {

            text = urldecode(text);

            string _t = text;
            if(trim(_t).length() == 0)
                return "";

            text = slashing(text);

            query(
                "INSERT INTO "
                    "`comment` "
                    "(`id_content`, `text`, `id_user`, `type_content`) "
                "VALUES "
                    "(" + content_id + ", '" + text + "', " + user.id + ", '" + content_type + "')"
            );

            mysql_free_result(res);

            query(
                "SELECT "
                    "c.text "
                "FROM "
                    "comment c "
                "WHERE "
                    "c.id = LAST_INSERT_ID()"
            );

            while((row = mysql_fetch_row(res)) != NULL)
            {
                 text = row[0];
            }

            mysql_free_result(res);

            return "{\"online\":\"" + user.id + "\",\"data\":[{\"text\":\"" + text + "\",\"avatar\":\"" + user.avatar + "\",\"login\":\"" + user.login + "\"}]}";
        };
};

class AddCommentController: public Controller {
    public:
        string run(map<const char*, const char*, cmp_str>* parms) {
            if(strcmp(setvalc(parms, "REQUEST_METHOD"), "POST") != 0)
                return "";

            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

            map<string, string> data = parseQuery(setvalc(parms, "DATA"));

            string content_id = setval(&data, "id_content");
            if( !content_id.size() || strspn(content_id.c_str(), "0123456789") != content_id.size() || stoi(content_id) == 0 )
                return "";

            string content_type = setval(&data, "type");
            if( !content_type.size() || strspn(content_type.c_str(), "0123456789") != content_type.size() )
                return "";

            string text = setval(&data, "text");
            if(!text.size())
                return "";

            string user_id = setval(&COOKIE, "id");
            if( strspn( user_id.c_str(), "0123456789" ) != user_id.size() )
                user_id = "";

            string user_token = setval(&COOKIE, "token");
            if( strspn( user_token.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz" ) != user_token.size() )
                user_token = "";

            AddCommentModel model;
            User user;

            if(user_id.size() > 0 && user_token.size() > 0)
                user = model.getDataUser(user_id, user_token);
            else
                return "";

            return model.addCommentToPost(content_id, content_type, text, user);
        }
};