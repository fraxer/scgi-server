class AboutView : public View {
    private:
        string insideContent;
    public:

        AboutView() : View() {
            aside =
            "<aside id=\"aside\" style=\"display:none\"><div class=\"a-wrap\"></div></aside>";
        };

        void setContent() {
            content = "<main id=\"block-content\" class=\"page-info\" style=\"max-width:1000px;width:initial\">";

            insideContent =
                "<h2 class=\"h2-c\">О сайте <span class=\"upper-c\">\"Diginterest\"</span></h2>"
                "<p>Здесь ничего нет.</p>"
            ;

//            insideContent += getInfo("/proc/cpuinfo");

            insideContent += "<hr>";

            insideContent += getBarMemory();

//            insideContent += getBarCpu();

            content += insideContent;
            content += "</main>";
        }

        string getInfo(const string _str) {
            ifstream stream(_str);
            string str;
            string out;

            if (stream.is_open()) {
                while ( getline (stream, str) ) {
                    out += "<p>" + str + "</p>";
                }
                stream.close();
            }

            return out;
        }

        string getBarMemory() {
            ifstream stream("/proc/meminfo");

            string out;

            if (stream.is_open()) {
                string str;
                string mem_total;
                string mem_free;
                string mem_buffers;
                string mem_cached;

                if(getline(stream, str))
                    mem_total = getStringInt(str);
                if(getline(stream, str))
                    mem_free = getStringInt(str);
                if(getline(stream, str))
                    mem_buffers = getStringInt(str);
                if(getline(stream, str))
                    mem_cached = getStringInt(str);

                stream.close();



                int int_mem_total   = atoi(mem_total.c_str());
                int int_mem_free    = atoi(mem_free.c_str());
                int int_mem_buffers = atoi(mem_buffers.c_str());
                int int_mem_cached  = atoi(mem_cached.c_str());
                int int_mem_active  = int_mem_total - int_mem_free - int_mem_buffers - int_mem_cached;

                out +=
                    "<h3 class=\"h3-c\">Оперативная память</h3>"
                    "<p><span class=\"mem-span total\">Всего</span>" + mem_total + " кб</p>"
                    "<p><span class=\"mem-span free\">Свободно</span>" + mem_free + " кб</p>"
                    "<p><span class=\"mem-span used\">Используется</span>" + to_string(int_mem_active) + " кб</p>"
                    "<p><span class=\"mem-span buffers\">Буферизовано</span>" + mem_buffers + " кб</p>"
                    "<p><span class=\"mem-span cached\">Кешировано</span>" + mem_cached + " кб</p>"

                    "<div class=\"bar-mem\">"
                        "<div class=\"bar-mem-block used\" style=\"width:" + to_string(int_mem_active*400/int_mem_total) + "px\"></div>"
                        "<div class=\"bar-mem-block buffers\" style=\"width:" + to_string(int_mem_buffers*400/int_mem_total) + "px\"></div>"
                        "<div class=\"bar-mem-block cached\" style=\"width:" + to_string(int_mem_cached*400/int_mem_total) + "px\"></div>"
                    "</div>";
            }

            return out;
        }

        string getBarCpu() {
            ifstream stream("/proc/stat");

            string out;
            int total_1 = 0;
            int work_1 = 0;
            int total_2 = 0;
            int work_2 = 0;

            if (stream.is_open()) {
                string str;

                if(getline(stream, str)) {
                    out += "<p>str_cpu1: " + str + "</p>";

                    setASDDD(str, total_1, work_1);
                }
            }

            stream.close();

            usleep(100000);

            ifstream stream2("/proc/stat");

            if(stream2.is_open()) {
                string str;

                if(getline(stream2, str)) {
                    out += "<p>str_cpu2: " + str + "</p>";

                    setASDDD(str, total_2, work_2);
                }
            }

            stream2.close();

            total_2++;
            work_2++;

            float result1 = work_2 - work_1;
            float result2 = total_2 - total_1;
            float result = result1 / result2 * 100;

            locale custom_locale("C");
            stringstream buff;
            buff.imbue(custom_locale);
            buff.precision(10);
            buff.setf( ios::fixed, ios::floatfield );
            buff << result;

            string s = buff.str();

            short index = s.find(".", 0);

            if(index != string::npos)
                s = s.substr(0, index+3);

            out +=
                "<h3 class=\"h3-c\">Процессор</h3>"
                "<p>total: " + to_string(total_1) + ", work: " + to_string(work_1) + "</p>"
                "<p>total: " + to_string(total_2) + ", work: " + to_string(work_2) + "</p>"
                "<p>%: " + s + "</p>";

            return out;
        }

        void setASDDD(const string& str_cpu, int& total, int& work) {
            short size = str_cpu.size();
            short size_l = size -1;
            bool flag = false;
            short pos = 0;
            short count = 0;
            short k = -1;

            for(short i = 0; i < size; i++) {
                k++;
                if(flag) {
                    if(str_cpu[i] == ' ') {
                        total += atoi(str_cpu.substr(pos, k-1).c_str());
                        if(count < 3)
                            work += atoi(str_cpu.substr(pos, k-1).c_str());

                        count++;
                        pos = i+1;
                        k = 0;
                    }
                    if(i == size_l) {
                        total += atoi(str_cpu.substr(pos, k).c_str());
                    }
                }
                if(!flag && str_cpu[i] == ' ') {
                    flag = true;
                    pos = i++;
                    k = 2;
                }
            }
        }

        string getStringInt(const string str) {
            char key[] = "0123456789";
            string s;

            char _str[str.size()];
            strcpy(_str, str.c_str());

            char *pch = strpbrk(_str, key);

            while (pch != NULL) {
                s += *pch;
                pch = strpbrk(pch+1, key);
            }

            return s;
        }

        string getInsideContent() {
            return insideContent;
        }
};

class AboutController: public Controller {
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

            Model model;
            AboutView view;
            User user;

            if(user_id.size() > 0 && user_token.size() > 0)
            {
                user = model.getDataUser(user_id, user_token);
                view.setUser(user);
//                user.reset();
            }

            view.setContent();

            string ajax_request = setvalc(parms, "HTTP_AJAX_REQUEST");

            if(ajax_request.size() > 0 && ajax_request == "xmlhttprequest")
            {
                string inside_content = view.getInsideContent();

                return
                    "{"
                       "\"data\":\"" + model.back_slash(inside_content) + "\","
                       "\"user\":" + (user.id.size() > 0 ? "" + user.id + "" : "\"0\"") +
                   "}";
            }

            view.setTitle("Diginterest - статьи, идеи, макеты, работы");
            view.setDescription("Площадка для web-разработчиков и дизайнеров. Обмен идеями, опытом, представление своих работ.");
            view.setRights(right);

            return view.templ();
        }
};