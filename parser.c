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
static void parse_for_recipients( char* message );
static void add_recipient( char* recipient, int len );

int main( int argc, char** argv , char** envp) {
	char mes[5000] = "To: mm@yuhu.biz\nSubject: Business Credit Tradelines Corporate Credit Trade Lines - Your Registration is Pending Approval\nDate: Mon, 25 Aug 2008 08:30:09 -0500\nFrom: My Credit Headquarters Registration <admin@mycreditheadquarters.com>\nReply-to: My Credit Headquarters Registration <admin@mycreditheadquarters.com>\nMessage-ID: <bbc84ce4c2efd3a04df12ef9818dd097@www.mycreditheadquarters.com>\nX-Priority: 3\nX-Mailer: PHPMailer [version 1.73]\nMIME-Version: 1.0\nContent-Transfer-Encoding: 8bit\nContent-Type: text/plain; charset=\"iso-8859-1\"\n\nGreetings test test,\nThank you for applying for registration with us. We have\nreceived your request and we will process it as soon as you\nconfirm your email address by clicking on the following\nhyperlink:\n";
	char *info = mes;
	parse_for_recipients(info);
}

static void parse_for_recipients( char* message ) {
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
	if ( to )	add_recipient(to, 3);
	if ( cc )	add_recipient(cc, 3);
	if ( bcc )	add_recipient(bcc, 4);
}


static void add_recipient( char* message, int chars_to_remove ) {
	char buffer[1000];
	char buf[1000];
	char *to_buf = buffer;
	int len = strlen(message) - chars_to_remove;
	int sec_check = 0;
	int status;
	// nulirame si bufera
 	memset(buffer, 0x00, sizeof(buffer));
	memset(buf, 0x00, sizeof(buf));
	message += chars_to_remove;

	// obhojdame message-a
	while (len >= 0) {
		// skip whitespaces
// 		if ( *message != ' ' && *message != '\t' ) {
			if ( *message == ',' || *message == '\n' || *message == '\r' || *message == '\0' ) {
				// do not print if the buffer is empty or containing :(To:, Bcc:, etc.)
 				if ( strlen(buffer) > 0 && strchr(buffer, ':') == NULL) {
					(void) snprintf( buf, sizeof(buf), "RCPT TO: %s", buffer );
					printf("%s\n", buf);
//  					status = read_response();
					status = 250;
					if ( status != 250  && status != 251 ) {
						(void) fprintf( stderr,  "%s: unexpected response %d to RCPT TO command\n", argv0, status );
// 						exit( 1 );
					}
					memset(buffer, 0x00, sizeof(buffer));
					memset(buf, 0x00, sizeof(buf));
					sec_check=0;

				}
				// nulirame poziciqta na bufera
				to_buf = buffer;
				// exitvame v kraq na reda i ne obrabotvame poveche redove
				if ( *message == '\n' ) break;
			} else {
				// possible hacking attempt
				if ( sec_check > 999 ) 
					exit( 12 );
				// kopirame tekushtta stoinost ot masiva message_pos v masiva to_buf
// 				if ( *message != '<' && *message != '>' ) {
					*to_buf = *message;
					// mestim se na sledvashtata poziciq v masiva
					to_buf++;
					sec_check++;
// 				}
			}
//		}
		// mestim se na sledvashtiq char
		message++;
		len--;
	}
	got_a_recipient++;
}
