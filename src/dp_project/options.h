#ifndef DPRJECT_PROJECT_CMDOPTONIOS_H
#define DPRJECT_PROJECT_CMDOPTONIOS_H

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

namespace dproject {
    class Options {

        public:
            //Options(int argc, char const* argv[]);
            Options();
            virtual ~Options();

            void parse_cmd(int agrc, char const* argv[]);

            /*
             * \brief parse the given config file and store values in given sqlite database
             * \param config_file_path path to config file
             * \param db_path path to sqlite database
             */
            void parse_config_file(const boost::filesystem::path& config_file_path,
                    const boost::filesystem::path& db_path);
            boost::program_options::variables_map get_parsed_options() const;

        private:
            boost::program_options::variables_map parsed_options;
            void print_help(const boost::program_options::options_description &generic_options) const;
            //void extract_command_and_target(const std::string& str,  // ====
            //std::vector<std::string>& values_out) const; // ====
            void extract_command_and_target(const std::string& str, 
                    std::vector<std::pair<std::string, std::string> >& values_out) const;
    };
} /* namespace dproject */

#endif
