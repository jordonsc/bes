/**
 * This file exists solely to ensure that header files are included by a source file, to prevent IDEs (cough) CLion..
 * (cough) doing stupid things and thinking libraries don't exist.
 *
 * Seems to be an issue where a header file is NOT included by a .cc file, this library contains mostly templates
 * (headers) so 90% of it reports as errors. Bad CLion, Bad!
 */

#include "exception.h"
#include "rpc_application.tcc"
#include "rpc_client.tcc"
#include "rpc_controller.tcc"
#include "rpc_handler.tcc"
