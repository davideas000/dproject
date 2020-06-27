#include "dp_project/project.h"
#include "db_manager/db_manager.h"

#include <iostream>
#include <boost/system/error_code.hpp>

boost::filesystem::path dproject::Project::dproject_dir = "";

/*constructors/destructors*/
dproject::Project::Project() {
    dproject_dir = getenv("HOME");
    dproject_dir /= boost::filesystem::path{".dproject"};
}

dproject::Project::~Project() {}

boost::filesystem::path dproject::Project::get_dproject_dir() {
    return dproject_dir;
}

void dproject::Project::run(int argc, char const* argv[]) {
    options.parse_cmd(argc, argv);
    auto subcommands = options.get_parsed_options()["subcommand"].
        as< std::vector<std::string> >();

    if (subcommands[0] == "new") {
        if (subcommands.size() < 2) {
            std::cerr << "missing argument" << std::endl;
            exit(EXIT_FAILURE);
        }
        create(subcommands);
    }
    else if (subcommands[0] == "add") {
        if (subcommands.size() < 2) {
            std::cerr << "missing argument" << std::endl;
            exit(EXIT_FAILURE);
        }
        add(subcommands);
    }
    else {
        std::cerr << "error: invalid command" << std::endl;
    }
}

void dproject::Project::create(const std::vector<std::string> &args){
    namespace fs = boost::filesystem;
    fs::path template_dir(dproject_dir);
    template_dir /= "templates";
    template_dir /= args[1];
    if (fs::exists(template_dir) && fs::is_directory(template_dir)) {

        fs::path project_dir_name = args.size() > 2 ? args[2] : args[1];

        if (fs::exists(project_dir_name)) {
            std::cerr << "error: project directory " << project_dir_name << " already exists." 
                << "\nhint: probably there is a directory or file with that name."
                << std::endl;
            exit(EXIT_FAILURE);
        }

        //create project directory
        fs::create_directory(project_dir_name);

        // copy contents from template directory
        copy_directory_recursively(template_dir, project_dir_name);

        // create dproject local directory
        project_dir_name /= ".dproject";
        fs::create_directory(project_dir_name);
        std::cout << "creating " << project_dir_name << std::endl;

        //get config from Dprofile and store in local(to project) sqlite database
        template_dir /= "Dprofile";
        project_dir_name /= "dp_db";
        options.parse_config_file(template_dir, project_dir_name);
        std::cout << "creating " << project_dir_name << std::endl;
    }
    else {
        std::cerr << "template folder not found!" << std::endl;

        exit(EXIT_FAILURE);
    }
}

void dproject::Project::add(const std::vector<std::string> &args){
    namespace fs = boost::filesystem;
    fs::path path(dproject_dir);
    path /= "templates";

    std::unique_ptr<DbManager> dbm;

    if (fs::exists(".dproject")) {
        dbm = std::make_unique<DbManager>(".dproject/dp_db");
    }
    else if (fs::exists("../.dproject")) {
        dbm = std::make_unique<DbManager>("../.dproject/dp_db");
    }
    else if (fs::exists("../../.dproject")) {
        dbm = std::make_unique<DbManager>("../../.dproject/dp_db");
    }
    else if (fs::exists("../../../.dproject")) {
        dbm = std::make_unique<DbManager>("../../../.dproject/dp_db");
    }
    else {
        std::cerr << "project root not found" << std::endl;
        exit(EXIT_FAILURE);
    }

    path /= dbm->get("type");
    path /= "build_files";

    if (fs::exists(path) && fs::is_directory(path)) {

        if (fs::exists(args[1])) {
            std::cerr << "error: directory " << args[1] << " already exists." 
                << "\nhint: probably there is a directory or file with that name."
                << std::endl;
            exit(EXIT_FAILURE);
        }

        // select build file that will be put into directory
        if (args.size() > 2) {
            if (args[2] == "bin" || args[2] == "binary") {
                path /= "bin";
            }
            else if (args[2] == "sh" || args[2] == "shared") {
                path /= "shared_lib";
            }
            else if (args[2] == "st" || args[2] == "static") {
                path /= "static_lib";
            }
            else {
                std::cerr << "error: invalid command" << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        else {
            path /= "static_lib";
        }

        if (fs::exists(path)) {
            fs::create_directory(args[1]);
            std::string build_file = dbm->get("build_file");
            boost::filesystem::copy_file(path, args[1] + "/" + build_file);
        }
        else {
            std::cerr << "error: " << path << " not found!" << std::endl;
            exit(EXIT_FAILURE);
        }

    }
    else {
        std::cerr << "template folder not found!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

/////////////////////////////////////////////////////
////////////// utility functions ////////////////////
/////////////////////////////////////////////////////
void dproject::Project::copy_directory_recursively(const boost::filesystem::path &from, 
        const boost::filesystem::path &to) const {

    std::cout << "creating " << to << std::endl;
    boost::filesystem::create_directory(to);

    if (!boost::filesystem::is_empty(from)) {

        for (const auto &entry : boost::filesystem::directory_iterator(from)) {
            boost::filesystem::path p(to);
            p /= entry.path().filename();
            if (!boost::filesystem::is_directory( entry.path() )) {
                std::cout << "copying " << entry.path() << " to " << p << std::endl;
                boost::filesystem::copy_file(entry.path(), p);
            }
            else if(entry.path().filename() != "build_files"){
                copy_directory_recursively(entry.path(), p);
            }
        }
    }
}
