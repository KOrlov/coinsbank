// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>



// reference additional headers your program requires here


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "crt310/IDCCRT310.h"
#include "scn83/CIMSCN83.h"
#include "cdm4000/cdm4000.h"
#include <boost/filesystem.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include "XFSAPI.H"
#include "xfs.h"