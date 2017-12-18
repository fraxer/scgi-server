class LikeModel : public Model {
    public:
        void getData(const string& id_content, const string& type_content, const string& id_user, string *flag) {
            string table = "";

            if(type_content == "0")
                table = "`post` `t` ";
            if(type_content == "1")
                table = "`content` `t` ";
            if(type_content == "2")
                table = "`product` `t` ";
            if(type_content == "3")
                table = "`product_content` `t` ";
//            if(type_content == "4")
//                table = "`comment` ";

            if(table == "")
                return;

            query(
                "SELECT "
                    "`l`.`is_like` "
                "FROM "
                    + table +
                "LEFT JOIN "
                    "`like` `l`"
                "ON "
                    "`l`.`id_content` = `t`.`id` AND "
                    "`l`.`type_content` = " + type_content + " AND "
                    "`l`.`id_user` = " + id_user + " "
                "WHERE "
                    "`t`.`id` = " + id_content
            );
            //                "SELECT "
            //                    "id, "
            //                  "(SELECT is_like FROM `like`  WHERE  id_content = " + id_content + " AND id_user = " + id_user + ") "
            //                "FROM "
            //                    "content "
            //                "WHERE "
            //                    "id = " + id_content

            if(mysql_num_rows(res) > 0)
            {
                row = mysql_fetch_row(res);

                mysql_free_result(res);

                if(row[0] != NULL)
                {
                    string is_like = string(row[0]) == "1" ? "0" : "1";

                    query(
                        "UPDATE "
                            "`like` "
                        "SET "
                            "`is_like` = " + is_like + " "
                        "WHERE "
                            "`id_content` = " + id_content + " AND "
                            "`type_content` = " + type_content + " AND "
                            "`id_user` = " + id_user

                     );

                    *flag = (is_like == "0") ? "off" : "on";
                }
                else
                {
                    query(
                        "INSERT INTO "
                            "`like` "
                        "(`id_content`, `type_content`, `id_user`) VALUES (" + id_content + ", " + type_content + ", " + id_user + ")"
                    );

                    *flag = "on";
                }
            }

            mysql_free_result(res);
        };

        string& getDataLike(const string& id_content, const string& type_content, const string& id_user) {
            string *flag = new string();

            getData(id_content, type_content, id_user, flag);

            return *flag;
        };
};

class LikeController: public Controller {
    public:
        string& run(map<const char*, const char*>& parms) {
            if(strcmp(getValueFromMap(parms, "REQUEST_METHOD"), "GET") != 0)
                return *(new string);

            addHeader("X-Accel-Expires: 0");

            map<string, string> GET = parseQuery(getValueFromMap(parms, "QUERY_STRING"));

            string post_id = setval(&GET, "id");
            if( !post_id.size() || strspn( post_id.c_str(), "0123456789" ) != post_id.size() )
                return *(new string);

            string post_type = setval(&GET, "type");
            if( !post_type.size() || strspn( post_type.c_str(), "0123456789" ) != post_type.size() )
                return *(new string);

            map<string, string> COOKIE = parseCookie(getValueFromMap(parms, "HTTP_COOKIE"));

            string user_id = setval(&COOKIE, "id");
            if( strspn( user_id.c_str(), "0123456789" ) != user_id.size() )
                return *(new string);

            string user_token = setval(&COOKIE, "token");
            if( strspn( user_token.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz" ) != user_token.size() )
                return *(new string);

            LikeModel model;
            User user;

            if(user_id.size() > 0 && user_token.size() > 0)
                user = model.getDataUser(user_id, user_token);
            else
                return *(new string);

            return model.getDataLike(post_id, post_type, user.id);
        }
};