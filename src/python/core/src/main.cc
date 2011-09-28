/**
 * @file src/python/core/src/main.cc 
 * @author <a href="mailto:andre.dos.anjos@cern.ch">Andre Anjos</a> 
 *
 * @brief Combines all modules to make up the complete bindings
 */

#include <boost/python.hpp>

using namespace boost::python;

void bind_version_info();
void bind_core_exception();
void bind_core_logging();
void bind_core_object();
void bind_core_tensor();
void bind_core_profiler();

BOOST_PYTHON_MODULE(libpytorch_core) {
  docstring_options docopt; 
# if !defined(TORCH_DEBUG)
  docopt.disable_cpp_signatures();
# endif
  scope().attr("__doc__") = "Torch core classes and sub-classes";
  bind_version_info();
  bind_core_exception();
  bind_core_logging();
  bind_core_object();
  bind_core_tensor();
  bind_core_profiler();
}
