/*
 * $XConsortium: parsedpy.c,v 1.9 94/04/17 20:37:51 hersh Exp $
 * $XFree86: xc/programs/xauth/parsedpy.c,v 3.0.4.1 1998/02/15 16:09:52 hohndel Exp $
 *
 * parse_displayname - utility routine for splitting up display name strings
 *
 * 
Copyright (c) 1989  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.
 * *
 * Author:  Jim Fulton, MIT X Consortium
 */
/*  Modified for stand-alone compiling by
 *  Peter 'Luna' Runestig <peter@runestig.com>
 */

/*
 * Incorporated into the SRP Telnet distribution 10/19/2000 by
 * Tom Wu <tjw@cs.stanford.edu>
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef FWD_X

#include <stdio.h>			/* for NULL */
#include <ctype.h>			/* for isascii() and isdigit() */
#include <string.h>			/* for strchr() and string routines */
#include <unistd.h>
#include <stdlib.h>
#ifdef hpux
#include <sys/utsname.h>		/* for struct utsname */
#endif
#include "Xauth.h"

/* explicitly set by Luna */
#define UNIXCONN 1

#if defined(UNIXCONN) || defined(LOCALCONN)
#define UNIX_CONNECTION "unix"
#define UNIX_CONNECTION_LENGTH 4
#endif

#ifdef WIN32
#include <X11/Xwinsock.h>
#endif

#ifdef USG
#define NEED_UTSNAME
#endif

#ifdef NEED_UTSNAME
#include <sys/utsname.h>
#endif

/*
 * Author:  Jim Fulton, MIT X Consortium
 *
 * _XGetHostname - similar to gethostname but allows special processing.
 */
int XmuGetHostname (buf, maxlen)
    char *buf;
    int maxlen;
{
    int len;
#ifdef WIN32
    static WSADATA wsadata;

    if (!wsadata.wVersion && WSAStartup(MAKEWORD(1,1), &wsadata))
	return -1;
#endif

#ifdef NEED_UTSNAME
    /*
     * same host name crock as in server and xinit.
     */
    struct utsname name;

    uname (&name);
    len = strlen (name.nodename);
    if (len >= maxlen) len = maxlen - 1;
    strncpy (buf, name.nodename, len);
    buf[len] = '\0';
#else
    buf[0] = '\0';
    (void) gethostname (buf, maxlen);
    buf [maxlen - 1] = '\0';
    len = strlen(buf);
#endif /* hpux */
    return len;
}

/*
 * private utility routines
 */

/*static*/ char *copystring (src, len)
    char *src;
    int len;
{
    char *cp;

    if (!src && len != 0) return NULL;
    cp = malloc (len + 1);
    if (cp) {
	if (src) strncpy (cp, src, len);
	cp[len] = '\0';
    }
    return cp;
}


char *get_local_hostname (buf, maxlen)
    char *buf;
    int maxlen;
{
    buf[0] = '\0';
    (void) XmuGetHostname (buf, maxlen);
    return (buf[0] ? buf : NULL);
}

#ifndef UNIXCONN
static char *copyhostname ()
{
    char buf[256];

    return (get_local_hostname (buf, sizeof buf) ? 
	    copystring (buf, strlen (buf)) : NULL);
}
#endif

/*
 * parse_displayname - display a display string up into its component parts
 */
int parse_displayname (displayname, familyp, hostp, dpynump, scrnump, restp)
    char *displayname;
    int *familyp;			/* return */
    char **hostp;			/* return */
    int *dpynump, *scrnump;		/* return */
    char **restp;			/* return */
{
    char *ptr;				/* work variables */
    int len;				/* work variable */
    int family = -1;			/* value to be returned */
    char *host = NULL;			/* must free if set and error return */
    int dpynum = -1;			/* value to be returned */
    int scrnum = 0;			/* value to be returned */
    char *rest = NULL;			/* must free if set and error return */
    int dnet = 0;			/* if 1 then using DECnet */

					/* check the name */
    if (!displayname || !displayname[0]) return 0;

					/* must have at least :number */
    ptr = strchr(displayname, ':');
    if (!ptr || !ptr[1]) return 0;
    if (ptr[1] == ':') {
	if (ptr[2] == '\0') return 0;
	dnet = 1;
    }


    /*
     * get the host string; if none is given, use the most effiecient path
     */

    len = (ptr - displayname);	/* length of host name */
    if (len == 0) {			/* choose most efficient path */
#if defined(UNIXCONN) || defined(LOCALCONN)
	host = copystring (UNIX_CONNECTION, UNIX_CONNECTION_LENGTH);
	family = FamilyLocal;
#else
	if (dnet) {
	    host = copystring ("0", 1);
	    family = FamilyDECnet;
	} else {
	    host = copyhostname ();
	    family = FamilyInternet;
	}
#endif
    } else {
	host = copystring (displayname, len);
	if (dnet) {
	    family = dnet;
	} else {
#if defined(UNIXCONN) || defined(LOCALCONN)
	    if (host && strcmp (host, UNIX_CONNECTION) == 0)
	      family = FamilyLocal;
	    else
#endif
	      family = FamilyInternet;
	}
    }

    if (!host) return 0;


    /*
     * get the display number; we know that there is something after the
     * colon (or colons) from above.  note that host is now set and must
     * be freed if there is an error.
     */

    if (dnet) ptr++;			/* skip the extra DECnet colon */
    ptr++;				/* move to start of display num */
    {
	register char *cp;

	for (cp = ptr; *cp && isascii(*cp) && isdigit(*cp); cp++) ;
	len = (cp - ptr);
					/* check present and valid follow */
	if (len == 0 || (*cp && *cp != '.')) {
	    free (host);
	    return 0;
	}
	
	dpynum = atoi (ptr);		/* it will handle num. as well */
	ptr = cp;
    }

    /*
     * now get screen number if given; ptr may point to nul at this point
     */
    if (ptr[0] == '.') {
	register char *cp;

	ptr++;
	for (cp = ptr; *cp && isascii(*cp) && isdigit(*cp); cp++) ;
	len = (cp - ptr);
	if (len == 0 || (*cp && *cp != '.')) {	/* all prop name */
	    free (host);
	    return 0;
	}

	scrnum = atoi (ptr);		/* it will handle num. as well */
	ptr = cp;
    }

    /*
     * and finally, get any additional stuff that might be following the
     * the screen number; ptr must point to a period if there is anything
     */

    if (ptr[0] == '.') {
	ptr++;
	len = strlen (ptr);
	if (len > 0) {
	    rest = copystring (ptr, len);
	    if (!rest) {
		free (host);
		return 1;
	    }
	}
    }

    /*
     * and we are done!
     */
    if (familyp)
	*familyp = family;
    if (hostp)
	*hostp = host;
    else
	free(host);
    if (dpynump)
	*dpynump = dpynum;
    if (scrnump)
	*scrnump = scrnum;
    if (restp)
	*restp = rest;
    else
	free(rest);
    return 1;
}

#endif /* FWD_X */
