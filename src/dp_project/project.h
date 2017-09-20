#ifndef DPROJECT_PROJECT_PROJECT_H
#define DPROJECT_PROJECT_PROJECT_H

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "options.h"

namespace dproject {
    class Project {

        public:
            Project();
            virtual ~Project();
            static boost::filesystem::path get_dproject_dir();
            void print_help(
                    const boost::program_options::options_description &generic_options) const;
            void create(const std::vector<std::string> &args);
            void add(const std::vector<std::string> &args);
            void run(int argc, char const* argv[]);
        private:
            /// \brief directory that contains dp cofigurations files and templates
            static boost::filesystem::path dproject_dir;
            dproject::Options options;
            void copy_directory_recursively(const boost::filesystem::path &from, 
                    const boost::filesystem::path &to) const;
            //void find_project_root();
    };
} /* namespace dprojcet */

#endif
