#ifdef  SAMODIVA_TEST_BUILD
#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_NO_EXCEPTIONS
#include <doctest/doctest.h>
#include <iostream>

int main(int argc, char** argv) {
	doctest::Context context; // initialize

	// defaults
	context.setOption("sort", "name"); // sort the test cases by their name

	context.applyCommandLine(argc, argv);

	// overrides
	context.setOption("no-breaks", true);             // don't break in the debugger when assertions fail

	int res = context.run(); // run

	if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
		return res;          // propagate the result of the tests

	std::cout << "Press any key to exit" << std::endl;
	char x;
	std::cin >> x;
	
}

TEST_SUITE("Suite for doctest");

SCENARIO("making sure tests work")
{
	CHECK(true);
}

TEST_SUITE_END();

#endif // SAMODIVA_TEST_BUILD