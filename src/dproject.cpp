#include <iostream>
#include <boost/exception/all.hpp>

#include "dp_project/project.h"

using namespace std;
using namespace dproject;
using namespace boost;

int main(int argc, char const* argv[])
{
    //general options
    Project project;
    try {
        project.run(argc, argv);
    } catch (const exception_detail::clone_impl<
            boost::exception_detail::error_info_injector<
            boost::program_options::unknown_option> > &e) {
        cerr << e.what() << endl;
    }
    catch (boost::exception_detail::clone_impl<
            boost::exception_detail::error_info_injector<
            boost::bad_any_cast> > &e) {
        //cerr << e.what() << "\n" << e.stack_trace() << endl;
        cerr << diagnostic_information(e) << endl;
    }

    return 0;
}
