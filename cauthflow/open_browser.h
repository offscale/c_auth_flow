#ifndef OAUTH2_OPEN_BROWSER_H
#define OAUTH2_OPEN_BROWSER_H

/* Reference https://github.com/microsoft/cpprestsdk/blob/7fbb08c491f9c8888cc0f3d86962acb3af672772/Release/samples/Oauth1Client/Oauth1Client.cpp */
#include <stdlib.h>
#include <c89stringutils_string_extras.h>

#ifdef __linux__
#include <stdio.h>
#include <resolv.h>

#elif defined(macintosh) || defined(Macintosh) || defined(__APPLE__) && defined(__MACH__)

#include <CoreFoundation/CFBundle.h>
#include <ApplicationServices/ApplicationServices.h>

#elif defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)

#include <ShlObj.h>
#include <winbase.h>
#include <Shellapi.h>

#endif

static
void open_browser(const char *url)
{
#ifdef __linux__
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

#else
#if defined(macintosh) || defined(Macintosh) || defined(__APPLE__) && defined(__MACH__)
    CFURLRef cf_url = CFURLCreateWithBytes (
            NULL,                        /* allocator */
            (UInt8*)url,                 /* URLBytes */
            (signed long)strlen(url),    /* length */
            kCFStringEncodingASCII,      /* encoding */
            NULL                         /* baseURL */
    );
    LSOpenCFURLRef(cf_url,0);
    CFRelease(cf_url);
#elif defined(_WIN32) || defined(__WIN32__) || defined(__WINDOWS__)
    ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL);
#endif

#endif
}

#ifdef TEST_OPEN_BROWSER
int main() {
    const char *url = "http://www.google.com";
    open_browser(url);
}
#endif

#endif /* OAUTH2_OPEN_BROWSER_H */
