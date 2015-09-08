#Agent/UDP set packetSize_ 6000

# ======================================================================
# Define options
# ======================================================================
set val(ifqlen)         50                         ;# max packet in ifq
set val(nn)             240                          ;# number of mobilenodes
set val(n2)		-1			;#if n2=-1; shift to nn
set val(rp)             tsbst                     ;# routing protocol
set val(chan)       Channel/WirelessChannel
set val(prop)       Propagation/TwoRayGround
set val(netif)      Phy/WirelessPhy
set val(mac)        Mac/802_11
set val(ifq)        Queue/DropTail/PriQueue
set val(ll)         LL
set val(ant)        Antenna/OmniAntenna
set val(stop)	    100000
#set opt(energymodel)    EnergyModel     ;
#set opt(initialenergy)  100.0             ;# Initial energy in Joules
set opt(seed)         0.1
set addr ./Peleus_time_sync/tsbst
# ======================================================================
# Main Program
# ======================================================================
if { [string compare $val(n2) "-1"] == 0} { 
} else { 
	set val(nn) [expr $val(n2)*$val(n2)+$val(n2)+1]
}
#ns-random 0

# Initialize Global Variables
set ns_ [new Simulator]
set tracefd [open ./Peleus_time_sync/tsbst/tsbst.tr w]
$ns_ trace-all $tracefd

set namtrace    [open ./Peleus_time_sync/tsbst/tsbst.nam w]
$ns_ namtrace-all-wireless $namtrace 1000 1000
$ns_ use-newtrace
# set up topography
set topo [new Topography]
$topo load_flatgrid 1000 1000

# Create God
create-god $val(nn)

set rng [new RNG]
$rng seed opt(seed) 
# paratmers for random variables for delays
set RVdly [new RandomVariable/Uniform]
$RVdly set min_ 0
$RVdly set max_ 1000
$RVdly use-rng $rng
set RVdly2 [new RandomVariable/Uniform]
$RVdly2 set min_ 0
$RVdly2 set max_ 1000
$RVdly2 use-rng $rng
set a  [expr [$RVdly value]]
puts a:$a
set b [expr [$RVdly2 value]]
puts b:$b

# Create the specified number of mobilenodes [$val(nn)] and "attach" them
# to the channel. 
# configure node
$val(netif) set RXThresh_ 3.2277e-09
#8.91754e-10 200m
#5.34106e-08 60m dense 9
#2.66129e-08 85m dense 12--10
#1.42681e-08 100m dense 21--20
#5.8442e-09 125 dense 28--30
#4.29566e-09 135 dense 36--40
#3.2277e-09 145 dense 44--50
#2.17713e-09 160 dense 50--60

set channel [new Channel/WirelessChannel]
$channel set errorProbability_ 0.0

        $ns_ node-config -adhocRouting $val(rp) \
			 -llType $val(ll) \
			 -macType $val(mac) \
			 -ifqType $val(ifq) \
			 -ifqLen $val(ifqlen) \
			 -antType $val(ant) \
			 -propType $val(prop) \
			 -phyType $val(netif) \
			 -channel $channel \
			 -topoInstance $topo \
			 -agentTrace OFF \
			 -routerTrace OFF\
			 -macTrace OFF \
			 -movementTrace OFF			
			 
	for {set i 0} {$i < $val(nn) } {incr i} {
		set node_($i) [$ns_ node]	
		$node_($i) random-motion 0;	
	}
########################################################
#for(int i=0, j=0, flag=1; i<15; i++)
#{
#	for(j=0; j<=i; j++)
#	{
#		x+=flag;
#		y+=0;
#	}
#	for(j=0; j<=i; j++)
#	{
#		x+=0;
#		y+=flag;
#	}
#	flag=flag^1;
#}
#	set X 0.0
#	set Y 500.0
#	set flag 1
#	set incre 40
#	$node_(0) set X_ $X
#	$node_(0) set Y_ $Y
#	$node_(0) set Z_ 0.0
#        for {set i 0} {$i < $val(n2) } {incr i} {
#        	for {set j 0} {$j <= $i } {incr j} {
#			set X [expr $X + $flag * $incre]
#			$node_([expr $i*($i+1)+$j+1]) set X_ $X
#			$node_([expr $i*($i+1)+$j+1]) set Y_ $Y
#			$node_([expr $i*($i+1)+$j+1]) set Z_ 0.0		
#        	}
#		for {set j 0} {$j <= $i } {incr j} {
#			set Y [expr $Y + $flag * $incre]
#			$node_([expr ($i+1)*($i+1)+$j+1]) set X_ $X
#			$node_([expr ($i+1)*($i+1)+$j+1]) set Y_ $Y
#			$node_([expr ($i+1)*($i+1)+$j+1]) set Z_ 0.0		
#		}
#        	if { [string compare $flag "1"] == 0} { 
#			set flag -1
#        	} else { 
#			set flag 1
#		}
#        }
#########################################################
# Provide initial (X,Y, for now Z=0) co-ordinates for mobilenodes
#	for {set i 0} {$i < $val(nn) } {incr i} {
#		$node_($i) set X_ [expr [$RVdly value]]
#		$node_($i) set Y_ [expr [$RVdly2 value]]
#		$node_($i) set Z_ 0.0	
#	}	

# Define node initial position in nam
for {set i 0} {$i < $val(nn)} {incr i} {

    # 20 defines the node size in nam, must adjust it according to your scenario
    # The function must be called after mobility model is defined

    $ns_ initial_node_pos $node_($i) 10
}

set udp_(0) [new Agent/UDP]
$ns_ attach-agent $node_(0) $udp_(0) 

# Tell nodes when the simulation ends
for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at $val(stop).0 "$node_($i) reset";
}
$ns_ at $val(stop).0 "stop"
$ns_ at $val(stop).01 "puts \"NS EXITING...\" ; $ns_ halt"
proc stop {} {
    global ns_ tracefd
    global namtrace
    global addr
    $ns_ flush-trace
    close $tracefd
    close $namtrace
    puts "MISSION COMPLISHED!"
#exec gawk -f $addr/broadcast.awk $addr/tsbst.tr >> $addr/broadcast.out
#exec gawk -f $addr/energy.awk $addr/tsbst.tr >> $addr/energy.out
		exec gawk -f $addr/error.awk $addr/error.tr > $addr/error.out
		exec gawk -f $addr/errorl.awk $addr/error.tr > $addr/errorl.out			
#	exec nam ./Peleus_time_sync/tsbst/tsbst.nam
	exit 0
} 
exec rm $addr/error.tr 
puts "Starting SimulatTIMER_IDLEion..."
$ns_ run






