#!/usr/bin/perl
use strict;
use warnings;
use HiPi::BCM2835;
use HiPi::BCM2835::I2C;
use HiPi::Utils;

my $register = $ARGV[0];

if( $< ) { die 'script must be run using sudo'; }

my $username = getpwuid($<);
print qq(Start up user $username\n);

my $targetuser = getlogin() || 'pi';

HiPi::BCM2835->bcm2835_init();

HiPi::Utils::drop_permissions_name($targetuser, $targetuser);

$username = getpwuid($<);
print qq(Now running as user $username\n);

HiPi::BCM2835::I2C->set_baudrate(1, 100000);

my $dev = HiPi::BCM2835::I2C->new( address => 0x1d );

print qq(Quering register $register\n);

my ($reg_val) = $dev->i2c_read_register_rs($register, 1);

print qq(Reg Val is $reg_val\n);

