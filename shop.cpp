class ShopView : public View {
    public:
        void setAside(map<string, string>& allCategory, vector<int>& v, map<string, string>& parameters, map<string, map<string, string>> values_parameters) {
            string categories = "";

            for(map<string, string>::iterator it = allCategory.begin(); it != allCategory.end(); ++it)
                categories += "<a href=\"/shop\" class=\"abl\" data-cat=\"" + it->second + "\" onclick=\"Model.filterData={category:this.dataset.cat};Controller.filter(event);return false;\">" + it->first + "</a>";

            string custom_blocks = "";
            map<string, string>::iterator it, itt;

            for(it = parameters.begin(); it != parameters.end(); ++it) {
                custom_blocks +=
                    "<div class=\"block custom-param\">"
                        "<span class=\"title\">" + it->first + "</span>";

                if(values_parameters[it->second].size() > 0 ) {
                    for(itt = values_parameters[it->second].begin(); itt != values_parameters[it->second].end(); ++itt) {
                        custom_blocks +=
                            "<div>"
                                "<label class=\"checkbox\" for=\"" + it->second + "_" + itt->second + "\">"
                                    "<input type=\"checkbox\" id=\"" + it->second + "_" + itt->second + "\" value=\"" + itt->second + "\" data-param=\"" + it->second + "\" onchange=\"Controller.checkbox(this);\">"
                                    "<span></span>"
                                    "<span>" + itt->first + "</span>"
                                "</label>"
                            "</div>";
                    }
                }

                custom_blocks +=
                    "</div>";
            }

            aside =
                "<aside id=\"aside\" style=\"display:table-cell;top:42px;width:208px;\">"
                    "<div class=\"a-wrap\" style=\"width:208px;padding:8px 0 0 8px;overflow:hidden;height:100%;\">"
                        "<div class=\"block\">"
                            "<span class=\"title\">Категории</span>"
                            + categories +
                        "</div>"
                        "<div class=\"block\">"
                            "<span class=\"title\">Цена</span>"
                            "<div class=\"inp l\">"
                                "<span>От</span>"
                                "<input type=\"text\" id=\"s-inp1\" value=\"" + to_string(v[0]) + "\">"
                            "</div>"

                            "<div class=\"inp r\">"
                                "<span>До</span>"
                                "<input type=\"text\" id=\"s-inp2\" value=\"" + to_string(v[1]) + "\">"
                            "</div>"
                            "<div class=\"clear\"></div>"
                            "<div id=\"slider\">"
                                "<div id=\"s-line\"></div>"
                                "<div id=\"s-cl\" onmousedown=\"View.priceSlider.dragButton(this, event);\"></div>"
                                "<div id=\"s-cr\" onmousedown=\"View.priceSlider.dragButton(this, event);\"></div>"
                            "</div>"
                        "</div>"
                        + custom_blocks +
                    "</div>"
                "</aside>";
        };

        void getContent(vector<Data> arr) {
            unsigned short size = arr.size();
            bool user_is_set = user.id.size() > 0;
            unsigned short count_comments = 0;
            bool short_text = false;

            content += "<main id=\"block-content\" style=\"display:table-cell;visibility: hidden;\">";

            for(unsigned short i = 0; i < size; i++)
            {
                short_text = (amountSpacesAndEnters((char*)arr[i].desc.c_str(), 60) > 60) ? true : false;

                count_comments = atoi(arr[i].amount_comments.c_str());

                content +=
                "<article class=\"content c shop\">"
                    +
                    getPreview(arr[i])
                    +
                    "<div class=\"post-text\"" + (short_text ? " style=\"height:62px\"" : string("")) + ">"
                        "<div class=\"wr-name\">"
                            "<div class=\"price\" onclick=\"Controller.addToCart('" + arr[i].name + "'," + arr[i].id + "," + arr[i].price + ");\" title=\"Добавить товар в корзину\"><span class=\"cy\">$</span>" + arr[i].price + "</div>"
                            "<h2 class=\"name\">" + arr[i].name + "</h2>"
                        "</div>"
                        "<div>"
                            "by"
                            "<a href=\"/i" + arr[i].adder_id + "\" class=\"al m-b\" onclick=\"Controller.openNewPage(this, event, 'mypage');\">"
                                "<span>" + arr[i].adder_name + "</span>"
                            "</a>"
                        "</div>"
                        + arr[i].desc +
                    "</div>"
                    +
                    (
                        short_text ?
                            "<div class=\"toggle-text\" data-state=\"0\" onclick=\"Controller.togglePostText(this, 62);\">Показать полностью</div>" :
                            ""
                    )
                    +
                    "<div class=\"bottom-nav\">"
                        // "<div class=\"comments\" " + ( (count_comments > 0 || user_is_set) ? "onclick=\"Controller.listComments(" + arr[i].id + ", " + (user_is_set ? "true" : "false") + ", this);\"" : "" ) + ">"
                        //     "<span>"
                            // +
                            // (
                            //     (count_comments > 0) ? getWord(arr[i].amount_comments) : (user_is_set ? "Добавить комментарий" : "Нет комментариев")
                            // )
                            // +
                        //     "</span>"
                        // "</div>"
                        // +
                        // (
                        //     user_is_set ?
                        //     "<div class=\"like l-" + (string)(arr[i].like.size() > 0 && arr[i].like == "1" ? "on" : "off") + "\" onclick=\"Controller.like(" + arr[i].id + ", this);\">"
                        //         "<span>" + (atoi(arr[i].amount_like.c_str()) > 0 ? arr[i].amount_like : "") + "</span>"
                        //     "</div>" : ""
                        // )
                        // +
                        "<div class=\"block-comments\">"
                            "<div class=\"list-comments\"></div>"
                        "</div>"
                    "</div>"
                "</article>";
            }

            content += "</main>";
        }

        const string getPreview(const Data& d) {
            if(d.param.length() == 0)
                return "";

            if(d.param[0] == '[')
            {
                vector< map<string, string> > arr(deserializeArray(d.param));

                short size = arr.size();

                if(size == 0)
                    return "";
                if(size == 1) {
                    short pos = arr[0]["resol"].find("x");
                    return
                        "<div class=\"block-image\" onclick=\"Controller.openViewerPost(" + d.id + ", 2, 'viewpost');\">"
                            "<img src=\"" + arr[0]["s"] +  "\" alt=\"post\" data-w=\"" + arr[0]["resol"].substr(0, pos) + "\" data-h=\"" + arr[0]["resol"].substr(pos+1) + "\">"
                        "</div>";
                }

                string block = "<div class=\"block-image multi\">";

                for(short i = 0; i < size; i++) {
                    short pos = arr[i]["resol"].find("x");
                    block += "<img src=\"" + arr[i]["s"] +  "\" data-w=\"" + arr[i]["resol"].substr(0, pos) + "\" data-h=\"" + arr[i]["resol"].substr(pos+1) + "\" onclick=\"Controller.openViewerPost(" + d.id + ", 3, 'viewcontent', " + to_string(i) + ");\" alt=\"content\">";
                }

                block += "</div>";

                return block;
            }

            if(d.param[0] == '{')
            {
                map<string, string> m(deserializeObject(d.param, ""));

                if(m["s"].length() > 0) {
                    short pos = m["resol"].find("x");
                    return
                        "<div class=\"block-image\" onclick=\"Controller.openViewerPost(" + d.id + ", 2, 'viewpost');\">"
                            "<img src=\"" + m["s"] +  "\" alt=\"post\" data-w=\"" +m["resol"].substr(0, pos) + "\" data-h=\"" + m["resol"].substr(pos+1) + "\">"
                        "</div>"
                    ;
                }
            }

            return "";
        }

        void setWrapper() {
            wrapper = "<div id=\"wrapper\" style=\"display:table;max-width:1200px;width:100%;min-width:720px;margin:0px auto;\">" + aside + content + "</div>";
        }
};

class ShopModel : public Model {
    public:
        void getData(const string& start, const User& user, const string& price, const string& category, map<string, string>& GET) {
            string str;
            string str_price;
            string str_query;
            string str_ids;
            bool user_is_set = user.id.size() > 0;
            bool is_set_query = false;

            if(user_is_set) {
                str = ", (SELECT COUNT(l.id_content) FROM `like` l WHERE l.id_user = " + user.id + " AND l.id_content = c.id AND l.is_like = 1 AND l.type_content = 2 ) as `like` ";
            }

            if(price.size() > 0) {
                short pos = price.find(",");

                if(pos != string::npos) {
                    string pr1 = price.substr(0, pos);

                    if(pr1.size() == 0)
                        pr1 = "0";

                    string pr2 = price.substr(pos+1, price.size());

                    if(pr2.size() == 0)
                        pr2 = "1000";

                    str_price = "AND c.`price` >= " + pr1 + " AND c.`price` <= " + pr2 + " ";
                }
            }

            str_query =
                "SELECT "
                    "`product_id` "
                "FROM "
                    "`parameters_product` `p1` "
                "USE INDEX(`uniq`) "
                "WHERE ";

            // Проверка параметров на наличие в базе
            // Проверка значений на корректность и уникальность
            // Создание sql запроса для выборки id-ков товаров(постов)
            for(map<string, string>::iterator it = GET.begin(); it != GET.end(); ++it) {
                if( strspn( it->first.c_str(), "0123456789" ) == it->first.size() && it->second.size() > 0 && paramExist(it->first) ) {

                    string _str = it->second;
                    vector<string> v;

                    while(_str.length() > 0) {
                        short pos = _str.find(",");
                        string sub;

                        if(pos > -1) {
                            sub = _str.substr(0, pos);
                            _str.replace(0, sub.length()+1, "");
                        }
                        else {
                            pos = _str.length();
                            sub = _str.substr(0, pos);
                            _str.replace(0, sub.length(), "");
                        }
                        if(sub.length() > 0 && strspn( sub.c_str(), "0123456789" ) == sub.size() && find(v.begin(), v.end(), sub) == v.end()) {
                            v.push_back(sub);
                            is_set_query = true;
                            str_query +=
                                "(" + it->first + "," + sub + ") IN ( SELECT  p2.`parameter_id`,p2.`parameters_value_id` FROM `parameters_product` p2 "
                                     "WHERE p2.`product_id` = p1.`product_id` AND p2.`parameter_id` = " + it->first + " AND p2.`parameters_value_id` = " + sub + ") AND ";
                        }
                    }
                }
            }


            if(is_set_query) {
                str_query = str_query.substr(0, str_query.size()-4);
                str_query += "UNION SELECT 0";
            }
            else {
                str_query = "";
            }

            string join;

            if(str_query.size() > 0) {
                join = "JOIN (" + str_query + ") as pp ";
            }

            string where_join;
            string order_by = "c.id DESC ";

            if(join.size() > 0) {
                where_join = " AND c.id IN(pp.`product_id`) ";
                order_by = "null ";
            }

            query(
                "SELECT SQL_CACHE "
                    "c.`id`, "
                    "c.`param`, "
                    "c.`name`, "
                    "c.`desc`, "
                    "c.`price`, "
                    "IF(c.type_adder = 0, u.id, cm.id) as id, "
                    "IF(c.type_adder = 0, u.login, cm.name) as name, "
                    "IF(c.type_adder = 0, u.avatar, cm.avatar) as avatar, "
                    "(SELECT COUNT(cm.id) FROM comment cm WHERE c.id = cm.id_content AND cm.type_content = 2 ) as amount_comments, "
                    "(SELECT COUNT(l.id_content) FROM `like` l WHERE l.id_content = c.id AND l.is_like = 1 AND l.type_content = 2 ) as `amount_like` "
                 + str +
                "FROM "
                    "product c "
                "LEFT JOIN "
                   "`user` u "
                "ON "
                    "c.id_adder = u.id "
                "LEFT JOIN "
                   "`community` cm "
                "ON "
                    "c.id_adder = cm.id "
                + join +
                "WHERE "
                    "c.visibility = 1 AND "
                    "c.category_id = " + category + " "
                    +
                    str_price
                    +
                    where_join
                    +
                "ORDER BY " + order_by +
                "LIMIT " + start + ", 20"
            );
        };

        vector<int> getMinMaxPrice(const string& category) {
            query("SELECT SQL_CACHE MIN(price), MAX(price) FROM `product` WHERE category_id = " + category);

            vector<int> dbl;

            if(res != NULL && (row = mysql_fetch_row(res)) != NULL) {
                if(row[0] != NULL) {
                    dbl.push_back(atoi(row[0]));
                    dbl.push_back(atoi(row[1]));
                }
            }

            if(dbl.size() == 0) {
                dbl.push_back(0);
                dbl.push_back(1000);
            }

            mysql_free_result(res);

            return dbl;
        }

        map<string, string> getAllCategory() {
            query("SELECT SQL_CACHE `name`, `id` FROM `product_categories` ORDER BY `name` ASC");

            map<string, string> arr;

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
                arr[(string)row[0]] = (string)row[1];

            mysql_free_result(res);

            return arr;
        }

        bool paramExist(const string& id) {
            query("SELECT SQL_CACHE `id` FROM `parameters` WHERE `id` = '" + id + "'");

            if(res != NULL && mysql_num_rows(res) > 0) {
                mysql_free_result(res);
                return true;
            }

            mysql_free_result(res);
            return false;
        }

        map<string, string> getParameters(const string& category) {
            // выборка всех основных параметров
//            query("SELECT SQL_CACHE `name`, `id` FROM `parameters` WHERE `category_id` = " + category + " ORDER BY `name` ASC");

            query(
                "SELECT SQL_CACHE "
                	"`p`.`name`, "
                    "`p`.`id` "
                "FROM "
                	"`parameters` `p` "
                "JOIN ( "
                    "SELECT `parameter_id` FROM `parameters_product` GROUP BY `parameter_id` "
                ") as `pp` "
                "WHERE "
                	"`p`.`category_id` = " + category + " AND `p`.`id` IN(`pp`.`parameter_id`) "
            );

            map<string, string> arr;

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
                arr[(string)row[0]] = (string)row[1];

            mysql_free_result(res);

            return arr;
        }

        map<string, map<string, string>> getValuesOfParameters(const string& parameters) {
            // выводятся все параметры
//            query(
//                "SELECT SQL_CACHE "
//                   	"`id`, "
//                   	"`value`, "
//                   	"`parameter_id` "
//                "FROM "
//                    "`parameters_value` "
//                "WHERE "
//                    "`parameter_id` IN(" + parameters + ")"
//                "ORDER BY `parameter_id` ASC, `value` DESC"
//            );

            query(
                "SELECT SQL_CACHE "
                	"`pv`.`id`, "
                	"`pv`.`value`, "
                	"`pv`.`parameter_id` "
                "FROM "
                	"`parameters_value` as `pv` "
                "JOIN ( "
                    "SELECT `parameter_id`, `parameters_value_id` FROM `parameters_product` GROUP BY `parameter_id`, `parameters_value_id` "
                ") as `pp` "
                "WHERE "
                	"`pv`.`parameter_id` IN(`pp`.`parameter_id`) AND `pv`.`id` IN(`pp`.`parameters_value_id`) "
                //"ORDER BY `pv`.`parameter_id` ASC, `pv`.`value` DESC"
            );

            map<string, map<string, string>> arr;
            map<string, map<string, string>>::iterator it;

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL) {
                it = arr.find((string)row[2]);
                if (it != arr.end()) {
                    arr[(string)row[2]][(string)row[1]] = (string)row[0];
                } else {
                    map<string, string> ar;
                    ar[(string)row[1]] = (string)row[0];

                    arr[(string)row[2]] = ar;
                }
            }

            mysql_free_result(res);

            return arr;
        }

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

        vector<Data> getDataIndex(const string& start, const User& user, const string& price, const string& category, map<string, string>& GET) {

            vector<Data> arr;
            getData(start, user, price, category, GET);
            bool user_is_set = user.id.size() > 0;
            string name;
            string desc;

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                name = row[2];
                desc = row[3];

                Data d;
                     d.id = row[0];
                     d.param = row[1];
                     d.name = clearSlashing(name);
                     d.desc = clearSlashing(desc);
                     d.price = row[4];
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

        string getJSON(const string& start, const User& user, const string& price, const string& category, map<string, string>& GET, const bool& canAddItem) {
            getData(start, user, price, category, GET);

            bool user_is_set = user.id.size() > 0;

            short size = mysql_num_rows(res);
            short size_l = size-1;
            short i = 0;
            string desc;

            string str = "[";

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                desc = row[3];

                str += "{"
                    "\"id\":" + (string)row[0] + ","
                    "\"param\":" + (strlen(row[1]) > 0 ? (string)row[1] : "\"\"") + ","
                    "\"name\":\"" + (string)row[2] + "\","
                    "\"desc\":\"" + back_slash(desc) + "\","
                    "\"price\":\"" + (string)row[4] + "\","
                    "\"adder_id\":\"" + (string)row[5] + "\","
                    "\"adder_name\":\"" + (string)row[6] + "\","
                    "\"adder_avatar\":\"" + (string)row[7] + "\","
                    "\"amount_comments\":" + (string)row[8] + ","
                    "\"amount_like\":" + (string)row[9] + ","
                    "\"like\":" + (user_is_set ? row[10] : "0") + ""
                "}" + ((i++ == size_l) ? "" : ",");
            }

            mysql_free_result(res);

            str += "]";



            map<string, string> allCategory = getAllCategory();

            string cats = "[";

            for(map<string, string>::iterator it = allCategory.begin(); it != allCategory.end(); ++it)
                cats += "[\"" + it->first + "\"," + it->second + "],";

            cats = cats.substr(0, cats.size()-1);

            cats += "]";



            map<string, string> parameters = getParameters(category);
            map<string, map<string, string>> values_parameters;

            string str_params = "";

            for(map<string, string>::iterator it = parameters.begin(); it != parameters.end(); ++it)
                str_params += it->second + ",";

            if(str_params.size() > 0) {
                str_params = str_params.substr(0, str_params.size()-1);

                values_parameters = getValuesOfParameters(str_params);
            }


            map<string, string>::iterator it, itt;

            string params = "[";

            for(it = parameters.begin(); it != parameters.end(); ++it) {

                string _values = "[";

                if(!values_parameters[it->second].empty()) {

                    for(itt = values_parameters[it->second].begin(); itt != values_parameters[it->second].end(); ++itt) {
                        _values += "[\"" + itt->first + "\"," + itt->second + "],";
                    }
                }

                if(_values[_values.size()-1] == ',')
                    _values = _values.substr(0, _values.size()-1);

                _values += "]";

                params += "[\"" + it->first + "\"," + it->second + "," + _values + "],";
            }

            if(params[params.size()-1] == ',')
                params = params.substr(0, params.size()-1);

            params += "]";

            vector<int> v = getMinMaxPrice(category);

            string obj =
            "{"
                "\"data\":" + str + ","
                "\"cats\":" + cats + ","
                "\"params\":" + params + ","
                "\"prices\":[" + to_string(v[0]) + "," + to_string(v[1]) + "],"
                "\"user\":" + (user_is_set ? "" + user.id + "" : "\"0\"") + ","
                "\"right\":[" + (canAddItem ? "\"addItem\"" : "") + "]" +
            "}";

            return obj;
        };
};

class ShopController: public Controller {
    public:
        ShopController() {
            fillRights();
        }

        string run(map<const char*, const char*, cmp_str>* parms) {
            if(strcmp(setvalc(parms, "REQUEST_METHOD"), "GET") != 0)
                return "";

            addHeader("Content-Type:text/html; charset=UTF-8");

            ShopModel model;
            ShopView  view;
            User      user;

            map<string, string> GET = parseQuery(setvalc(parms, "QUERY_STRING"));
            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

            string start = setval(&GET, "start");

            if( !start.size() || strspn( start.c_str(), "0123456789" ) != start.size() )
                start = "0";

            string price = setval(&GET, "price");
            if( strspn( price.c_str(), "0123456789," ) != price.size() )
                price = "";

            map<string, string> allCategory = model.getAllCategory();
            map<string, string>::iterator it;

            string category = setval(&GET, "category");
            if( !category.size() || strspn( category.c_str(), "0123456789" ) != category.size() ) {
                it = allCategory.begin();
                if(it != allCategory.end())
                    category = it->second;
            }

            string user_id = setval(&COOKIE, "id");
            if( strspn(user_id.c_str(), "0123456789") != user_id.size() )
                user_id = "";

            string user_token = setval(&COOKIE, "token");
            if( strspn(user_token.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz") != user_token.size() )
                user_token = "";

            // TODO fix user access
            if(user_id.size() > 0 && user_token.size() > 0) {
                user = model.getDataUser(user_id, user_token);
                view.setUser(user);
            }

            string ajax_request = setvalc(parms, "HTTP_AJAX_REQUEST");

            bool canAddItem = hasRight(user.role, "addItem");

            if(ajax_request.size() > 0 && ajax_request == "xmlhttprequest") {
                return model.getJSON(start, user, price, category, GET, canAddItem);
            }


            vector<int> minMaxPrice = model.getMinMaxPrice(category);
            map<string, string> parameters = model.getParameters(category);
            map<string, map<string, string>> values_parameters;

            string str_params = "";

            for(it = parameters.begin(); it != parameters.end(); ++it)
                str_params += it->second + ",";

            if(str_params.size() > 0) {
                str_params = str_params.substr(0, str_params.size()-1);

                values_parameters = model.getValuesOfParameters(str_params);
            }

            view.setTitle("Прототипы");
            view.setDescription("Купить html шаблоны, psd макеты.");
            view.setAside(allCategory, minMaxPrice, parameters, values_parameters);
            view.getContent(model.getDataIndex(start, user, price, category, GET));
            view.setRights(right);

            return view.templ();
        }
};