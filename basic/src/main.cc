#include <suil/init.h>
#include <suil/cmdl.h>
#include <suil/config.h>
#include <suil/http/endpoint.h>
#include <suil/http/fserver.h>
#include <suil/http/middlewares.h>

using namespace suil;

static int runServer(String&& config);

static void cmd_Run(cmdl::Parser& parser) {
    // run command
    cmdl::Cmd run{"run", "start running basic example"};
    run << cmdl::Arg{"config", "the configuration file to use with the example",
                     'C', false, false};
    run([](cmdl::Cmd& cmd){
        // hand run command
        String config = cmd.getvalue<String>("config", "res/config.lua").dup();
        runServer(std::move(config));
    });
    parser.add(std::move(run));
}

int main(int argc, char *argv[])
{
    cmdl::Parser parser(APP_NAME, APP_VERSION, "suil library example demonstrating basic REST functionality");
    try {
        // add commands and run
        cmd_Run(parser);
        parser.parse(argc, argv);
        parser.handle();

        return EXIT_SUCCESS;
    }
    catch(...) {
        // unhandled Exception
        serror("unhandled error %s", Exception::fromCurrent().what());
        return EXIT_FAILURE;
    }
}

int runServer(String&& configFile)
{
    auto config = Config::load(configFile());
    sinfo("GUID: %s", utils::uuidstr()());
    suil::init(opt(printinfo, false));
    log::setup(opt(verbose, config.get("config.verbosity", 1)));
    // create a new HTTP endpoint listening on port 80
    http::TcpEndpoint<http::SystemAttrs, http::mw::EndpointAdmin> ep("/",
            opt(name, config.get("config.http.host", std::string("0.0.0.0"))),
            opt(port, config.get("config.http.port", 1080)),
            opt(accept_backlog, 1000));

    // accessed via http://0.0.0.0:1080/api/hello
    ep("/hello")
    ("GET"_method)
    (Desc{"Return message hello world"})
    ([]() {
        // return just a string saying hello world
        return "Hello World";
    });

    ep("/hello/{string}")
    ("GET"_method)
    ([](std::string name) {
        // return just a string saying hello world
        return "Hello World " + name;
    });

    // accessed via GET http://0.0.0.0:1080/about
    ep("/about")
    ("GET"_method)
    ([]() {
        // Simple About sting
        return APP_NAME " " APP_VERSION " Copyright (c) 2018";
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
    ep("/check/{int}")
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
    ep("/add/{int}/{int}")
    ("GET"_method)
    (Desc{"Adds the given two values and returns results"})
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

    ep.middleware<http::mw::EndpointAdmin>().setup(ep);

    // we need to listen before creating workers
    ep.listen();
    int nprocs = config.get("config.nprocs", 1);
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

