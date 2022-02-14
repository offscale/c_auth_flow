#include <cauthflow_configure.h>
#include <greatest.h>
#include <stdbool.h>

#include <google_auth.h>

#ifdef _MSC_VER
#define NUM_FORMAT "%zu"
typedef size_t num_type;
#elif defined(__linux__) || defined(linux) || defined(__linux)
#define NUM_FORMAT "%d"
typedef int num_type;
#else
#define NUM_FORMAT "%lu"
typedef unsigned long num_type;
#endif /* _MSC_VER */

TEST x_is_directory_should_be_true(void) {
  /* TODO: Actually test things */
  get_google_auth(CLIENT_ID, CLIENT_SECRET, /*refresh_token*/ NULL);
  ASSERT_EQ(0, 0);
  PASS();
}

/* Suites can group multiple tests with common setup. */
SUITE(cauthflow_suite) { RUN_TEST(x_is_directory_should_be_true); }
