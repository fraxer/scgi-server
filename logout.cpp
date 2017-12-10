class LogoutController: public Controller {
    public:
        string run(map<const char*, const char*, cmp_str>* parms) {
            addHeader("Set-Cookie: id=; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT");
            addHeader("Set-Cookie: token=; path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT");

            return redirect(302, "Moved Temporarily", "/index.d", "/");
        }
};