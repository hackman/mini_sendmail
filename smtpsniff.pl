#!/usr/bin/perl
use strict;
use warnings;
use IO::Socket;
use POSIX qw(setsid);

my $logfile = '/home/hackman/smtpsniff';

chdir '/' or die "cannot change to /:$!";
open STDIN,'/dev/null' or die "cannot read stdin:$!";
open STDOUT ,'>>/dev/null' or die "cannot write to stdout:$!";
open STDERR ,'>>/dev/null' or die "cannot write to stderr:$!";
defined(my $pid=fork) or die "cannot fork process:$!";
exit if $pid;
setsid;
#umask 0;
$0 = "smtpsniff";
open LOG, '>>', $logfile or die "Unable to open logfile!\n";
select((select(LOG), $| = 1)[0]);
my $sock = IO::Socket::INET->new(Listen    => 5,
                                 LocalAddr => "127.0.0.1",
                                 LocalPort => 25,
                                 Proto     => 'tcp');
die "Could not create socket: $!\n" unless $sock;

while (1) {
	# listen for connections
	while(my $s = $sock->accept()) {
		$s->autoflush(1);
		print $s "220 MM ESMTP Sniffer\n";
		while ( <$s> ) {
			print LOG $_;
			if ($_ =~ /^DATA/i) {
				print $s "354 End data with <CR><LF>.<CR><LF>\n";
			} elsif ($_ =~ /^QUIT/i) {
				print $s "221 Bye\n";
				close $s;
			} elsif ($_ =~ /^\.\r\n$/) {
				print $s "250 OK\n";
			} else {
				print $s "250 OK\n";
			}
		}
    	close $s;
	}
}


