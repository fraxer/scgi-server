class CartView : public View {
    public:
        void setAside() {
            aside =
                "<aside id=\"aside\" style=\"display:none\">"
                    "<div class=\"a-wrap\"></div>"
                "</aside>";
        }

        void getContent(const vector<Data>& arr, const string& total_cost) {
            unsigned short size = arr.size();
//            bool user_is_set = user.id.size() > 0;
            string oclk;
            string imgsrc;

            if(size == 0) {
                content =
                    "<main id=\"block-content\" style=\"width:520px\">"
                        "<div class=\"order\">"
                            "<div class=\"order__total\">"
                                "<div class=\"total__text\">Корзина пуста</div>"
                                "<div class=\"total__cost rouble\">" + total_cost + "</div>"
                            "</div>"
                            "<a href=\"/shop\" class=\"order__send\" onclick=\"Controller.openNewPage(this, event, 'shop');\">Добавить товары</a>"
                        "</div>"
                    "</main>";
                return;
            }

            content =
                "<main id=\"block-content\" style=\"width:100%;padding:0;\">"
                    "<div class=\"cart\">";

            for(unsigned short i = 0; i < size; i++) {
                content +=
                    "<article class=\"content card-cart\">"
                        "<button class=\"card-cart-del\" onclick=\"Controller.removeFromCart(this, " + arr[i].id + ");\" title=\"Убрать из корзины\"></button>"
                        "<div class=\"block-image\">";

                if(arr[i].param[0] == '[') {
                    vector< map<string, string> > a(deserializeArray(arr[i].param));

                    oclk = "onclick=\"Controller.openViewerPost(" + a[0]["id"] + ", 3, 'viewcontent', 0);\"";
                    imgsrc = (a.size() == 0) ? "" : a[0]["s"];
                    short pos = a[0]["resol"].find("x");
                    content +=
                            "<img src=\"" + imgsrc + "\" class=\"card-cart-preview\" " + oclk + " data-w=\"" + a[0]["resol"].substr(0, pos) + "\" data-h=\"" + a[0]["resol"].substr(pos+1) + "\">";
                }

                if(arr[i].param[0] == '{') {
                    map<string, string> m(deserializeObject(arr[i].param, ""));

                    oclk = "onclick=\"Controller.openViewerPost(" + arr[i].id + ", 2, 'viewpost');\"";
                    imgsrc = m["s"];
                    short pos = m["resol"].find("x");
                    content +=
                            "<img src=\"" + imgsrc + "\" class=\"card-cart-preview\" " + oclk + " data-w=\"" +m["resol"].substr(0, pos) + "\" data-h=\"" + m["resol"].substr(pos+1) + "\">";
                }



                content +=
                        "</div>"
                        "<span class=\"card-cart-name\" " + oclk + ">" + arr[i].name + "</span>"
                        "<span class=\"card-cart-creator\">item by <a href=\"/i" + arr[i].adder_id + "\" class=\"card-cart-link\" onclick=\"Controller.openNewPage(this, event, \'i\');\">" + arr[i].adder_name + "</a></span>"
                        "<span class=\"card-cart-price\">" + arr[i].price + "<span class=\"rouble\"></span></span>"
                    "</article>";
            }

            content +=
                    "</div>"
                    "<hr class=\"order__horizontal-line\">"
                    "<div class=\"order\">"
                        "<p class=\"h2-c\">Оформление заказа</p>"
                        "<label class=\"order__label col-1-2 fl-l\">"
                            "<span class=\"order__name\">Имя</span>"
                            "<input class=\"order__input\" type=\"text\" name=\"name\">"
                            "<span class=\"order__error\"></span>"
                        "</label>"
                        "<label class=\"order__label col-1-2 fl-r\">"
                            "<span class=\"order__name\">E-mail</span>"
                            "<input class=\"order__input\" type=\"email\" name=\"email\">"
                            "<span class=\"order__error\"></span>"
                        "</label>"
                        "<div class=\"clr\"></div>"
                        "<label class=\"order__label\">"
                            "<span class=\"order__name\">Комментарий</span>"
                            "<textarea class=\"order__input order__textarea\" name=\"comment\"></textarea>"
                            "<span class=\"order__error\"></span>"
                        "</label>"
                        "<label class=\"order__label col-1-2 fl-l\">"
                            "<span class=\"order__name\">Способ оплаты</span>"
                            "<select class=\"order__input order__select\" name=\"payment\">"
                                "<option>Способ 1</option>"
                                "<option>Способ 2</option>"
                                "<option>Способ 3</option>"
                                "<option>Способ 4</option>"
                            "</select>"
                            "<span class=\"order__error\"></span>"
                        "</label>"
                        "<div class=\"clr\"></div>"
                        "<div class=\"order__total\">"
                            "<div class=\"total__text\">Итого</div>"
                            "<div class=\"total__cost rouble\">" + total_cost + "</div>"
                        "</div>"
                        "<button type=\"button\" class=\"order__send\" onclick=\"Controller.orderSend(this)\">Оформить заказ</button>"
                    "</div>"
                "</main>";
        }

        void setWrapper() {
            wrapper = "<div id=\"wrapper\" style=\"display:table;max-width:100%;width:100%;min-width:720px;margin:0px auto;\">" + aside + content + "</div>";
        }
};

class CartModel : public Model {
    private:
        double totalCost;
    public:
        CartModel() {
            totalCost = 0;
        }

        void getData(const string& ids, const User& user) {
            string str;
            bool user_is_set = user.id.size() > 0;

            query(
                "SELECT SQL_CACHE "
                    "c.id, "
                    "c.param, "
                    "c.`name`, "
                    "c.`price`, "
                    "IF(c.type_adder = 0, u.id, cm.id) as id, "
                    "IF(c.type_adder = 0, u.login, cm.name) as name, "
                    "IF(c.type_adder = 0, u.avatar, cm.avatar) as avatar "
                "FROM "
                    "`product` c "
                "USE INDEX(PRIMARY) "
                "LEFT JOIN "
                   "`user` u "
                "ON "
                    "c.id_adder = u.id "
                "LEFT JOIN "
                   "`community` cm "
                "ON "
                    "c.id_adder = cm.id "
                "WHERE "
                    "c.visibility = 1 AND "
                    "c.id IN(" + ids + ") "
                "ORDER BY c.id DESC "
            );
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

        vector<Data> getDataIndex(const string& ids, const User& user) {

            vector<Data> arr;
            getData(ids, user);
            bool user_is_set = user.id.size() > 0;
            locale custom_locale("C");
            string name;
            double d = 0;

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                name = row[2];

                stringstream buff;
                buff.imbue(custom_locale);
                buff.precision(2);
                buff.setf( ios::fixed, ios::floatfield );
                buff << row[3];
                buff >> d;

                totalCost += d;

                Data d;
                     d.id = row[0];
                     d.param = row[1];
                     d.name = clearSlashing(name);
                     d.price = row[3];
                     d.adder_id = row[4];
                     d.adder_name = row[5];
                     d.adder_avatar = row[6];

                arr.push_back(d);
            }

            mysql_free_result(res);

            return arr;

        }

        string getJSON(const string& ids, const User& user) {
            getData(ids, user);

            bool user_is_set = user.id.size() > 0;
            locale custom_locale("C");
            short size = mysql_num_rows(res);
            short size_l = size-1;
            short i = 0;
            string name;
            double d = 0;

            string str = "[";

            while(res != NULL && (row = mysql_fetch_row(res)) != NULL)
            {
                name = row[2];

                stringstream buff;
                buff.imbue(custom_locale);
                buff.precision(2);
                buff.setf( ios::fixed, ios::floatfield );
                buff << row[3];
                buff >> d;

                totalCost += d;

                str += "{"
                    "\"id\":" + (string)row[0] + ","
                    "\"param\":" + (strlen(row[1]) > 0 ? (string)row[1] : "\"\"") + ","
                    "\"name\":\"" + back_slash(name) + "\","
                    "\"price\":\"" + (string)row[3] + "\","
                    "\"adder_id\":\"" + (string)row[4] + "\","
                    "\"adder_name\":\"" + (string)row[5] + "\","
                    "\"adder_avatar\":\"" + (string)row[6] + "\""
                "}" + ((i++ == size_l) ? "" : ",");


            }

            mysql_free_result(res);

            str += "]";

            stringstream buff;
            buff.imbue(custom_locale);
            buff.precision(20);
            buff.setf( ios::fixed, ios::floatfield );
            buff << totalCost;

            string s_total_cost = buff.str();

            short index = s_total_cost.find(".", 0);

            if(index != string::npos)
                s_total_cost = s_total_cost.substr(0, index+3);

            string obj =
            "{"
                "\"data\":" + str + ","
                "\"user\":" + (user_is_set ? "" + user.id + "" : "\"0\"") + ","
                "\"totalCost\":\"" + s_total_cost + "\""
            "}";

            return obj;
        }

        string getTotalCost() {
            locale custom_locale("C");
            stringstream buff;
            buff.imbue(custom_locale);
            buff.precision(2);
            buff.setf( ios::fixed, ios::floatfield );
            buff << totalCost;

            string s_total_cost = buff.str();

            short index = s_total_cost.find(".", 0);

            if(index != string::npos)
                s_total_cost = s_total_cost.substr(0, index+3);

            return s_total_cost;
        }
};

class CartController: public Controller {
    public:
        string run(map<const char*, const char*, cmp_str>* parms) {
            if(strcmp(setvalc(parms, "REQUEST_METHOD"), "GET") != 0)
                return "";

            addHeader("Content-Type:text/html; charset=UTF-8");

            map<string, string> COOKIE = parseCookie(setvalc(parms, "HTTP_COOKIE"));

            string user_id = setval(&COOKIE, "id");
            if( strspn(user_id.c_str(), "0123456789") != user_id.size() )
                user_id = "";

            string user_token = setval(&COOKIE, "token");
            if( strspn(user_token.c_str(), "0123456789abcdefghijklmnopqrstuvwxyz") != user_token.size() )
                user_token = "";

            string ids = setval(&COOKIE, "i");
            if( ids.size() == 0 || strspn(ids.c_str(), "0123456789,") != ids.size() )
                ids = "0";

            prepareIds(ids);

            CartModel model;
            CartView  view;
            User      user;

            if(user_id.size() > 0 && user_token.size() > 0)
            {
                user = model.getDataUser(user_id, user_token);
                view.setUser(user);
            }

            string ajax_request = setvalc(parms, "HTTP_AJAX_REQUEST");

            if(ajax_request.size() > 0 && ajax_request == "xmlhttprequest")
            {
                return model.getJSON(ids, user);
            }

            vector<Data> arr(model.getDataIndex(ids, user));

            view.setTitle("Корзина");
            view.setAside();
            view.getContent(arr, model.getTotalCost());
            view.setRights(right);

            return view.templ();
        }

        void prepareIds(string& ids) {
            if(ids.size() == 0)
                return;

            vector<string> result;
            int pos;
            string sub;

            while(ids.length() > 0) {
                pos = ids.find(",");

                if(pos > -1)
                {
                    sub = ids.substr(0, pos);
                    ids.replace(0, sub.length()+1, "");
                }
                else
                {
                    pos = ids.length();
                    sub = ids.substr(0, pos);
                    ids.replace(0, sub.length(), "");
                }

                result.push_back(sub);
            }

            unsigned short len = result.size();
            string s;

            for(unsigned short i = 0; i < len; i++) {
                s = result[i];

                if(s.length() > 0 && strspn(s.c_str(), "0123456789") == s.size()) {
                    ids += result[i] + ",";
                }
            }

            if(ids[ids.length()-1] == ',')
                ids = ids.substr(0, ids.length()-1);
        }
};