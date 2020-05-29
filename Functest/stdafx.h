#pragma once
#define _CRT_SECURE_NO_WARNINGS
#ifndef  _STDAFX_H
#define _STDAFX_H

#include <WS2tcpip.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <list>
#include <map>
#include <io.h>
#include <conio.h>

#include <mutex>
#include <thread>

#include "libMag.h"
#include "paraParser.h"
#include "register.h"
#include "csCommunication.h"

typedef int(*FUNADDR)(...);

#endif // ! _STDAFX_H
