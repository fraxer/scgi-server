class UploadZIPModel : public Model {
    public:
        string addContent(const string& post_id, const string& content_type, const string& json) {

            query("SELECT `id`, COUNT(`id`) as `count` FROM `content` WHERE (`id_parent` = " + post_id + " AND `type` = 0) LIMIT 1");

            row = mysql_fetch_row(res);

            string string_id = row[0];
            int count_free = atoi(row[1]);

            mysql_free_result(res);

            //TODO порешать со скобками у json
            if(count_free == 10) {
                query(
                    "UPDATE "
                        "`post` "
                    "SET "
                        "`param` = '" + json + "' "
                    "WHERE "
                        "`id` = " + post_id
                );
                mysql_free_result(res);
            }

            query(
                "UPDATE "
                    "`content` "
                "SET "
                    "`type` = " + content_type + ", "
                    "`param` = '" + json + "' "
                "WHERE "
                    "`id` = " + string_id
            );

            count_free--;

            mysql_free_result(res);

            return string("\"id\":" + string_id + ",\"free\":" + to_string(count_free));
        }

        string getIdPost(const string& user_id) {
            query(
                "SELECT "
                    "`id_parent_content` "
                "FROM "
                    "`user_uploads` "
                "WHERE `id_user` = " + user_id
            );

            row = mysql_fetch_row(res);

            string id = row[0];

            mysql_free_result(res);

            return id;
        }

        string reservePlace(const string& user_id) {
            query("INSERT INTO `post` (`id`, `visibility`, `id_adder`, `type_adder`, `param`, `desc`, `date`) VALUES (NULL, '0', '" + user_id + "', '0', '', '', CURRENT_TIMESTAMP)");

            string post_id = to_string(mysql_insert_id(mysql));

            mysql_free_result(res);

            query("INSERT INTO `content` (`id`, `visibility`, `type`, `id_adder`, `type_adder`, `id_parent`, `param`, `desc`, `hash`, `date`) "
                "VALUES "
                    "(NULL, '0', '0', '" + user_id + "', '0', '" + post_id + "', '', '', '', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + user_id + "', '0', '" + post_id + "', '', '', '', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + user_id + "', '0', '" + post_id + "', '', '', '', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + user_id + "', '0', '" + post_id + "', '', '', '', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + user_id + "', '0', '" + post_id + "', '', '', '', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + user_id + "', '0', '" + post_id + "', '', '', '', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + user_id + "', '0', '" + post_id + "', '', '', '', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + user_id + "', '0', '" + post_id + "', '', '', '', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + user_id + "', '0', '" + post_id + "', '', '', '', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + user_id + "', '0', '" + post_id + "', '', '', '', CURRENT_TIMESTAMP) "
            );

            mysql_free_result(res);

            query("UPDATE `user_uploads` SET `id_parent_content` = " + post_id + " WHERE `id_user` = " + user_id);

            mysql_free_result(res);

            return post_id;
        }

        bool hasPlace(const string& user_id) {
            query(
                "SELECT "
                    "`id_parent_content`, "
                    "(SELECT COUNT(`c`.`id`) FROM `content` as `c` WHERE `c`.`id_parent` = `uu`.`id_parent_content` AND `c`.`type` = 0 LIMIT 1) "
                "FROM "
                    "`user_uploads` as `uu`"
                "WHERE `uu`.`id_user` = " + user_id
            );

            row = mysql_fetch_row(res);

            string parent_id = row[0];
            string count = row[1];

            mysql_free_result(res);

            if(parent_id == "0") {
                return true;
            }
            else {
                if(count == "0")
                    return false;
                else
                    return true;
            }
        }
};



class UploadZIPController: public Controller {
    public:
        void writeFile(const string &path, $_FILE &file) {
            ofstream sfile;

            sfile.open(path.c_str(), ios::out | ios::binary);
            if (sfile.is_open())
            {
                sfile.write(file.body, file.size);
                sfile.close();
            }
        }

        string getResForPOST(map<const char*, const char*, cmp_str>* parms, UploadZIPModel& model, User& user) {

            if(!model.hasPlace(user.id))
                return "{\"error\":\"no free space\"}";

            string content_type = setvalc(parms, "CONTENT_TYPE");

            unsigned int mpdindex = content_type.find("form-data;");

            if(mpdindex != 10)
                return "{\"status\":\"denied\"}";

            unsigned int bndindex = content_type.find("boundary=");
            string boundary = "--" + content_type.substr(bndindex+9);

            const char* data = setvalc(parms, "DATA");

            unsigned int len = atoi(setvalc(parms, "CONTENT_LENGTH"));

//            ofstream myfile;
//            myfile.open("upl.bin", ios::out | ios::binary);
//
//            if (myfile.is_open())
//            {
//                myfile.write(data, len);
//                myfile.close();
//            }

//            return "stop";

            vector<$_FILE> FILES;
            vector<$_PARAM> PARAMS;

            int s = parseData(data, boundary, len, FILES, PARAMS);

            if(s==0)
                return "error read file";

            random_device rd;
            mt19937 rng(rd());
            uniform_int_distribution<int> uni(0, 61);
            string chrs = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
            string json;

            int fsize = FILES.size();
            int size_l = fsize-1;

            if(fsize == 0)
                return json;

            json += "[";

            for(int o = 0; o < fsize; o++) {
                string _s = (o == size_l) ? "" : ",";
                string name_rand;

                for(int i = 0; i < 15; i++)
                    name_rand += chrs[uni(rng)];

                string ext;
//                string content_type = "image"; // image
                content_type = "2";

                if(FILES[o].type == "image/jpeg")
                    ext = ".jpg";
                if(!ext.size())
                    continue;

                string folder_content = "/content/img";
                string folder = "/sda1/www" + folder_content;
                string img_width;
                string img_height;

                string url_o = folder + string("/o/") + name_rand + ext;

                writeFile(url_o, FILES[o]);

                string obj = "{"
                    "\"s\":\"" +     folder_content + string("/s/") + name_rand + ext   +   "\","
                    "\"m\":\"" +     folder_content + string("/m/") + name_rand + ext   +   "\","
                    "\"mob\":\"" +   folder_content + string("/mob/") + name_rand + ext +   "\","
                    "\"o\":\"" +     folder_content + string("/o/") + name_rand + ext   +   "\","
                    "\"resol\":\"" + img_width + "x" + img_height + "\","
                    "\"type\":\"" +  ext + "\""
                "}";

                string post_id = model.getIdPost(user.id);

                if(post_id == "0")
                    post_id = model.reservePlace(user.id);

                json += "{" +
                    model.addContent(post_id, content_type, obj) + ","
                    "\"url\":\"" + folder_content + string("/s/") + name_rand + ext + "\""
                "}" + _s;
            }

            json += "]";

            return json;
        }

        string run(map<const char*, const char*, cmp_str>* parms) {
            string method = setvalc(parms, "REQUEST_METHOD");

            if(method != "POST")
                return "{\"status\":\"denied\"}";

            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

            string user_id = setval(&COOKIE, "id");

            if( strspn( user_id.c_str(), "0123456789" ) != user_id.size() )
                return "";

            string user_token = setval(&COOKIE, "token");

            if( strspn( user_token.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz" ) != user_token.size() )
                return "";

            UploadZIPModel model;
            User user;

            if(user_id.size() > 0 && user_token.size() > 0)
                user = model.getDataUser(user_id, user_token);
            else
                return "";

            string ajax_request = setvalc(parms, "HTTP_AJAX_REQUEST");

            if(ajax_request.size() > 0 && ajax_request == "xmlhttprequest") {
                return "{\"status\":\"allowed\"}";
            }

//            addHeader("Content-Type:application/json; charset=UTF-8");

            return getResForPOST(parms, model, user);
        }
};