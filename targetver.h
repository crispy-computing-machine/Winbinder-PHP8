#pragma once


#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10

/** Windows Constants **/
#define WINVER _WIN32_WINNT_WIN10         // Windows 10
#define _WIN32_WINNT _WIN32_WINNT_WIN10   // Windows XP
#define _WIN32_WINDOWS _WIN32_WINNT_WIN10 // Windows XP
#define _WIN32_IE _WIN32_WINNT_WIN10      // Internet Explorer 6

// NTDDI_VERSION
#define NTDDI_VERSION 0x0A000000