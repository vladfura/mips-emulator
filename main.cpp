#include <iostream>
#include <stdint.h>
#include <fstream>
#include "memory.h"
#include "core.h"
using namespace std;

int main(int argc, char **argv) {
    Core core;
	
	core.preparation();
	
	core.execute();
	
	return 0;
}
