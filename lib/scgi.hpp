#include <algorithm>
#include <arpa/inet.h>
#include <assert.h>
#include <cctype>
#include <cerrno>
#include <climits>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <err.h>
#include <errno.h>
#include <event.h>
#include <fcntl.h>
#include <functional>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <locale>
#include <map>
#include <unordered_map>
#include <memory>
#include <mysql/mysql.h>
#include <netinet/in.h>
#include <netdb.h>
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
// #include <pthread.h>
#include <random>
#include <signal.h>
#include <sstream>
#include <string>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stack>
#include <vector>
#include <exception>
#include <zlib.h>
#include <openssl/whrlpool.h>

#ifndef __SCGILIB__
#define __SCGILIB__

#define BUFFSIZE 5242880  // 500000

using namespace std;

int demonize();
bool init(char*, u_short);
map<string, string> deserializeObject(const string&, const string&);
vector< map<string, string> > deserializeArray(const string&);
vector<string> deserializeSimpleArray(const string&);
const string serializeObject(map<string, string>);
inline unsigned char from_hex (unsigned char);
const string urldecode (const string&);
const string& trim(string&);
string ftos(double);

struct cmp_str {
    bool operator()(char const *a, char const *b) {
        return strcmp(a, b) < 0;
    }
};



class Writer {
    public:
        Writer(){};

        void writeInLog(string);
};

struct User {
    string id;
    string login;
    string token;
    string first_name;
    string last_name;
    string avatar;
    string theme;
    string role;

    void reset() {
        id = "";
        login = "";
        token = "";
        first_name = "";
        last_name = "";
        avatar = "";
        theme = "";
        role = "";
    };

    User& operator=(const User& u)
    {
        id = u.id;
        login = u.login;
        token = u.token;
        first_name = u.first_name;
        last_name = u.last_name;
        avatar = u.avatar;
        theme = u.theme;
        role = u.role;

        return *this;
    }
};

struct Data {
    string id;
    string param;
    string name;
    string desc;
    string title;
    string description;
    string price;
    string adder_id;
    string adder_name;
    string adder_avatar;
    string amount_comments;
    string amount_like;
    string like;
};

struct ImageParams {
    string url_m;
    string url_s;
    short width;
    short height;
};

struct $_FILE {
    string name;
    string type;
    int    size;
    const char* body;
};

struct $_PARAM {
    string name;
    string body;
};

class View {
    protected:
        User user;
        string theme;
        string keywords;
        string description;
        string title;
        string content;
        string aside;
        string wrapper;
        map<string, vector<string>> right;
    public:
        View();
        void setUser(User&);
        virtual void setWrapper() {wrapper = "<div id=\"wrapper\">" + aside + content + "</div>";};
        const string getWord(const string&);
        int amountSpacesAndEnters(char*, int);
        void getContent(vector<Data>);
        const string getPreview(const Data&);
        void setTitle(const string);
        void setKeywords(const string);
        void setDescription(const string);
        void setRights(const map<string, vector<string>>&);
        bool hasRight(const string&, const string&);
        inline string templ() {
            bool user_is_set = user.id.size() > 0;

            setWrapper();

            return
                "<!doctype html>"
                "<html lang=\"ru\">"
                    "<head>"
                        "<title>" + title + "</title>"
                        "<link type=\"text/css\" rel=\"StyleSheet\" href=\"/templates/" + theme + "/css/style.css\">"
                        "<meta name=\"keywords\" content=\"" + keywords + "\">"
                        "<meta name=\"description\" content=\"" + description + "\">"
                        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=0.5, maximum-scale=0.8, user-scalable=yes\">"
                        "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">"
                        "<meta property=\"og:site_name\" content=\"Diginterest\"/>"
                        "<meta property=\"og:locale\" content=\"ru_RU\">"
                        "<meta property=\"og:title\" content=\"Diginterest — платформа для общения и ..\"/>"
                        "<meta property=\"og:description\" content=\"Платформа для общения и ..\"/>"
                        "<meta property=\"og:image\" content=\"http://galleryhd.ru/templates/hydrogen/icon/og-logo.jpg\"/>"
                        "<link rel=\"apple-touch-icon\" sizes=\"57x57\" href=\"/favicon/apple-icon-57x57.png\">"
                        "<link rel=\"apple-touch-icon\" sizes=\"60x60\" href=\"/favicon/apple-icon-60x60.png\">"
                        "<link rel=\"apple-touch-icon\" sizes=\"72x72\" href=\"/favicon/apple-icon-72x72.png\">"
                        "<link rel=\"apple-touch-icon\" sizes=\"76x76\" href=\"/favicon/apple-icon-76x76.png\">"
                        "<link rel=\"apple-touch-icon\" sizes=\"114x114\" href=\"/favicon/apple-icon-114x114.png\">"
                        "<link rel=\"apple-touch-icon\" sizes=\"120x120\" href=\"/favicon/apple-icon-120x120.png\">"
                        "<link rel=\"apple-touch-icon\" sizes=\"144x144\" href=\"/favicon/apple-icon-144x144.png\">"
                        "<link rel=\"apple-touch-icon\" sizes=\"152x152\" href=\"/favicon/apple-icon-152x152.png\">"
                        "<link rel=\"apple-touch-icon\" sizes=\"180x180\" href=\"/favicon/apple-icon-180x180.png\">"
                        "<link rel=\"icon\" type=\"image/png\" sizes=\"192x192\" href=\"/favicon/android-icon-192x192.png\">"
                        "<link rel=\"icon\" type=\"image/png\" sizes=\"32x32\" href=\"/favicon/favicon-32x32.png\">"
                        "<link rel=\"icon\" type=\"image/png\" sizes=\"96x96\" href=\"/favicon/favicon-96x96.png\">"
                        "<link rel=\"icon\" type=\"image/png\" sizes=\"16x16\" href=\"/favicon/favicon-16x16.png\">"
                        "<link rel=\"manifest\" href=\"/favicon/manifest.json\">"
                        "<meta name=\"msapplication-TileColor\" content=\"#ffffff\">"
                        "<meta name=\"msapplication-TileImage\" content=\"/favicon/ms-icon-144x144.png\">"
                        "<meta name=\"theme-color\" content=\"#ffffff\">"
                       "<link href='https://fonts.googleapis.com/css?family=Arimo:400,700&subset=latin,cyrillic' rel='stylesheet' type='text/css'>"


                        "<meta name=\"yandex-verification\" content=\"6eeab68e0d25290c\" />"
                    "</head>"
                    "<body id=\"body\">"
                        "<div id=\"wrap\">"
                            "<header id=\"header\">"
                                "<div id=\"header-fix-width\">"
                                    "<div id=\"top-line\">"
                                        "<span id=\"site-name\"><a href=\"/\" onclick=\"Controller.openNewPage(this, event, 'newsfeed');\">diginterest</a></span>"
                                        "<nav id=\"tabs\">"
                                        +
                                        (
                                            (!user_is_set) ?
                                            "<a href=\"/login\">Вход / Регистрация</a>" :
                                            "<div class=\"list\">"
                                                "<a href=\"/i" + user.id + "\" class=\"lst\" onclick=\"Controller.openNewPage(this, event, 'i');\">Моя страница</a>"
                                                "<div class=\"hide-list\">"
                                                    +
                                                    (
                                                        (hasRight(user.role, "addNote")) ?
                                                        "<a href=\"/addnote\" onclick=\"Controller.openModalAddNote(this, event, " + user.id + ",0);\">Добавить запись</a>" :
                                                        ""
                                                    )
                                                    +
                                                    "<a href=\"/logout\">Выход</a>"
                                                "</div>"
                                            "</div>"
                                        )
                                        +
                                            "<a href=\"/\" onclick=\"Controller.openNewPage(this, event, 'newsfeed');\">Новостная лента</a>"
//                                            "<a href=\"/people\" onclick=\"Controller.openNewPage(this, event, 'people');\">Люди</a>"
//                                            "<a href=\"/community\" onclick=\"Controller.openNewPage(this, event, 'community');\">Сообщества</a>"
                                       +
                                       (
                                           (!user_is_set) ?
                                           "<a href=\"/shop\" onclick=\"Controller.openNewPage(this, event, 'shop');\">Магазин</a>" :
                                           "<div class=\"list\">"
                                               "<a href=\"/shop\" class=\"lst\" onclick=\"Controller.openNewPage(this, event, 'shop');\">Магазин</a>"
                                               "<div class=\"hide-list\">"
                                                   +
                                                   (
                                                       (hasRight(user.role, "addItem")) ?
                                                       "<a href=\"/additem\" onclick=\"Controller.openModalAddItem(this, event, " + user.id + ",0);\">Добавить работу</a>" :
                                                       ""
                                                   )
                                                   +
                                                   "<a href=\"#\">Мои заказы</a>"
                                               "</div>"
                                           "</div>"
                                       )
                                       +
                                            "<a href=\"/about\" onclick=\"Controller.openNewPage(this, event, 'about');\">О сайте</a>"
//                                            "<div class=\"list\">"
//                                                "<a href=\"/about\" class=\"lst\" onclick=\"Controller.openNewPage(this, event, 'about');\">О сайте</a>"
//                                                "<div class=\"hide-list\">"
//                                                    "<a href=\"#\" onclick=\"Controller.openNewPage(this, event, 'about');\">Способы оплаты</a>"
//                                                    "<a href=\"#\" onclick=\"Controller.openNewPage(this, event, 'about');\">Контакты</a>"
//                                                "</div>"
//                                            "</div>"
                                           "<a href=\"/cart\" class=\"cart\" onclick=\"Controller.openNewPage(this, event, 'cart');\">Корзина</a>"
                                        "</nav>"
                                    "</div>"
                                "</div>"
                                "<canvas id=\"canvas\"></canvas>"
                            "</header>"
                            +
                            wrapper
                            +
                        "</div>"
                        "<div id=\"black-block\"></div>"
                        "<script type=\"text/javascript\" src=\"/js/scroll.js\"></script>"
                        "<script type=\"text/javascript\" src=\"/js/linkage.js\"></script>"
                        "<script type=\"text/javascript\" src=\"/js/elemento.js\"></script>"
                        "<script type=\"text/javascript\" src=\"/templates/" + theme + "/js/test.js\"></script>"
                    "</body>"
                "</html>";
        };
        ~View();
};

class Model {
    protected:
        MYSQL* mysql;

        MYSQL_ROW row;

        MYSQL_RES* res;

    public:
        Model();

        void query(string);

        string& back_slash(string&);

        User getDataUser(const string&, const string&);

        ~Model();
};

class Controller {
    public:
        short statusCode;

        string statusMessage;

        string redirectUrl;

        string moveUrl;

        list <string> headers;

	protected:
		void addHeader(string);

		string amount_posts;

		map<string, vector<string>> right;

	public:
		Controller(){
		    amount_posts = "20";
		    statusCode = 200;
		    statusMessage = "OK";
		    fillRights();
		};

		virtual ~Controller() {};

        const char* setvalc(map<const char*, const char*, cmp_str>*, const string&);

        string setval(map<string, const char*>*, const string&);

        string setval(map<string, string>*, const string&);

		map<string, string> parseQuery(string);

		map<string, string> parseCookie(string);

		int parseData(const char*, const string&, unsigned int&, vector<$_FILE>&, vector<$_PARAM>&);

		virtual string run(map<const char*, const char*, cmp_str>* parms){return "";};

		void getHeaders(char* outBuffer);

		string redirect(const short, const string, const string, const string);

		bool hasRight(const string&, const string&);

		void fillRights();
};

#endif