/* Reference
 * https://github.com/microsoft/cpprestsdk/blob/7fbb08c491f9c8888cc0f3d86962acb3af672772/Release/samples/Oauth1Client/Oauth1Client.cpp
 */
#include <stdlib.h>

#if defined(__linux) || defined(__linux__) || defined(linux)
#include <c89stringutils_string_extras.h>
#include <resolv.h>
#include <stdio.h>

#elif defined(macintosh) || defined(Macintosh) ||                              \
    defined(__APPLE__) && defined(__MACH__)

#include <ApplicationServices/ApplicationServices.h>
#include <CoreFoundation/CFBundle.h>

#elif defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)

#include <ShlObj.h>
#include <basetsd.h>
#include <winbase.h>
#include <windef.h>

#include <Shellapi.h>

#endif

#include "open_browser.h"

void open_browser(const char *url) {
#ifndef OPEN_BROWSER
  printf("Authenticate in your browser with:\n\t%s\n", url);
#elif defined(__linux) || defined(__linux__) || defined(linux)
  /* On linux xdg-open is a command that opens the
   preferred application for the type of file or url.
   For more information use: man xdg-open on the
   terminal command line.

   In OAuth2 we open the browser for the user to
   enter their credentials. */
  char *browser_cmd_string;
  asprintf(&browser_cmd_string, "xdg-open \"%s\"", url);
  puts(browser_cmd_string);
  (void)system(browser_cmd_string);
  free(browser_cmd_string);

#else
#if defined(macintosh) || defined(Macintosh) ||                                \
    defined(__APPLE__) && defined(__MACH__)
  CFURLRef cf_url = CFURLCreateWithBytes(NULL,                   /* allocator */
                                         (UInt8 *)url,           /* URLBytes */
                                         (long)strlen(url),      /* length */
                                         kCFStringEncodingASCII, /* encoding */
                                         NULL                    /* baseURL */
  );
  LSOpenCFURLRef(cf_url, 0);
  CFRelease(cf_url);
#elif defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
  ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#endif /* defined(macintosh) || defined(Macintosh) || defined(__APPLE__) &&    \
          defined(__MACH__) */

#endif /* defined(__linux) || defined(__linux__) || defined(linux) */
}
