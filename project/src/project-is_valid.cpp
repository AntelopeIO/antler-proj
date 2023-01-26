#include <antler/project/project.h>


#define TEST_POPULATED(X,Y) if(X.empty()) { os << Y << " is unpopulated.\n"; rv = false; }

namespace antler {
namespace project {

bool project::is_valid(std::ostream& os) {

   bool rv = true;

   // First, validate the members of this object.
   TEST_POPULATED( m_path, "path" );
   TEST_POPULATED( m_name, "name" );
   TEST_POPULATED( m_ver,  "version" );

   // Now validate: apps, libs, and tests.


   return rv;
}


} // namespace project
} // namespace antler
