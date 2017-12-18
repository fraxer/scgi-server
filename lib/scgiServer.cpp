#include "scgi.hpp"

#include "../index.cpp"
#include "../viewpost.cpp"
#include "../viewcontent.cpp"
#include "../viewhide.cpp"
#include "../like.cpp"
#include "../about.cpp"
// #include "../addcomment.cpp"
// #include "../getcomments.cpp"
#include "../addnote.cpp"
// #include "../additem.cpp"
#include "../uploadimage.cpp"
// #include "../uploadimageshop.cpp"
// #include "../uploadzip.cpp"
// #include "../shop.cpp"
// #include "../search.cpp"
// #include "../i.cpp"
// #include "../cart.cpp"
#include "../login.cpp"
#include "../logout.cpp"
// #include "../reg.cpp"
// #include "../order.cpp"
// #include "../404.cpp"
// #include "../post.cpp"

using namespace std;

void setHandler(const char* document_uri, Controller** handler) {

    if(strcmp(document_uri, "/index.d") == 0)                *handler = new IndexController();
    else if(strcmp(document_uri, "/viewpost.d") == 0)        *handler = new ViewPostController();
    else if(strcmp(document_uri, "/viewcontent.d") == 0)     *handler = new ViewContentController();
    else if(strcmp(document_uri, "/viewhide.d") == 0)        *handler = new ViewHideController();
    else if(strcmp(document_uri, "/like.d") == 0)            *handler = new LikeController();
    else if(strcmp(document_uri, "/about.d") == 0)           *handler = new AboutController();
    // else if(strcmp(document_uri, "/addcomment.d") == 0)      *handler = new AddCommentController();
    // else if(strcmp(document_uri, "/getcomments.d") == 0)     *handler = new GetCommentsController();
    else if(strcmp(document_uri, "/addnote.d") == 0)         *handler = new AddNoteController();
    // else if(strcmp(document_uri, "/additem.d") == 0)         *handler = new AddItemController();
    else if(strcmp(document_uri, "/uploadimage.d") == 0)     *handler = new UploadImageController();
    // else if(strcmp(document_uri, "/uploadimageshop.d") == 0) *handler = new UploadImageShopController();
    // else if(strcmp(document_uri, "/uploadzip.d") == 0)       *handler = new UploadZIPController();
    // else if(strcmp(document_uri, "/shop.d") == 0)            *handler = new ShopController();
    // else if(strcmp(document_uri, "/search.d") == 0)          *handler = new SearchController();
    // else if(strcmp(document_uri, "/i.d") == 0)               *handler = new IController();
    // else if(strcmp(document_uri, "/cart.d") == 0)            *handler = new CartController();
    else if(strcmp(document_uri, "/login.d") == 0)           *handler = new LoginController();
    else if(strcmp(document_uri, "/logout.d") == 0)          *handler = new LogoutController();
    // else if(strcmp(document_uri, "/reg.d") == 0)             *handler = new RegController();
    // else if(strcmp(document_uri, "/order.d") == 0)           *handler = new OrderController();
    // else if(strcmp(document_uri, "/404.d") == 0)             *handler = new NotFoundController();
    // else if(strcmp(document_uri, "/post.d") == 0)            *handler = new PostController();
}

void Writer::writeInLog(string message) {
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80 ,"%d-%m-%Y %I:%M:%S", timeinfo);
    string dt(buffer);


    ofstream myfile("logs.txt", ios::app);

    if (myfile.is_open()) {
        myfile << dt << " - " << message << "\n";
        myfile.close();
    }
};

short number_digits(int n) {
    short count = 0;

    while(n) {
        n /= 10;
        count++;
    }

    return count;
};


// map<const char*, const char*>::iterator findValueInMap(map<const char*, const char*>& parms, const char* key) {
const char* getValueFromMap(map<const char*, const char*>& parms, const char* key) {
    for (map<const char*, const char*>::iterator it = parms.begin(); it!=parms.end(); ++it) {
        if(strcmp(it->first, key) == 0) {
            return it->second;
        }
    }
    return "";
};

void setValueInMap(map<const char*, const char*>& parms, const char* key, const char* value) {
    for (map<const char*, const char*>::iterator it = parms.begin(); it != parms.end(); ++it) {
        if(strcmp(it->first, key) == 0) {
             it->second = value;
             return;
        }
    }
};

bool parseInputString(char* buff, map<const char*, const char*>& parms) {

    // Writer w;

    // w.writeInLog("p1");

    char* p = (char*)memchr(buff, ':', 30);

    if(p == NULL) {
        Writer w;
        w.writeInLog("can't parse string");
        return false;
    }

    char value[p-buff];

    strncpy(value, buff, p-buff);

    value[p-buff] = '\0';

    int len = atoi(value);

    p++;

    int g = 0;

    int l = 0;

    char* n;

    while(g < len) {
        n = p;

        l = strlen(p) + 1;
        p += l;
        g += l;

        parms.emplace(n, p);

        l = strlen(p) + 1;
        p += l;
        g += l;
    }

    len = atoi(getValueFromMap(parms, "CONTENT_LENGTH"));

    if(len > 0) {
        ++p;
        strncpy(p+len, "\0", 1);
        parms.emplace("DATA", p);
    }

    return true;
}

unsigned long long getInputLength(char* buff) {

    unsigned long long full_length = 0;

    char* p = (char*)memchr(buff, ':', 30);

    if(p == NULL) {
        Writer w;
        w.writeInLog("can't parse string");
        return false;
    }

    char value[p-buff];

    full_length += p-buff;

    strncpy(value, buff, p-buff);

    value[p-buff] = '\0';

    int len = atoi(value);

    full_length += len + 2;

    p++;

    int g = 0;

    int l = 0;

    char* n;

    while(g < len) {
        n = p;

        l = strlen(p) + 1;
        p += l;
        g += l;

        if(strcmp(n, "CONTENT_LENGTH") == 0) {
            full_length += atoi(p);
            break;
        }

        l = strlen(p) + 1;
        p += l;
        g += l;
    }

    return full_length;
}

map<string, string> deserializeObject(const string& str, const string& prop = "") {
    map<string, string> obj;
    string _str = str.substr(1, str.size()-2);
    int i = 0;
    int p = 0;
    int amount_bracket = 0;
    bool in_quotes = true;
    string p_name;
    string p_data;
    int pos = -1;
    string sub;

    while(_str[i++] != '\0') {
        if(_str[i] == '\"')
            in_quotes = !in_quotes;

        if(!in_quotes)
        {
            if(_str[i] == '{' || _str[i] == '[')
                amount_bracket++;
            else if(_str[i] == '}' || _str[i] == ']')
                amount_bracket--;
        }

        if(amount_bracket != 0)
            continue;

        if(_str[i] == ',' || _str[i] == '\0')
        {
            if(_str[i] == ',')
            {
                sub = _str.substr(p, i-p);
                p = i+1;
            }
            else if(_str[i] == '\0')
                sub = _str.substr(p, _str.size());

            pos = sub.find(":");

            if(pos > -1)
            {
                p_name = sub.substr(1, pos-2);
                p_data = sub.substr(pos+1, sub.size());

                if(p_data[0] == '\"')
                    p_data = p_data.substr(1, p_data.size()-2);

                obj[p_name] = p_data;
                if(prop.size() > 0 && prop == p_name)
                    break;
            }
        }
    }

    return obj;
};

vector< map<string, string> > deserializeArray(const string& str) {
    string _str = str.substr(1, str.size()-2);
    int i = -1;
    int p = 0;
    int amount_bracket = 0;
    bool in_quotes = false;
    string p_name;
    string p_data;
    int pos = -1;
    string sub;

    map<string, string> mapobj;
    vector< map<string, string> > arr;

    if(str[0] != '[')
        return arr;

    while(_str[i++] != '\0') {
        if(_str[i] == '\"')
            in_quotes = !in_quotes;

        if(!in_quotes)
        {
            if(_str[i] == '{' || _str[i] == '[') {
                amount_bracket++;
                p++;

                continue;
            }
            else if(_str[i] == '}' || _str[i] == ']') {
                amount_bracket--;
                arr.push_back(mapobj);
                mapobj.clear();
            }
        }

        if(amount_bracket == 0) {
            p++;
            continue;
        }

        if(_str[i] != ',' && _str[i+1] != '}')
            continue;

        if(_str[i] == ',')
        {
            sub = _str.substr(p, i-p);
            p = i+1;
        }
        if(_str[i+1] == '}')
        {
            sub = _str.substr(p, i-p+1);
            p = i+1;
        }

        pos = sub.find(":");

        if(pos > -1)
        {
            p_name = sub.substr(1, pos-2);
            p_data = sub.substr(pos+1, sub.size());

            if(p_data[0] == '\"')
                p_data = p_data.substr(1, p_data.size()-2);

            mapobj[p_name] = p_data;
        }

    }

    return arr;
};

vector<string> deserializeSimpleArray(const string& str) {
    int sz = str.size()-2;
    string _str = str.substr(1, sz);
    int i = -1;
    int p = 0;
    int amount_bracket = 0;
    bool quote_blocked = false;
    bool bracket_blocked = false;
    int pos = -1;
    string sub;

    vector< string > arr;

    if(str[0] != '[' || str[str.size()-1] != ']')
        return arr;

    pos = 1;

    while(_str[i++] != '\0') {
        if(bracket_blocked == false && (_str[i] == '[' || _str[i] == '{')) {
            if(amount_bracket == 0)
                p = i-1;
            amount_bracket++;
            quote_blocked = true;
        }
        if(bracket_blocked == false && (_str[i] == ']' || _str[i] == '}')) {
            amount_bracket--;

            if(amount_bracket == 0) {
                quote_blocked = false;

                sub = _str.substr(p+1, i-p);

                p = i+1;

                arr.push_back(sub);
            }
        }
        if(quote_blocked == false && _str[i] == '\"') {
            if(amount_bracket == 0) {
                p = i;
                amount_bracket++;
                bracket_blocked = true;
            } else {
                amount_bracket--;

                bracket_blocked = false;

                sub = _str.substr(p+1, i-p-1);

                p = i+1;

                arr.push_back(sub);
            }
        }
        if(amount_bracket == 0 && (_str[i] == ',' || i == sz)) {
            if(p == i) {
                p++;
                continue;
            }
            sub = _str.substr(p, i-p);

            arr.push_back(sub);

            p = i + 1;
        }
    }

    return arr;
};

const string serializeObject(map<string, string> obj) {
    string str = "{";

    int size_l = obj.size()-1;
    int i = 0;

    for (auto it = obj.begin(); it != obj.end(); ++it)///вывод на экран
    {
        string _s = (i == size_l) ? "" : ",";
        string quote = ( it->second[0] == '[' || it->second[0] == '{' ) ? "" : "\"";

        str += "\"" + it->first + "\"" + ":" + quote + it->second + quote + _s;

        i++;
    }

    str += "}";

    return str;
};

inline unsigned char from_hex (unsigned char ch) {
    if (ch <= '9' && ch >= '0')
        ch -= '0';
    else if (ch <= 'f' && ch >= 'a')
        ch -= 'a' - 10;
    else if (ch <= 'F' && ch >= 'A')
        ch -= 'A' - 10;
    else
        ch = 0;
    return ch;
};

const string urldecode (const string& str) {
    string result;
    string::size_type i;
    for (i = 0; i < str.size(); ++i)
    {
        if (str[i] == '+')
        {
            result += ' ';
        }
        else if (str[i] == '%' && str.size() > i+2)
        {
            const unsigned char ch1 = from_hex(str[i+1]);
            const unsigned char ch2 = from_hex(str[i+2]);
            const unsigned char ch = (ch1 << 4) | ch2;
            result += ch;
            i += 2;
        }
        else
        {
            result += str[i];
        }
    }
    return result;
};

const string& trim(string& str) {
    int g = 0;
    int l = str.size();

    while(l > 0 && g < l)
    {
        if(str[g] == ' ' || str[g] == '\n')
        {
            str.replace(g, 1, "");
            l = str.size();
            g--;
        }
        else
            break;

        g++;
    }

    while(l > 0)
    {
        if(str[l-1] == ' ' || str[l-1] == '\n')
        {
            str.replace(l-1, 1, "");
            l = str.size();
        }
        else
            break;
    }

    return str;
};

string ftos(double number) {
    stringstream buff;
    buff.precision(20);
    buff << number;

    string s_total_cost = buff.str();

    short index = s_total_cost.find(".", 0);

    if(index != string::npos)
        s_total_cost = s_total_cost.substr(0, index+3);

    return s_total_cost;
};

void on_connect(evutil_socket_t fd, short event, void *arg) {

    // auto clock_begin = std::chrono::steady_clock::now();

    sockaddr_storage ss;
    socklen_t len = sizeof(ss);

    int sock = accept(fd, (sockaddr*)&ss, &len);

    // Writer w;

    if(sock < 0) {
        // string ____s = string("socket error: ") + strerror(errno);
        // w.writeInLog(____s);
        return;
    }

    evutil_make_socket_nonblocking(sock);

    char message[BUFSIZ];

    ssize_t len_recv;

    // string s_recv;

    ssize_t data_size = BUFSIZ;

    char* data = (char*)malloc(data_size);

    ssize_t slen = 0;

    bool get_data = false;

    unsigned long long full_length = 0;

    while(len_recv = recv(sock, &message, BUFSIZ, MSG_DONTWAIT)) {
        
        if(len_recv < 0) {

            if(errno == EBADF) {
                // s_recv = string("EBADF: ") + strerror(errno);
                // w.writeInLog(s_recv);
                break;
            }
            if(errno == ECONNREFUSED) {
                // s_recv = string("ECONNREFUSED: ") + strerror(errno);
                // w.writeInLog(s_recv);
                break;
            }
            if(errno == EFAULT) {
                // s_recv = string("EFAULT: ") + strerror(errno);
                // w.writeInLog(s_recv);
                break;
            }
            if(errno == EINTR) {
                // s_recv = string("EINTR: ") + strerror(errno);
                // w.writeInLog(s_recv);
                break;
            }
            if(errno == EINVAL) {
                // s_recv = string("EINVAL: ") + strerror(errno);
                // w.writeInLog(s_recv);
                break;
            }
            if(errno == ENOMEM) {
                // s_recv = string("ENOMEM: ") + strerror(errno);
                // w.writeInLog(s_recv);
                break;
            }
            if(errno == ENOTCONN) {
                // s_recv = string("ENOTCONN: ") + strerror(errno);
                // w.writeInLog(s_recv);
                break;
            }
            if(errno == ENOTSOCK) {
                // s_recv = string("ENOTCONN: ") + strerror(errno);
                // w.writeInLog(s_recv);
                break;
            }
            if(errno == EAGAIN) {

                if(get_data && full_length == slen)
                    break;

                // s_recv = string("EAGAIN: ") + strerror(errno);
                // w.writeInLog(s_recv);

                continue;
            }
            if(errno == EWOULDBLOCK) {
                
               
                if(get_data && full_length == slen)
                    break;
                
                // s_recv = string("EWOULDBLOCK: ") + strerror(errno);
                // w.writeInLog(s_recv);

                continue;
            }

            break;
        }
        else {
            
            // w.writeInLog("get data");
            // w.writeInLog("slen: " + to_string(slen));
            // w.writeInLog("len_recv: " + to_string(len_recv));

            if(data_size < slen + len_recv) {
                data_size += len_recv;
                data = (char*)realloc(data, data_size);
            }

            if(!get_data) {
                full_length = getInputLength(message);
            }

            memmove(data + slen, &message, len_recv);

            slen += len_recv;

            get_data = true;
        }
    }



// string sssss = "data_" + to_string(rand() % 100 + 1);
// ofstream myfile;
// myfile.open(sssss.c_str(), ios::app);

// if (myfile.is_open())
// {
//    myfile.write(data, slen);
//    myfile.close();
// }

    map<const char*, const char*> parms;

    if(!parseInputString(data, parms)) {
        free(data);
        close(sock);
        return;
    }

    int content_length = atoi(getValueFromMap(parms, "CONTENT_LENGTH"));

    if(BUFFSIZE < content_length) {
        char out_data[38];

        sprintf(out_data, "Status: %d %s\r\n", 413, "Request Entity Too Large");

        // Status: 413 Request Entity Too Large\r\n

        send(sock, out_data, strlen(out_data), MSG_DONTWAIT);

        free(data);

        close(sock);

        return;
    }

    short   statusCode = 200;
    char    statusMsg[10];
    unsigned int contentLength = 0;
    char    headersOutBuff[1024];

    Controller* handler = nullptr;

    char* document_uri = (char*)getValueFromMap(parms, "DOCUMENT_URI");

    setHandler(document_uri, &handler);

    if (handler == nullptr) {
        statusCode = 404;

        strcpy(statusMsg, "Not Found");

        char out_data[23];

        // Status: 404 Not Found\r\n
        sprintf(out_data, "Status: %d %s\r\n", statusCode, statusMsg);

        send(sock, out_data, strlen(out_data), MSG_DONTWAIT);

    } else {

        string *s = &handler->run(parms);

        const char* handler_data = s->c_str();

        // Writer w;
        // w.writeInLog("0---");
        // w.writeInLog(document_uri);
        // w.writeInLog("1---");
        // w.writeInLog(*s);
        // w.writeInLog("2---");
        // w.writeInLog(handler_data);
        // w.writeInLog("3---");

        statusCode = handler->statusCode;

        strcpy(statusMsg, handler->statusMessage.c_str());

        // Writer w;
        // w.writeInLog("1");

        if(statusCode == 301 || statusCode == 302) {

            char document_uri_buffer[128];

            unsigned short document_uri_buffer_length = strlen(handler->redirectUrl->c_str()) > 128 ? 128 : strlen(handler->redirectUrl->c_str());

            strncpy(document_uri_buffer, handler->redirectUrl->c_str(), document_uri_buffer_length+1);

            document_uri_buffer[document_uri_buffer_length] = '\0';

            document_uri = document_uri_buffer;

            setValueInMap(parms, "DOCUMENT_URI", document_uri);
            setValueInMap(parms, "REQUEST_METHOD", "GET");

            // w.writeInLog(document_uri);

            statusCode = handler->statusCode;

            list<string> headers;

            char header[512];

            while(handler->headers.size() > 0) {

                unsigned short header_length = strlen(handler->headers.back().c_str()) > 512 ? 512 : strlen(handler->headers.back().c_str());

                strncpy(header, handler->headers.back().c_str(), header_length+1);

                header[header_length] = '\0';

                headers.push_back(header);

                handler->headers.pop_back();
            }

            // for (list<string>::iterator it=headers.begin(); it != headers.end(); ++it)
                // w.writeInLog(*it);


            strcpy(statusMsg, handler->statusMessage.c_str());

            // w.writeInLog("1.2");

            delete s;

            // w.writeInLog("1.3");

            delete handler;

            // w.writeInLog("1.4");

            setHandler(document_uri, &handler);

            // w.writeInLog("1.4.1");

            // w.writeInLog(document_uri);

            // w.writeInLog("1.4.2");

            // for (list<string>::iterator it=headers.begin(); it != headers.end(); ++it)
                // w.writeInLog(*it);

            // w.writeInLog("1.4.3");

            while(headers.size() > 0) {
                handler->headers.push_back(headers.back());
                headers.pop_back();
            }

            // w.writeInLog("1.5");

            s = &handler->run(parms);

            // w.writeInLog("1.6");

            handler_data = s->c_str();

            // w.writeInLog("1.7");

            handler->headers.unique();
        }

        // w.writeInLog("2");

        contentLength = strlen(handler_data);

        handler->getHeaders(headersOutBuff);

        char out_data[31+number_digits(statusCode)+strlen(statusMsg)+number_digits(contentLength)+strlen(headersOutBuff)+contentLength];
        
        // Status:  rnContent-length: rnrn
        sprintf(out_data, "Status: %d %s\r\nContent-length: %d\r\n%s\r\n%s", statusCode, statusMsg, contentLength, headersOutBuff, handler_data);

        send(sock, out_data, strlen(out_data), MSG_DONTWAIT);

        delete s;

        // w.writeInLog("3");

        delete handler;

        // handler = nullptr;
    }

    free(data);

    close(sock);

    // auto clock_end = std::chrono::steady_clock::now();

    // auto time_span = clock_end - clock_begin;

    // double nseconds = double(time_span.count()) * std::chrono::steady_clock::period::num / std::chrono::steady_clock::period::den;

    // w.writeInLog(to_string(nseconds));

    return;
}

bool init(char* ip_addr, u_short port) {

    Writer     w;
    event_base *base = event_base_new();
    event      *listener_event;

    if (!base) {
        w.writeInLog("Failed to create new event base");

        return false;
    }

    remove("/run/scgiServer.sock");

    sockaddr_un sa;
    int size;

    // bzero((void*)&sa,sizeof(sa));

    sa.sun_family      = AF_UNIX;
    // sa.sin_family      = AF_INET;
    // sa.sin_port        = htons(port);
    // sa.sin_addr.s_addr = inet_addr(ip_addr);

    strcpy(sa.sun_path, "/run/scgiServer.sock");

    evutil_socket_t server_sock = socket(AF_UNIX, SOCK_STREAM, 0);

    if (server_sock < 0) {
        string s = string("Failed to create socket") + strerror(errno);
        w.writeInLog(s);

        return false;
    }

    size = offsetof(struct sockaddr_un, sun_path) + strlen(sa.sun_path);

    int onra = 1;
    if (setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &onra, sizeof(onra)) < 0) {
        w.writeInLog("Failed to set option SO_REUSEADDR");

        return false;
    }

    // int onk = 100;
    // if (setsockopt(server_sock, SOL_SOCKET, SO_KEEPALIVE, &onk, sizeof(onk)) < 0) {
    //     w.writeInLog("Failed to set option SO_KEEPALIVE");

    //     return false;
    // }

    evutil_make_socket_nonblocking(server_sock);

    // if (::bind(server_sock, (sockaddr*)&(sa), sizeof(sa)) < 0) {
    if (::bind(server_sock, (sockaddr*)&(sa), size) < 0) {
        string s = string("bind error: ") + strerror(errno);
        w.writeInLog(s);

        return false;
    }

    if (listen(server_sock, SOMAXCONN) < 0) {
        close(server_sock);

        string s = string("Failed to make server listen: ") + strerror(errno);
        w.writeInLog(s);

        return false;
    }

    // Ignore SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    event *ev = event_new(base, server_sock, EV_READ|EV_PERSIST, on_connect, (void*)base);

    event_add(ev, NULL);

    event_base_dispatch(base);

    event_base_free(base);

    return true;
};

int demonize() {

    int fd;
    Writer w;

    switch (fork()) {
        case -1:
            w.writeInLog("demonize fork failed");
            return -1;

        case 0:
            break;

        default:
            return 0;
    }

    int pid = getpid();

    if (setsid() == -1) {
        w.writeInLog("setsid() failed");
        return -1;
    }

    umask(0);

    // for (int i = 0; i < 1024; i++)
    //     close(i);

    fd = open("/dev/null", O_RDWR);

    if (fd == -1) {
        w.writeInLog("open(\"/dev/null\") failed");
        return -1;
    }

    if (fd > STDERR_FILENO) {
        if (close(fd) == -1) {
            w.writeInLog("close(fd) failed");
            return -1;
        }
    }

    return pid;
};

string& Controller::redirect(const short code, const string message, const string url, const string move) {
    statusCode = code;
    statusMessage = message;
    redirectUrl = new string(url);
    moveUrl = move;
    addHeader("Location: " + move);
    return *(new string());
};

const char* Controller::setvalc(map<const char*, const char*>* data, const string& value) {
	map<const char*, const char*>::iterator it = data->find(value.c_str());

	if (it != data->end())
		return (*it).second;

	return "";
};

string Controller::setval(map<string, const char*>* data, const string& value) {
	map<string, const char*>::iterator it = data->find(value);

	if (it != data->end())
		return (*it).second;

	return "";
};

string Controller::setval(map<string, string>* data, const string& value) {
	map<string, string>::iterator it = data->find(value);

	if (it != data->end())
		return (*it).second;

	return "";
};

map<string, string> Controller::parseQuery(string _str) {

    map<string, string> result;

    while(_str.length() > 0) {
        int pos = _str.find("&");
        string sub;

        if(pos > -1)
        {
            sub = _str.substr(0, pos);
            _str.replace(0, sub.length()+1, "");
        }
        else
        {
            pos = _str.length();
            sub = _str.substr(0, pos);
            _str.replace(0, sub.length(), "");
        }

        pos = sub.find("=");

        if(pos > -1)
        {
            string p_name = sub.substr(0, pos);
            string p_data = sub.substr(pos+1, sub.length());

            result.insert(pair<string,string>(p_name, p_data));
        }
    }

    return result;
};

map<string, string> Controller::parseCookie(string _str) {

    map<string, string> result;

    while(_str.length() > 0) {
        int pos = _str.find("; ");
        string sub;

        if(pos > -1)
        {
            sub = _str.substr(0, pos);
            _str.replace(0, sub.length()+2, "");
        }
        else
        {
            pos = _str.length();
            sub = _str.substr(0, pos);
            _str.replace(0, sub.length(), "");
        }

        pos = sub.find("=");

        if(pos > -1)
        {
            string p_name = sub.substr(0, pos);
            string p_data = sub.substr(pos+1, sub.length());

            result.insert(pair<string,string>(p_name, p_data));
        }
    }

    return result;
};

void Controller::getHeaders(char* p) {

    if (headers.size()) {

        for (list<string>::iterator it = headers.begin(); it != headers.end(); it++) {

            strcpy(p, (*it).c_str());

            p += (*it).size();

            *(p++) = '\r';

            *(p++) = '\n';
        }

    }

    *p = '\0';

    headers.clear();
};

int Controller::parseData(const char* data, const string& boundary, unsigned int& len, vector<$_FILE>& FILES, vector<$_PARAM>& PARAMS) {
    string name;
    string file_name;
    string file_type;

    const char* u = strstr(data, boundary.c_str());

    if(*(u+1) != '-' && *(u+2) != '-')
        return 0;

    u += boundary.size() + 1; // boudary + \n
    u = strstr(u, "Content-Disposition: form-data; name=\"");

    if(u)
        u += 37;
    else
        return 1;

    unsigned int i = 0;

    while(*(++u) != '"')
        i++;

    name = (u-i);
    name = name.substr(0, i);

    if(name == "file") {
        u = strstr(u, "filename=\"");

        u += 9;
        i = 0;

        while(*(++u) != '"')
            i++;

        file_name = (u-i);
        file_name = file_name.substr(0, i);

        u += 2;

        u = strstr(u, "Content-Type: ");

        u += 13;

        i = 0;

        while(*(++u) != '\n')
            i++;

        file_type = (u-i);
        file_type = file_type.substr(0, i-1);

    }
    else {
        u += 2;
    }

    bool finded = false;

    i = 0;
    while(u++) {
        i++;

        if(*(u) == '-' && *(u+1) == '-' && *(u+2) == '-' && *(u+3) == '-') {
            if(strstr(u, boundary.c_str())) {
                finded = true;
                break;
            }
        }

    }
    i--;

    if(!finded)
        return 0;

    if(name == "file") {
        if(file_type != "") {
            $_FILE arr;

            arr.name = file_name;
            arr.type = file_type;
            arr.size = i-4;
            arr.body = u-i+2;

            FILES.push_back(arr);
        }
    }
    else {
        $_PARAM arr;

        arr.name = file_name;
        arr.body = u-i+2;

        PARAMS.push_back(arr);
    }

    return parseData(u, boundary, len, FILES, PARAMS);
};

bool Controller::hasRight(const string& who, const string& what) {
    short size = right[who].size();
    for(short i = 0; i < size; i++) {
        if(right[who][i] == what)
            return true;
    }
    return false;
}

void Controller::fillRights() {
    vector<string> admin_operations;
                   admin_operations.push_back("addNote");
                   admin_operations.push_back("addItem");
                   admin_operations.push_back("addComment");

    vector<string> user_operations;
                   user_operations.push_back("addNote");
                   user_operations.push_back("addComment");

    right["admin"] = admin_operations;
    right["user"]  = user_operations;
};


void Controller::addHeader(string header) {
	headers.push_back(header);
};

View::View() {
    theme = "hydrogen";
};

void View::setUser(User &_user) {
    user = _user;
};

const string View::getWord(const string& num) {
    //Оставляем две последние цифры от num
    string number;

    if(num.length() == 1) {
        number = num;
    }
    if(num.length() >= 2) {
        number += num[num.length()-2];
        number += num[num.length()-1];
    }

    //Если 2 последние цифры входят в диапазон от 11 до 14
    //Тогда подставляем окончание "ЕВ"
    short _number = stoi(number);
    if(_number > 10 and _number < 15)
        return " " + num + " комментариев";

    string n;
    n += number[number.length()-1];
    short l_number = stoi(n);
    string term = "ев";

    if(l_number == 1)
        term = "й";
    if(l_number > 1)
        term = "я";
    if(l_number > 4)
        term = "ев";

    return " " + num + " комментари" + term;
};

void View::setTitle(const string val) {
    title = val;
};
void View::setKeywords(const string val) {
    keywords = val;
};
void View::setDescription(const string val) {
    description = val;
};

int View::amountSpacesAndEnters(char* str, int amount) {
    int spaces = 0;
    int enters = 0;
    int max_amount = amount + 1;

    while(*(str++) != '\0')
    {
        if(*str == '\n')
        {
            enters++;
            if(enters >= 9)
                return max_amount;
            continue;
        }
        if(*str == ' ')
        {
            spaces++;
            if(spaces == max_amount)
                return spaces;
        }
    }

    return spaces;
};

void View::getContent(vector<Data> arr) {
    unsigned short size = arr.size();
    bool user_is_set = user.id.size() > 0;
    unsigned short count_comments = 0;
    bool short_text = false;

    content += "<main id=\"block-content\">";

    for(unsigned short i = 0; i < size; i++)
    {
        short_text = (amountSpacesAndEnters((char*)arr[i].desc.c_str(), 60) > 60) ? true : false;

        count_comments = atoi(arr[i].amount_comments.c_str());

        content +=
        "<article class=\"content c\">"
            "<a href=\"/post" + arr[i].id + "\"></a>"
            "<div class=\"post-text\"" + (short_text ? " style=\"height:120px\"" : "") + ">"
                "<div>"
                    "<a href=\"/i" + arr[i].adder_id + "\" class=\"al " + (arr[i].desc.size() > 0 ? "m-b" : "") + "\" onclick=\"Controller.openNewPage(this, event, 'mypage');\">"
                        "<div class=\"av\">"
                            "<img src=\"" + arr[i].adder_avatar + "\">"
                        "</div>"
                        "<span>" + arr[i].adder_name + "</span>"
                    "</a>"
                "</div>"
                + arr[i].desc +
            "</div>"
            +
            (
                short_text ?
                    "<div class=\"toggle-text\" data-state=\"0\" onclick=\"Controller.togglePostText(this, 120);\">Показать полностью</div>" :
                    ""
            )
            +
            getPreview(arr[i])
            +
            "<div class=\"bottom-nav\">"
                "<div class=\"comments\" " + ( count_comments > 0 || user_is_set ? "onclick=\"Controller.listComments(" + arr[i].id + ", " + (user_is_set ? "true" : "false") + ", this);\"" : "" ) + ">"
                    "<span>"
                    +
                    (
                        count_comments > 0 ? getWord(arr[i].amount_comments) : (user_is_set ? "Добавить комментарий" : "Нет комментариев")
                    )
                    +
                    "</span>"
                "</div>"
                +
                (
                    user_is_set ?
                    "<div class=\"like l-" + (string)(arr[i].like.size() > 0 && arr[i].like == "1" ? "on" : "off") + "\" onclick=\"Controller.like(" + arr[i].id + ", this);\">"
                        "<span>" + (atoi(arr[i].amount_like.c_str()) > 0 ? arr[i].amount_like : "") + "</span>"
                    "</div>" : ""
                )
                +
                "<div class=\"block-comments\">"
                    "<div class=\"list-comments\"></div>"
                "</div>"
            "</div>"
        "</article>";
    }

    content += "</main>";
};

const string View::getPreview(const Data& d) {
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
                "<div class=\"block-image\" onclick=\"Controller.openViewerPost(" + d.id + ", 0, 'viewpost');\">"
                    "<img src=\"" + arr[0]["s"] +  "\" alt=\"post\" data-w=\"" + arr[0]["resol"].substr(0, pos) + "\" data-h=\"" + arr[0]["resol"].substr(pos+1) + "\">"
                "</div>";
        }

        string block = "<div class=\"block-image multi\">";

        for(short i = 0; i < size; i++) {
            short pos = arr[i]["resol"].find("x");
            block += "<img src=\"" + arr[i]["s"] +  "\" data-w=\"" + arr[i]["resol"].substr(0, pos) + "\" data-h=\"" + arr[i]["resol"].substr(pos+1) + "\" onclick=\"Controller.openViewerPost(" + d.id + ", 1, 'viewcontent', " + to_string(i) + ");\" alt=\"content\">";
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
                "<div class=\"block-image\" onclick=\"Controller.openViewerPost(" + d.id + ", 0, 'viewpost');\">"
                    "<img src=\"" + m["s"] +  "\" alt=\"post\" data-w=\"" +m["resol"].substr(0, pos) + "\" data-h=\"" + m["resol"].substr(pos+1) + "\">"
                "</div>"
            ;
        }
    }

    return "";
};

void View::setRights(const map<string, vector<string>>& r) {
    right = r;
};

bool View::hasRight(const string& who, const string& what) {
    short size = right[who].size();
    for(short i = 0; i < size; i++) {
        if(right[who][i] == what)
            return true;
    }
    return false;
}

View::~View() {
    user.reset();
    content = "";
};


Model::Model() {
    mysql = mysql_init(NULL);

//    unsigned int flag_socket = MYSQL_PROTOCOL_SOCKET;
//    mysql_options(mysql, MYSQL_OPT_PROTOCOL, &flag_socket);

    my_bool reconnect = true;
    mysql_options(mysql, MYSQL_OPT_RECONNECT, &reconnect);


    // Подключаемся к серверу
//    if(!mysql_real_connect(mysql, "127.0.0.1", "user", "", "pvine", 3306, NULL, 0))
    if(!mysql_real_connect(mysql, "localhost", "root", "root", "pvine", 3306, "/var/run/mysqld/mysqld.sock", 0))
    {
        string s = string("Error: can't create MySQL-descriptor: ") + mysql_error(mysql);
        Writer w;
        w.writeInLog(s);
    }
};

void Model::query(string s) {
//    Writer w;
//    w.writeInLog("query: " + s);
    if(mysql_query(mysql, s.c_str()) != 0)
    {
        string gdf = mysql_error(mysql);

        Writer w;
        w.writeInLog("Wrong query: " + gdf);
    }

    res = mysql_store_result(mysql);
};

string& Model::back_slash(string& str) {
    int g = 0;

    while(str[g] != '\0')
    {
        g++;
        if(str[g] == '\"')
        {
            str.insert(g, "\\");
            g++;
        }
        if(str[g] == '/')
        {
            str.insert(g, "\\");
            g++;
        }
        if(str[g] == '\n')
        {
            str.replace(g, 1, "\\n");
            g -= 2;
        }
        if(str[g] == '\r')
        {
            str.replace(g, 1, "\\r");
            g -= 2;
        }
        if(str[g] == '\t')
        {
            str.replace(g, 1, "\\t");
            g -= 2;
        }
        if(str[g] == '\v')
        {
            str.replace(g, 1, "\\v");
            g -= 2;
        }
    }

    return str;
};

User Model::getDataUser(const string& id, const string& token) {
    query("SELECT SQL_CACHE "
            "`id`, "
            "`token`, "
            "`avatar`, "
            "`login`, "
            "`role` "
        "FROM "
            "`user` "
        "WHERE "
            "`id` = " + id + " AND "
            "`token` = '" + token + "'"
    );

    User u;

    if(res != NULL && (row = mysql_fetch_row(res)) != NULL)
    {
        u.id = row[0];
        u.token = row[1];
        u.avatar = row[2];
        u.login = row[3];
        u.role = row[4];
    }

    mysql_free_result(res);

    return u;
};

Model::~Model() {
    mysql_close(mysql);
};