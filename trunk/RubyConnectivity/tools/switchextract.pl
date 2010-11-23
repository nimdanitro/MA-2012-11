#!/usr/local/bin/perl
#
#	Script for extracting Prefixes of an specific AS and their more specific prefixes
#	out of a bgpdumpfile file! Moreover, it extracts all validly announced prefixes into prefixes.txt
#	usage: perl switchextract.pl bgpdump.txt prefixes.txt
#	create bgpdump.txt with ./libbgpdump-1.4.99.12/bgpdump bview.20101006.0800.gz -m > bgpdump.txt
#	find the bviewXXXXXX.gz file from the rrc00.ripe.net at RIPE NCC, Amsterdam.(Collects default free routing updates!)
#	Author: Dani Aschwanden <asdaniel@ee.ethz.ch>
#
use 5.010;
use Net::IP;
my $start = time();
my $finish = 0;

$numArgs = $#ARGV + 1;

if ($numArgs != 2){
	die "ERROR: WRONG COMMAND SYNTAX\nUSAGE: perl switchextract.pl BGPDUMPFILE.txt PREFIX.txt\n";
	}

print "EXTRACTING SWITCH PREFIXES\n";

my @switchzeilen;		#Liste für die Switchzeilen des Files!
my $asnumber = "559";	#Hardcodes Target-AS für Switch!
my %ashash;				#Hash für die AS-Nummer mit den Prefixes zu verbinden!
my %pfseen;

open (bgpdump,"<$ARGV[0]") || die "ERROR: WRONG BGPDUMPFILENAME: $!";
while(<bgpdump>){
	#Nach einem Muster für SWITCH-Prefixe suchen: AS 559 ist am Ende des ASPATH"
	#Und extrahiere das Prefix, speichern in myswitch!
	@zeilen = split(/\|/,$_);
	$aspath = $zeilen[6];
	$prefix = $zeilen[5];
	@aspath = split(/ /,$aspath);
	if (pop(@aspath)==$asnumber){
		if ($pfseen{$prefix}==0){
			push(@switchzeilen, new Net::IP($prefix));
			$ashash{$prefix}=$asnumber;
			$pfseen{$prefix}++;
		}
	}
}
print "EXTRACTING DONE: " . scalar @switchzeilen . " PREFIXES EXTRACTED!\n";

#Suche nach more specific prefixes!
print "LOOKING FOR MORE SPECIFIC PREFIXES!\n";
close(bgpdump);
open (bgpdump,"<$ARGV[0]") || die "ERROR: WRONG BGPDUMPFILENAME: $!";
print "Please wait.. Get a coffee!\n";
my %seen = ();
my @prefixes_all;

while(<bgpdump>){
	# Extrahiere alle Prefixe und AS
	@zeilen = split(/\|/,$_);
	$aspath = $zeilen[6];
	@aspath = split(/ /,$aspath);
	$as = pop(@aspath);
	$prefix = $zeilen[5];
	#Wenn das Prefix bereits geprüft, gehe zu nächstem Prefix!
	next if $seen{$prefix}++;
	my $ip = new Net::IP($prefix);
	push(@prefixes_all, $ip);
	$ashash{$prefix} = ${as};
	
	#Ist $prefix ein MSP von einem SWITCH prefix?
	foreach $switchprefix (@switchzeilen){
		if ($switchprefix->overlaps($ip)==$IP_B_IN_A_OVERLAP){
			print "ONE MSP FOUND: " . $ip->prefix() ."\n";
			push(@switchzeilen, $ip);
		}
	}
}
close(bgpdump);
print "PREFIX EXTRACTION FINISHED! \n";

# Schreibe die gefundenen Prefixe für SWITCH im richtigen Format in das SWITCH-Prefix-Ausgabe-File!
open (myswitch,">$ARGV[1]") || die "ERROR: CANNOT WRITE ON DIRECTORY: $!";

foreach $ip (@switchzeilen){

	if($ip->version()==4){
		print myswitch $ip->prefix() . "/" . $ashash{$ip->prefix()} . "\n";
	}
	if($ip->version()==6){
		print myswitch $ip->short()."/" . $ip->prefixlen() . "/" . $ashash{$ip->short()."/". $ip->prefixlen()} . "\n";
	}
}
close(myswitch);

# Schreibe alle gültigen Prefixe in das prefix-all-File!
open (prefixes,">prefixes.txt") || die "ERROR: CANNOT WRITE ON DIRECTORY: $!";
foreach $ip (@prefixes_all){

	if($ip->version()==4){
		print prefixes $ip->prefix() . "/" . $ashash{$ip->prefix()} . "\n";
	}
	if($ip->version()==6){
		print prefixes $ip->short()."/" . $ip->prefixlen() . "/" . $ashash{$ip->short()."/". $ip->prefixlen()} . "\n";
	}
}
close(prefixes),

$finish = time();
$finish = $finish - $start;
print "TIME USED: " . $finish . " seconds \n";
