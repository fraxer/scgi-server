class NotFoundView : public View {
    public:
        inline string templ() {
            return
            "<!doctype>"
            "<html>"
                "<head>"
                    "<title>404</title>"
                "</head"
                "<body>"
                    "404!"
                "</body>"
            "</html>";
        }
};

class NotFoundController: public Controller {
    public:
        string run(map<const char*, const char*, cmp_str>* parms) {
            if(strcmp(setvalc(parms, "REQUEST_METHOD"), "GET") != 0)
                return "";

            addHeader("Content-Type:text/html; charset=UTF-8");

            statusCode = 404;
            statusMessage = "Not Found";

            NotFoundView view;

            return view.templ();
        }
};