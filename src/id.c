/*
 * Copyright 1991 - 1994, Julianne Frances Haugh
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

/*
 * id - print current process user identification information
 *
 *	Print the current process identifiers.  This includes the
 *	UID, GID, effective-UID and effective-GID.  Optionally print
 *	the concurrent group set if the current system supports it.
 */

#include <config.h>

#include "rcsid.h"
RCSID(PKG_VER "$Id: id.c,v 1.6 2000/08/26 18:27:18 marekm Exp $")

#include <sys/types.h>
#include <stdio.h>
#include <grp.h>
#include <pwd.h>
#include "defines.h"

/* local function prototypes */
static void usage(void);

static void
usage(void)
{
#ifdef HAVE_GETGROUPS
	fprintf(stderr, _("usage: id [ -a ]\n"));
#else
	fprintf(stderr, _("usage: id\n"));
#endif
	exit(1);
}

/*ARGSUSED*/
int
main(int argc, char **argv)
{
	uid_t ruid, euid;
	gid_t rgid, egid;
	int i;
/*
 * This block of declarations is particularly strained because of several
 * different ways of doing concurrent groups.  Old BSD systems used int
 * for gid's, but short for the type passed to getgroups().  Newer systems
 * use gid_t for everything.  Some systems have a small and fixed NGROUPS,
 * usually about 16 or 32.  Others use bigger values.
 */
#ifdef HAVE_GETGROUPS
	GETGROUPS_T groups[NGROUPS_MAX];
	int	ngroups;
	int	aflg = 0;
#endif
	struct	passwd	*pw;
	struct	group	*gr;

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

#ifdef HAVE_GETGROUPS
	/*
	 * See if the -a flag has been given to print out the
	 * concurrent group set.
	 */

	if (argc > 1) {
		if (argc > 2 || strcmp (argv[1], "-a"))
			usage();
		else
			aflg = 1;
	}
#else
	if (argc > 1)
		usage();
#endif

	ruid = getuid();
	euid = geteuid();
	rgid = getgid();
	egid = getegid();

	/*
	 * Print out the real user ID and group ID.  If the user or
	 * group does not exist, just give the numerical value.
	 */

	pw = getpwuid(ruid);
	if (pw)
		printf(_("uid=%d(%s)"), (int) ruid, pw->pw_name);
	else
		printf(_("uid=%d"), (int) ruid);

	gr = getgrgid(rgid);
	if (gr)
		printf(_(" gid=%d(%s)"), (int) rgid, gr->gr_name);
	else
		printf(_(" gid=%d"), (int) rgid);

	/*
	 * Print out the effective user ID and group ID if they are
	 * different from the real values.
	 */

	if (ruid != euid) {
		pw = getpwuid(euid);
		if (pw)
			printf(_(" euid=%d(%s)"), (int) euid, pw->pw_name);
		else
			printf(_(" euid=%d"), (int) euid);
	}
	if (rgid != egid) {
		gr = getgrgid(egid);
		if (gr)
			printf(_(" egid=%d(%s)"), (int) egid, gr->gr_name);
		else
			printf(_(" egid=%d"), (int) egid);
	}

#ifdef HAVE_GETGROUPS
	/*
	 * Print out the concurrent group set if the user has requested
	 * it.  The group numbers will be printed followed by their
	 * names.
	 */

	if (aflg && (ngroups = getgroups (NGROUPS_MAX, groups)) != -1) {

		/*
		 * Start off the group message.  It will be of the format
		 *
		 *	groups=###(aaa),###(aaa),###(aaa)
		 *
		 * where "###" is a numerical value and "aaa" is the
		 * corresponding name for each respective numerical value.
		 */

		printf(_(" groups="));
		for (i = 0; i < ngroups; i++) {
			if (i)
				putchar(',');

			gr = getgrgid(groups[i]);
			if (gr)
				printf("%d(%s)", (int) groups[i], gr->gr_name);
			else
				printf("%d", (int) groups[i]);
		}
	}
#endif

	/*
	 * Finish off the line.
	 */

	putchar('\n');
	exit(0);
	/*NOTREACHED*/
}
