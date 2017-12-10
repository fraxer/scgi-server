class AddItemModel : public Model {
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

        void reservePlace(const string& adder_id, const string& adder_type, const string& name, const string& desc, const string& desc_param, const string& price) {
            query("INSERT INTO `product` (`id`, `visibility`, `id_adder`, `type_adder`, `name`, `desc`, `desc_param`, `price`, `date`) VALUES (NULL, '1', '" + adder_id + "', '" + adder_type + "', '" + name + "', '" + desc + "', '" + desc_param + "', '" + price + "', CURRENT_TIMESTAMP)");

            string id = to_string(mysql_insert_id(mysql));

            mysql_free_result(res);

            query("INSERT INTO `product_content` (`id`, `visibility`, `type`, `id_adder`, `type_adder`, `id_parent`, `date`) "
                "VALUES "
                    "(NULL, '0', '0', '" + adder_id + "', '" + adder_type + "', '" + id + "', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + adder_id + "', '" + adder_type + "', '" + id + "', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + adder_id + "', '" + adder_type + "', '" + id + "', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + adder_id + "', '" + adder_type + "', '" + id + "', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + adder_id + "', '" + adder_type + "', '" + id + "', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + adder_id + "', '" + adder_type + "', '" + id + "', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + adder_id + "', '" + adder_type + "', '" + id + "', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + adder_id + "', '" + adder_type + "', '" + id + "', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + adder_id + "', '" + adder_type + "', '" + id + "', CURRENT_TIMESTAMP), "
                    "(NULL, '0', '0', '" + adder_id + "', '" + adder_type + "', '" + id + "', CURRENT_TIMESTAMP) "
            );

            mysql_free_result(res);
        }

        string& prepare(string& val) {
            val = urldecode(val);
            val = slashing(val);
            val = trim(val);
            return val;
        }

        void publishPost(
            string& name,
            string& desc,
            string& price,
            const string& compatibleBrowsers,
            const string& compatibleWith,
            const string& column,
            const string& layout,
            const string& adder_id,
            const string& adder_type,
            vector<ImageParams>& image_params
        ) {

            name = prepare(name);
            desc = prepare(desc);
            price = prepare(price);

            time_t t = time(NULL);
            char buffer [80];
            struct tm *tm = localtime(&t);
            strftime(buffer, 80, "%d %b %Y", tm);

            string d = buffer;
            string desc_param;

            desc_param += "\\\\n\\\\n";
            desc_param += "<a href=\"/\" target=\"_blank\">Живой просмотр</a>";
            desc_param += "\\\\n\\\\n";
            desc_param += "Опубликовано: " + d;
            desc_param += "\\\\n\\\\n";
            desc_param += "Поддерживаемые браузеры: " + compatibleBrowsers;
            desc_param += "\\\\n\\\\n";
            desc_param += "Используемые технологии: " + compatibleWith;
            desc_param += "\\\\n\\\\n";
            desc_param += "Колонок: " + column;
            desc_param += "\\\\n\\\\n";
            desc_param += "Тип верстки: " + layout;

            query(
                "SELECT "
                    "`id_parent_shop_content` "
                "FROM "
                    "`user_uploads` "
                "WHERE `id_user` = " + adder_id
            );

            row = mysql_fetch_row(res);

            string post_id = row[0];

            mysql_free_result(res);

            if(post_id == "0") {
                if(desc.size() == 0 || name.size() == 0|| price.size() == 0)
                    return;

                reservePlace(adder_id, adder_type, name, desc, desc_param, price);

                return;
            }

            query(
                "SELECT "
                    "`id`, "
                    "`param` "
                "FROM "
                    "`product_content` "
                "WHERE "
                    "`id_parent` = " + post_id + " AND `type` <> 0 "
//                "LIMIT 1"
            );

            int size = mysql_num_rows(res);

//            mysql_free_result(res);

            if(size <= 1) {
                mysql_free_result(res);

                query(
                    "UPDATE "
                        "`product` "
                    "SET "
                        "`name` = '" + name + "', "
                        "`desc` = '" + desc + "', "
                        "`desc_param` = '" + desc_param + "', "
                        "`price` = '" + price + "', "
                        "`visibility` = 1 "
                    "WHERE "
                        "`id` = " + post_id
                );
            }
            else {
                // На будущее обновлять пост, чтобы отображал несколько картинок

//                row = mysql_fetch_row(res);

                string id;
                string param;
                string _s;

                int size = mysql_num_rows(res);
                int size_l = size-1;
                int i = 0;

                string json = "[";

                while((row = mysql_fetch_row(res)) != NULL) {
                    _s = (i == size_l) ? "" : ",";

                    id    = row[0];
                    param = row[1];

                    map<string, string> m(deserializeObject(param, ""));

                    short pos = m["resol"].find("x");

                    ImageParams ip;
                                ip.url_m  = m["m"];
                                ip.url_s  = m["s"];
                                ip.width  = stoi(m["resol"].substr(0, pos));
                                ip.height = stoi(m["resol"].substr(pos+1));

                    image_params.push_back(ip);

                    m.erase("m");
                    m.erase("mob");
                    m.erase("o");
                    m.erase("type");

                    json += "{"
                        "\"id\":\"" + id + "\","
                        "\"s\":\"" + m["s"] + "\","
                        "\"resol\":\"" + m["resol"] + "\""
                    "}" + _s;

                    i++;
                }

                json += "]";

                query(
                    "UPDATE "
                        "`product` "
                    "SET "
                        "`name` = '" + name + "', "
                        "`desc` = '" + desc + "', "
                        "`desc_param` = '" + desc_param + "', "
                        "`price` = '" + price + "', "
                        "`param` = '" + json + "', "
                        "`visibility` = 1 "
                    "WHERE "
                        "`id` = " + post_id
                );
            }

            mysql_free_result(res);

            query(
                "UPDATE "
                    "`product_content` "
                "SET "
                    "`visibility` = 1 "
                "WHERE "
                    "`id_parent` = " + post_id + " AND "
                    "`type` <> 0"
            );

            mysql_free_result(res);

            query(
                "UPDATE "
                    "`user_uploads` "
                "SET "
                    "`id_parent_shop_content` = 0 "
                "WHERE "
                    "`id_user` = " + adder_id
            );

            mysql_free_result(res);
        }


};

class AddItemController: public Controller {
    public:
        string getResForGET(map<const char*, const char*, cmp_str>* parms) {

            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

            string user_id = setval(&COOKIE, "id");

            if( strspn( user_id.c_str(), "0123456789" ) != user_id.size() )
                user_id = "";

            string user_token = setval(&COOKIE, "token");

            if( strspn( user_token.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz" ) != user_token.size() )
                user_token = "";

            AddItemModel model;
            User user;

            if(user_id.size() > 0 && user_token.size() > 0)
            {
                user = model.getDataUser(user_id, user_token);
                if(user.id.size() == 0 || !hasRight(user.role, "addItem"))
                    return "{\"status\":\"0\"}";
            }
            else
                return "{\"status\":\"0\"}";

            string ajax_request = setvalc(parms, "HTTP_AJAX_REQUEST");

            if(ajax_request.size() > 0 && ajax_request == "xmlhttprequest") {
                

                return "{\"status\":\"1\"}";
            }

            return "{\"status\":\"0\"}";
        }

        string getResForPOST(map<const char*, const char*, cmp_str>* parms) {

            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

            string user_id = setval(&COOKIE, "id");

            if( strspn( user_id.c_str(), "0123456789" ) != user_id.size() )
                user_id = "";

            string user_token = setval(&COOKIE, "token");

            if( strspn( user_token.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz" ) != user_token.size() )
                user_token = "";

            AddItemModel model;
            User user;

            if(user_id.size() > 0 && user_token.size() > 0)
                user = model.getDataUser(user_id, user_token);
            else
                return "{\"status\":\"0\"}";

            if(!hasRight(user.role, "addItem"))
                return "{\"status\":\"0\"}";

            map<string, string> data = parseQuery(setvalc(parms, "DATA"));

            string adder_id = setval(&data, "ai");
            if( strspn( adder_id.c_str(), "0123456789" ) != adder_id.size() )
                adder_id = "";

            string adder_type = setval(&data, "at");
            if( strspn( adder_type.c_str(), "0123456789" ) != adder_type.size() )
                adder_type = "";

            if(adder_id.size() == 0 || adder_type.size() == 0)
                return "{\"status\":\"0\"}";

            if(adder_type == "0" && adder_id == user_id) {
                vector<ImageParams> image_params;

                string name                       = setval(&data, "name");
                string desc                       = setval(&data, "desc");
                string price                      = setval(&data, "price");
                vector<string> compatibleBrowsers = deserializeSimpleArray(setval(&data, "compatibleBrowsers"));
                vector<string> compatibleWith     = deserializeSimpleArray(setval(&data, "compatibleWith"));
                string column                     = setval(&data, "column");
                string layout                     = setval(&data, "layout");

                string sCompatibleBrowsers;
                string sCompatibleWith;

//                vector<string> mCompatibleBrowsers;
//                               mCompatibleBrowsers.push_back("IE6");
//                               mCompatibleBrowsers.push_back("IE7");
//                               mCompatibleBrowsers.push_back("IE8");
//                               mCompatibleBrowsers.push_back("IE9");
//                               mCompatibleBrowsers.push_back("IE10");
//                               mCompatibleBrowsers.push_back("IE11");
//                               mCompatibleBrowsers.push_back("EDGE");
//                               mCompatibleBrowsers.push_back("Firefox");
//                               mCompatibleBrowsers.push_back("Safari");
//                               mCompatibleBrowsers.push_back("Opera");
//                               mCompatibleBrowsers.push_back("Chrome");
//
//                short lmcb = mCompatibleBrowsers.size();
//                short lcb = compatibleBrowsers.size();
//
//                for(short i = 0; i < lmcb; i++) {
//                    for(short j = 0; j < lcb; j++) {
//                        if(stoi(compatibleBrowsers[j]) == i)
//                            sCompatibleBrowsers += (mCompatibleBrowsers[i] + ", ");
//                    }
//                }
//                if(sCompatibleBrowsers.size() > 0)
//                    sCompatibleBrowsers = sCompatibleBrowsers.substr(0, sCompatibleBrowsers.size()-2);


//                vector<string> mCompatibleWith;
//                               mCompatibleWith.push_back("N/A");
//                               mCompatibleWith.push_back("AngularJS 2");
//                               mCompatibleWith.push_back("AngularJS");
//                               mCompatibleWith.push_back("ReactJS");
//                               mCompatibleWith.push_back("Bootstrap 3.x");
//                               mCompatibleWith.push_back("Bootstrap 2.3.x");
//                               mCompatibleWith.push_back("Bootstrap 2.2.2");
//                               mCompatibleWith.push_back("Bootstrap 2.2.1");
//                               mCompatibleWith.push_back("Bootstrap 2.1.1");
//                               mCompatibleWith.push_back("Bootstrap 2.1.0");
//                               mCompatibleWith.push_back("Bootstrap 2.0.4");
//                               mCompatibleWith.push_back("Bootstrap 2.0.3");
//                               mCompatibleWith.push_back("Bootstrap 2.0.2");
//                               mCompatibleWith.push_back("Bootstrap 2.0.1");
//                               mCompatibleWith.push_back("Bootstrap 2.0");
//                               mCompatibleWith.push_back("Foundation 6");
//                               mCompatibleWith.push_back("Foundation 5");
//                               mCompatibleWith.push_back("Foundation 4");
//
//                short lmcw = mCompatibleWith.size();
//                short lcw = compatibleWith.size();
//
//                for(short i = 0; i < lmcw; i++) {
//                    for(short j = 0; j < lcw; j++) {
//                        if(stoi(compatibleWith[j]) == i)
//                            sCompatibleWith += (mCompatibleWith[i] + ", ");
//                    }
//                }
//                if(sCompatibleWith.size() > 0)
//                    sCompatibleWith = sCompatibleWith.substr(0, sCompatibleWith.size()-2);


                model.publishPost(
                    name,
                    desc,
                    price,
                    sCompatibleBrowsers,
                    sCompatibleWith,
                    column,
                    layout,
                    adder_id,
                    adder_type,
                    image_params
                );

                buildGrid(image_params);
            }

            return "";
        }

        vector<bool> getSequenceOrientationImages(const vector<ImageParams>& image_params) {
            vector<bool> seq;
            short size = image_params.size();
            bool all_zero = true;
            bool all_one = true;

            for(short i = 0; i < size; i++) {
                if(image_params[i].width >= image_params[i].height) {
                    all_one = false;
                }
                if(image_params[i].width < image_params[i].height) {
                    all_zero = false;
                }
            }

            seq.push_back(all_one);
            seq.push_back(all_zero);

            return seq;
        }

        void cell(short start, short count, const vector<ImageParams>& image_params) {
            short blockWidth = 518;
            short tWidth = 0;
            int tHeight = 170;
            short count_l = count - 1;

//            Writer ww;

            for(short p = start; p < count; p++) {
                blockWidth -= 6;
                tWidth += round((float)(tHeight * image_params[p].width) / (float)image_params[p].height);

//                ww.writeInLog("imwidth: " +  to_string(image_params[p].width) + ", imheight: " + to_string(image_params[p].height) );

//                ww.writeInLog("round_" + to_string(p) + ": " + to_string(round((float)(tHeight * image_params[p].width) / (float)image_params[p].height)) );
            }

//            ww.writeInLog("tWidth: " + to_string(tWidth) );

            float k = (float)blockWidth / (float)tWidth;
            short imWidth = blockWidth;


//            ww.writeInLog("tHeight: " + to_string(tHeight) + ", k: " + to_string(k) + ", floor: " + to_string(floor(tHeight * k)));

            tHeight = floor(tHeight * k);

            for(short i = start; i < count; i++) {
                int _w = round((float)(tHeight * image_params[i].width) / (float)image_params[i].height);

                resizeImage(image_params[i].url_m, image_params[i].url_s, _w, tHeight);

                if(i != count_l)
                    imWidth -= _w;
            }

            resizeImage(image_params[count_l].url_s, image_params[count_l].url_s, imWidth, tHeight);
        }

        void resizeImage(const string &ipath, const string &opath, int w, int h) {
            // получаем картинку

            string folder = "/sda1/www";

            string inf  = folder + ipath;
            string outf = folder + opath;

            IplImage* image = cvLoadImage(inf.c_str(), 1);

            int width = w;
            int height = h;

            IplImage *destination = cvCreateImage( cvSize(width , height), image->depth, image->nChannels);
            cvResize(image, destination, cv::INTER_AREA);
            cvSaveImage(outf.c_str(), destination);
            cvReleaseImage(&image);
            cvReleaseImage(&destination);
        }

        void buildGrid(const vector<ImageParams>& image_params) {
            vector<bool> array = getSequenceOrientationImages(image_params);
            short count = image_params.size();

            if(count == 2 || count == 3) {
                cell(0, count, image_params);
            }
            if(count == 4) {
                if(array[1]) {
                    cell(0, 1, image_params);
                    cell(1, count, image_params);
                }
                if(array[0]) {
                    cell(0, count, image_params);
                }
                if(!array[0] && !array[1]) {
                    cell(0, 2, image_params);
                    cell(2, count, image_params);
                }
            }
            if(count == 5) {
                if((!array[0] && !array[1]) || array[1]) {
                    cell(0, 2, image_params);
                    cell(2, count, image_params);
                }
                if(array[0]) {
                    cell(0, count, image_params);
                }
            }
            if(count == 6) {
                cell(0, 3, image_params);
                cell(3, count, image_params);
            }
            if(count > 6 && count < 10) {
                cell(0, 3, image_params);
                cell(3, count, image_params);
            }
            if(count == 10) {
                if(!array[1]) {
                    cell(0, 4, image_params);
                    cell(4, count, image_params);
                }
                if(array[1]) {
                    cell(0, 3, image_params);
                    cell(3, 6, image_params);
                    cell(6, count, image_params);
                }
            }

        }

        string run(map<const char*, const char*, cmp_str>* parms) {
            string method = setvalc(parms, "REQUEST_METHOD");

            if(method == "GET")
                return getResForGET(parms);

            if(method == "POST")
                return getResForPOST(parms);

            return "{\"status\":\"0\"}";
        }
};