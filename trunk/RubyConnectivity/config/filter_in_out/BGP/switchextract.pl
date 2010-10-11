#!/usr/local/bin/perl
use 5.010;
use Net::IP;

$numArgs = $#ARGV + 1;

if ($numArgs != 2){
	die "ERROR: WRONG COMMAND SYNTAX\nUSAGE: perl switchextract.pl BGPDUMPFILE.txt PREFIX.txt\n";
	}

print "EXTRACTING SWITCH PREFIXES\n";

my @switchzeilen;	#Liste für die Switchzeilen des Files!

open (bgpdump,"<$ARGV[0]") || die "ERROR: WRONG BGPDUMPFILENAME: $!";
open (myswitch,">$ARGV[1]") || die "ERROR: CANNOT WRITE ON DIRECTORY";
while(<bgpdump>){
	#Nach einem Muster für SWITCH suchen: AS 559 ist am Ende des ASPATH"
	#Und extrahiere das Prefix, speichern in myswitch!
	if(m[(?<prefix>[^\|]*)\|[^\|]*[ ]559\|]){
		push(@switchzeilen,$+{prefix});
		
	}	
}
close(bgpdump) ;

print "EXTRACTING DONE: ", scalar @switchzeilen, " PREFIXES EXTRACTED!\n";
#sortiere Liste und entferne doppelte einträge!
print "SORTING AND REMOVING DUPLICATE PREFIXES!\n";
my %entries=(); 		#Hash deklaration
foreach (@switchzeilen) {
	next if exists $entries{$_};
	$entries{$_}=1; 		#Hashkey!    my $prefix = shift;

}
my $number = 0;
foreach (@switchzeilen) { 		
	if($entries{$_}){ 		#Element wird gelöscht falls Hash = 0
		$number++;
		print myswitch $_;
		print myswitch "/559\n";#nur falls noch ein neues Element, füge es zur Liste!
		$entries{$_}=0; 	#Markiere Element als gesehen!
	}
}
print "SORTING DONE: ", $number, " PREFIXES FOR SWITCH!\n";

#Suche nach more specific prefixes!
print "LOOKING FOR MORE SPECIFIC PREFIXES!\n";

close(myswitch);
#print @switchzeilen;

