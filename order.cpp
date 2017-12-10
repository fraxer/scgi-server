class OrderView : public View {
    public:
        OrderView() : View() {
            aside =
            "<aside id=\"aside\"><div class=\"a-wrap\"></div></aside>";
        };

        void setTotalCost(const string& total_cost) {
            aside =
            "<aside id=\"aside\">"
                "<div class=\"a-wrap\">"
                    "<span class=\"total-cost\">Итого: <span class=\"cy\">$</span><span class=\"pr\">" + total_cost + "</span></span>"
                "</div>"
            "</aside>";
        };

        void getContent(vector<Data> arr) {
//            bool user_is_set = user.id.size() > 0;

            content +=
                "<main id=\"block-content\">";
                    "<div class=\"form\">"
                        "<span>text</span>"
                    "</div>"
                "</main>";
        };
};

class OrderModel : public Model {
    private:
        double totalCost;
    public:
        OrderModel() {
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
            buff.precision(20);
            buff.setf( ios::fixed, ios::floatfield );
            buff << totalCost;

            string s_total_cost = buff.str();

            short index = s_total_cost.find(".", 0);

            if(index != string::npos)
                s_total_cost = s_total_cost.substr(0, index+3);

            return s_total_cost;
        }
};

class OrderController: public Controller {
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

            OrderModel model;
            OrderView  view;
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

            view.setTitle("Оформление заказа");
            view.getContent(model.getDataIndex(ids, user));
            view.setTotalCost(model.getTotalCost());
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