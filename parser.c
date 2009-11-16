#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include "version.h"


/* Globals. */
static char* argv0;
static int parse_message, verbose, debug;
static int got_a_recipient;
static void parse_for_recipients( char* message, char** envp );
static void add_recipient( char* recipient, int len, char** envp );
static void send_command( char* command );


int main( int argc, char** argv , char** envp) {
//	char mes[5000] = "To: hristo.p@siteground.com, dimo@siteground.com, svetlio@siteground.com\nSubject:Testing\nDate: Mon, 25 Aug 2008 08:30:09 -0500\nFrom: My Credit Headquarters Registration <admin@mycreditheadquarters.com>\nReply-to: <admin@mycreditheadquarters.com>\nMessage-ID: <bbc84ce4c2efd3a04df12ef9818dd097@www.mycreditheadquarters.com>\nX-Priority: 3\nX-Mailer: PHPMailer [version 1.73]\nMIME-Version: 1.0\nContent-Transfer-Encoding: 8bit\nContent-Type: text/plain; charset=\"iso-8859-1\"\n\nGreetings test test,\nThank you for applying for registration with us. We have\nreceived your request and we will process it as soon as you\nconfirm your email address by clicking on the following\nhyperlink:\n";
	char mes[5000] = "To: hristo.p@siteground.com, dimo@siteground.com, svetlio@siteground.com, mm@siteground.com\nSubject: test 6\nFrom: hristo.p@siteground.com\nTesting.\n.";
	char *info = mes;
	parse_for_recipients(info, envp);
}

static void parse_for_recipients( char* message, char** envp ) {
	char *pos = NULL, *to = NULL, *cc = NULL, *bcc = NULL;

	// search for To:
	if (pos = strstr(message, "To:"))		to=pos;
	if (!to)
		if (pos = strstr(message, "to:"))	to=pos;
	if (!to)
		if (pos = strstr(message, "TO:"))	to=pos;


	// search for Cc:
	if (pos = strstr(message, "Cc:"))		cc=pos;
//	if (!cc)
		if (pos = strstr(message, "CC:"))	cc=pos;
// 	if (!cc)
		if (pos = strstr(message, "\ncc:"))	cc=pos;

	// search for Bcc
	if (pos = strstr(message, "Bcc:"))		bcc=pos;
// 	if (!bcc)
		if (pos = strstr(message, "BCC:"))	bcc=pos;
// 	if (!bcc)
		if (pos = strstr(message, "bcc:"))	bcc=pos;

	// search for recipients in the found lines
	if ( to )	add_recipient(to, 3, envp);
	if ( cc )	add_recipient(cc, 3, envp);
	if ( bcc )	add_recipient(bcc, 4, envp);
}

static void send_command( char* command ) {
	(void) fprintf( stderr, "<<<< %s\n", command );
}

static void add_recipient( char* message, int chars_to_remove, char** envp ) {
	char buffer[1000];
	char buf[1000];
	char *space_check = NULL;
	char *to_buf = buffer;
	int len = strlen(message) - chars_to_remove;
	int sec_check = 0;
	int status;
	int found_char = 0;
	int skip_to_comma = 0;

	// Clear the buffer
 	memset(buffer, 0x00, sizeof(buffer));
	memset(buf, 0x00, sizeof(buf));
	message += chars_to_remove;

	// Walking trough the message, char by char
	while (len >= 0) {
/*
  We have to match lines like these:
	To: "mm@siteground.com" <mm@siteground.com>
	To: "mm@siteground.com" <mm@siteground.com>, m.m@siteground.com
*/
		if ( *message == ',' || *message == '\n' || *message == '\r' || *message == '\0' ) {
			// clear the skip flag
			if ( *message == ',' )
				skip_to_comma = 0;
			// do not print if the buffer is empty or containing :(To:, Bcc:, etc.)
			if ( strlen(buffer) > 0 && strchr(buffer, ':') == NULL) {
				(void) snprintf( buf, sizeof(buf), "RCPT TO: %s", buffer );
				send_command( buf );
//				status = read_response();
				status = 250;
				if ( status != 250  && status != 251 )
					(void) fprintf( stderr,  "%s: unexpected response %d to RCPT TO command\n", argv0, status );
				memset(buffer, 0x00, sizeof(buffer));
				memset(buf, 0x00, sizeof(buf));
				sec_check=0;
			}
			// clear the buffer
			to_buf = buffer;
			// If this is the end of the line we stop searching for recipients
			if ( *message == '\n' ) break;
		} else {
			// possible hacking attempt
			if ( sec_check > 999 )
				exit( 12 );
			// skip every character until we find comma
			if ( skip_to_comma ) {
				// goto next character in the message
				message++;
				len--;
				continue;
			}
			if ( found_char == 0 && *message == '@' )
				found_char = 1;
			if ( found_char && *message == ' ' ) {
				space_check = message;
				space_check++;
				if (*space_check != ',' && *space_check != ' ' )
					found_char = 1;
				else {
					skip_to_comma = 1;
					found_char = 0;
				}
				// goto next character in the message
				message++;
				len--;
				continue;
			}
			// copy the current character into the TO buffer
 			if ( *message != '"' ) {
				*to_buf = *message;
				// goto next character in the TO buffer
				to_buf++;
				sec_check++;
 			}
		}
//	"mm@siteground.com" <mm@siteground.com>, m.m@siteground.com, "mm@siteground.com" <mm@siteground.com>, m.m@siteground.com
		// goto next character in the message
		message++;
		len--;
	}
	got_a_recipient++;
}
