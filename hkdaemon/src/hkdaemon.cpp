#include <opt3.hpp>

#include <Windows.h>

int main(const int argc, char** argv)
{
	try {
		opt3::ArgManager args{ argc, argv };



		return 0;
	} catch (...) {
		return 1;
	}
}
