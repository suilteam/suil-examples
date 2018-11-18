#include <suil/init.h>
#include <suil/cmdl.hpp>
#include <suil/config.h>
#include <suil/http/endpoint.h>
#include <suil/http/fserver.h>

using namespace suil;

int main(int argc, char *argv[])
{
    auto config = Config::load("../examples.conf.lua");
    sinfo("GUID: %s", utils::uuidstr()());
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose, config.get("config.basic.verbosity", 0)));
    // create a new HTTP endpoint listening on port 80
    http::TcpEndpoint<http::SystemAttrs> ep("/api",
            opt(name, config.get("config.basic.host", std::string("0.0.0.0"))),
            opt(port, config.get("config.basic.port", 1080)),
            opt(accept_backlog, 1000));

    http::FileServer fs(ep,
        opt(root, "/home/dc/Downloads"),
        opt(enable_send_file, true));

    // accessed via http://0.0.0.0:1080/api/hello
    ep("/hello")
    ("GET"_method)
    ([]() {
        // return just a string saying hello world
        return "Hello World";
    });

    ep("/hello/<string>")
    ("GET"_method)
    ([](std::string name) {
        // return just a string saying hello world
        return "Hello World " + name;
    });

    // accessed via GET http://0.0.0.0:1080/api/hello
    ep("/about")
    ("GET"_method)
    ([]() {
        // Simple About sting
        return APP_NAME " " APP_VERSION " Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018 Copyright (c) 2018";
    });

    // accessed via GET http://0.0.0.0:1080/api/json
    ep("/json")
    ("GET"_method)
    ([]() {
        // returning a json object will add the Content-Type: application/json header
        typedef decltype(iod::D(
                prop(name,    String),
                prop(version, String)
        )) JsonType;

        return JsonType{String{APP_NAME}, String{APP_VERSION}};
    });

    // <int> parameter via GET http://0.0.0.0:1080/api/check/integer
    ep("/check/<int>")
    ("GET"_method)
    ([](int num) {
        // demonstrates use of parameters and returning status codes
        sdebug("checking number %d", num);
        if (num > 100) {
            // number out of range
            return http::Status::BAD_REQUEST;
        }

        return http::Status::OK;
    });

    // route parameters, e.g http://0.0.0.0:1080/api/add/3/1
    ep("/add/<int>/<int>")
    ("GET"_method)
    ([](const http::Request& req, http::Response& resp, int a, int b) {
        // demonstrates use of parameters and returning status codes
        sdebug("adding numbers a=%d, b=%d", a, b);
        resp.appendf("a + b = %d\n", a+b);
    });

    ep("/post_json")
    ("POST"_method)
    ([](const http::Request& req, http::Response& resp) {
        // demonstrate parsing request body into a json object
        typedef decltype(iod::D(
            prop(name,  String),
            prop(email, String)
        )) JsonType;

        try {
            auto data = req.toJson<JsonType>();
            resp << "Hello " << data.name << ", we have registered your email '"
                 << data.email << "' :-)";
        }
        catch(...) {
            // invalid json data
            resp << "Invalid JSON data: " << Exception::fromCurrent().what();
            resp(http::Status::BAD_REQUEST);
        }
    });

    // parsing an HTTP form requires the middle-ware
    // http::SystemAttrs which handles form parsing and
    // the route need to be marked with the PARSE_FORM attribute
    ep("/post_form")
    ("POST"_method)
    .attrs(opt(PARSE_FORM, true)) // required for http to parse request as a form
    ([](const http::Request& req, http::Response& resp) {
        // demonstrate parsing request body into a json object
        typedef decltype(iod::D(
                prop(name,  String),
                prop(email, String)
        )) FormType;

        try {
            // extract parse form
            FormType data;
            http::RequestForm form(req);
            form >> data;
            resp << "Hello " << data.name << ", we have registered your email '"
                 << data.email << "' :-)";
        }
        catch(...) {
            // invalid json data
            resp << "Invalid form data: " << Exception::fromCurrent().what();
            resp(http::Status::BAD_REQUEST);
        }
    });

    // routes also support URL parameters
    // try http://{ip}:1080/api/params?name='MyName'&email='myemail@gmail.com'
    ep("/params")
    ("GET"_method)
    ([](const http::Request& req, http::Response& resp) {
        // parameters can be accessed via the request object
        auto name  = req.query<String>("name");
        auto email =  req.query<String>("email");
        if (name.empty() || email.empty()) {
            // email and name required
            resp << "Invalid Query: name and email required :-(\n";
            resp(http::Status::BAD_REQUEST);
        }
        else {
            // valid
            resp << "Welcome " << name << ", your email '" << email << "' is recognized :-)\n";
        }
    });

    // we need to listen before creating workers
    ep.listen();
    int nprocs = config.get("config.basic.nprocs", 1);
    sinfo("starting server with %d procs", nprocs);
    for (int i = 0; i < nprocs-1; ++i) {
        pid_t pid = mfork();
        if(pid < 0) {
            perror("Can't create new process");
            return 1;
        }
        if(pid == 0) {
            break;
        }
        sinfo("created worker %d", getpid());
    }

    return ep.start();
}

