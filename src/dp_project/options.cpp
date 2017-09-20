#include <iostream>
#include <iomanip>
#include <regex>
#include "options.h"
#include "version.h"
#include "project.h"
#include "db_manager/db_manager.h"

/*constructors and destructors*/
dproject::Options::Options() {}

dproject::Options::~Options() {}

void dproject::Options::parse_cmd(int argc, char const* argv[]) {
    //general options
    boost::program_options::options_description generic_option{"Options"};
    generic_option.add_options()
        ("help,h", "Display help message.")
        ("version", "Display version number.");

    // options that will capture subcommnds
    boost::program_options::options_description subcommand_desc{"Subcommand:"};
    subcommand_desc.add_options()
        ("subcommand", boost::program_options::value< std::vector<std::string> >(), 
         "subcommand");

    boost::program_options::options_description all{"all options"};
    all.add(generic_option).add(subcommand_desc);

    boost::program_options::positional_options_description pos_desc;
    pos_desc.add("subcommand", -1);

    boost::program_options::store(
            boost::program_options::command_line_parser(argc, argv).options(all).
            positional(pos_desc).run(), parsed_options);

    boost::program_options::notify(parsed_options);

    if (parsed_options.count("help")) {
        print_help(generic_option);
    }
    else if (parsed_options.count("version")) {
        std::cout << "DProject:  " << DPROJECT_VERSION << std::endl;
    }
    else if (!parsed_options.count("subcommand")) {
        print_help(generic_option);
        exit(EXIT_SUCCESS);
    }
}

void dproject::Options::parse_config_file(const boost::filesystem::path& config_file_path,
        const boost::filesystem::path& db_path)
{
    namespace po = boost::program_options;
    namespace fs = boost::filesystem;

    if (!fs::exists(config_file_path)) {
        std::cerr << "error: Dprofile not found" << std::endl;
        exit(EXIT_FAILURE);
    }

    po::options_description config_options{"Project configuration"};
    config_options.add_options()
        ("project", "project name")
        ("description", "project description")
        ("type", "project type")
        ("version", "initial version")
        ("build_file", "project build file name")
        ("insert_library", "command to insert a library in the project")
        ("insert_include_directory", "command to insert a include directory in the project")
        ("bump_version", "command to bump version number");

    std::ifstream config_file{config_file_path.string()};
    po::store(po::parse_config_file(config_file, config_options), parsed_options);
    po::notify(parsed_options);

    std::vector<std::pair<std::string, std::string> > values{{"name", "project"}, 
        {"description", "no description"}, {"type", "not defined"}, 
        {"version", "0.0.0"}, {"build_file", "none"}};

    if (parsed_options.count("project")) {
        values[0].second = parsed_options["project"].as<std::string>();
    }
    if (parsed_options.count("description")) {
        values[1].second = parsed_options["description"].as<std::string>();
    }
    if (parsed_options.count("type")) {
        values[2].second = parsed_options["type"].as<std::string>();
    }
    if (parsed_options.count("version")) {
        values[3].second = parsed_options["version"].as<std::string>();
    }
    if (parsed_options.count("build_file")) {
        values[4].second = parsed_options["build_file"].as<std::string>();
    }

    dproject::DbManager dbm{db_path.string()};
    dbm.save(values);
    //dbm.create_tables();
    //dbm.insert_into_table(values, "project", true);

    values = {{"insert_library_command", "not defined"}, {"lib_target", "not defined"}};
    //values = {{"insert_static_library", "kj" // ====
    if (parsed_options.count("insert_library")) {

        extract_command_and_target(parsed_options["insert_library"].as<std::string>(),
                values);

    }
    //dbm.insert_into_table(values, "commands"); // ====
    dbm.save(values);

    values = {{"insert_include_directory_com", "not defined"}, {"inc_target", "not defined"}};
    if (parsed_options.count("insert_include_directory")) {

        extract_command_and_target(parsed_options["insert_include_directory"].as<std::string>(),
                values);

    }
    //dbm.insert_into_table(values, "commands"); // ====
    dbm.save(values);

    values = {{"bump_version_com", "not defined"}, {"version_target", "not defined"}};
    if (parsed_options.count("bump_version")) {
        extract_command_and_target(parsed_options["bump_version"].as<std::string>(),
                values);
    }
    //dbm.insert_into_table(values, "commands"); // ====
    dbm.save(values);

}

boost::program_options::variables_map dproject::Options::get_parsed_options() const {
    return parsed_options;
}

///////////////////////////////////////////
// utility functions
///////////////////////////////////////////

void dproject::Options::extract_command_and_target(const std::string& str, 
        std::vector<std::pair<std::string, std::string> >& values_out) const {

    std::regex re{"(.*?)\\s+>>\\s+(.*?)"};
    std::smatch result;

    if (std::regex_match(str, result, re)) {
        values_out[0].second  = result[1].str();
        values_out[1].second  = result[2].str();
    }
}

void dproject::Options::print_help(
        const boost::program_options::options_description &generic_options) const {

    // print help message header
    std::cout << "Create projects from templates" << std::endl
        << "Usage: dproject [options] command args..." << std::endl;

    // print simple options
    std::cout << std::endl << generic_options << std::endl;

    //define subcommands
    std::vector< std::pair<std::string, std::string> > subcommands{
        {"new", "Create a new project based on template."},
        {"add", "Add a new directory to project, and put \n\t\t\ta build file based on template."}
    };

    int default_line_width = 22;

    std::cout << "Commands:" << std::endl;
    for (auto &subcommand : subcommands) {
        // print formatted output :(
        std::cout << "  " << std::setw(default_line_width) << std::left
            << subcommand.first << subcommand.second << std::endl;
    }
    std::cout << std::endl;

    //get projects templates name and descripton from template.txt
    boost::filesystem::path user_templates(Project::get_dproject_dir());
    user_templates /= "templates.txt";
    std::ifstream file{user_templates.string()};
    if (!file) {
        std::cerr << "ERROR could not find "
            << user_templates << std::endl;
        exit(EXIT_FAILURE); 
    }

    //print templates
    std::cout << "Templates:" << std::endl;

    std::string subcommand;
    std::string description;
    while (file >> subcommand) {
        std::getline(file, description);

        std::stringstream ss;
        bool nskip = false;
        // remove leading whitespace.
        std::for_each(description.begin(), description.end(), 
                [&ss, &nskip](char c) {
                if (c != ' ' || nskip)
                {
                ss << c;
                nskip = true;
                }
                });

        // lenght of field that subcommand is displayed
        // this is just for output formatting, default of boost::program_options is 22
        int opt_fmt_len = subcommand.length() < 21 ? 22 : subcommand.length() + 2;

        //print templates name and description;
        std::cout << "  " 
            << std::left << std::setw(opt_fmt_len)
            << subcommand 
            << ss.str()
            << std::endl;
    }

    std::cout << std::endl;
}

