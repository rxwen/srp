/*
 * Copyright 1989 - 1991, Julianne Frances Haugh
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of Julianne F. Haugh nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY JULIE HAUGH AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL JULIE HAUGH OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <config.h>
#include "prototypes.h"
#include "defines.h"
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "getdef.h"

#include "rcsid.h"
RCSID("$Id: mail.c,v 1.1 2000/12/17 05:34:11 tom Exp $")

#define MSG_NEWMAIL "You have new mail."
#define MSG_MAIL "You have mail."
#define MSG_NOMAIL "No mail."

void
mailcheck()
{
	struct stat statbuf;
	char *mailbox;

	if (!getdef_bool("MAIL_CHECK_ENAB", 1))
		return;
#ifndef NOISY_SHELL
	/*
	 * Check incoming mail in Maildir format - J.
	 */
	if ((mailbox = getenv("MAILDIR"))) {
		char *newmail;

		newmail = xmalloc(strlen(mailbox) + 5);
		sprintf(newmail, "%s/new", mailbox);
		if (stat(newmail, &statbuf) != -1 && statbuf.st_size != 0) {
			free(newmail);
			if (statbuf.st_mtime > statbuf.st_atime) {
				puts(MSG_NEWMAIL);
				return;
			}
		}
		free(newmail);
	}

	if (!(mailbox = getenv("MAIL")))
		return;

	if (stat(mailbox, &statbuf) == -1 || statbuf.st_size == 0)
		puts(MSG_NOMAIL);
	else if (statbuf.st_atime > statbuf.st_mtime)
		puts(MSG_MAIL);
	else
		puts(MSG_NEWMAIL);
#endif
}

