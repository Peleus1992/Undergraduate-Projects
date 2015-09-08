#include "tsbst.h"
#include "tsbst-packet.h"
#include <random.h>
#include <cmu-trace.h>
//#include <iostream>

// New packet type
int hdr_tsbst::offset_;
//packet binding; exclaim an area of hdr_tsbst size; packet.h only know the size, not the content.
static class tsbstHeaderClass : public PacketHeaderClass {
public:
	tsbstHeaderClass() : PacketHeaderClass("PacketHeader/tsbst",
					      sizeof(hdr_tsbst)) {
		bind_offset(&hdr_tsbst::offset_);
	}
} class_tsbsthdr;

// TCL Hooks
//
static class tsbstclass : public TclClass {
public:
// bind tcl and cpp; when set rp to tsbst in tcl file, system will call function create.
	tsbstclass() : TclClass("Agent/tsbst") {}
	TclObject* create(int argc, const char*const* argv) {
		assert(argc == 5);
		return (new tsbst((nsaddr_t) atoi(argv[4])));	// PBO agrv[4] is index_}
	}
} class_rtPrototsbst;
//two timers
void tsbstTimer::expire(Event* e) {
	agent_->packSync();
}
void tsbstRoundTimer::expire(Event* e) {
	FILE *fp = fopen("./Peleus_time_sync/tsbst/error.tr", "at");
	fprintf(fp, "%d\n", no++);
	printf("%d\n", no);
	fclose(fp);
	agent_->startSync();
	agent_->resetRTimer();
}
void recurTimer::expire(Event* e) {
	agent_->resetFlag();
}
void flagTimer::expire(Event* e) {
	agent_->changeFlag();
}

// use tcl instruction to call cpp function here
int
tsbst::command(int argc, const char*const* argv) {
	Tcl& tcl = Tcl::instance();
	if(argc == 2) {		
		if(strncasecmp(argv[1], "id", 2) == 0) {
			tcl.resultf("%d", index_);
			return TCL_OK;
		}
		else if (strcmp(argv[1], "uptarget") == 0) {
			if (uptarget_ != 0)
				tcl.result(uptarget_->name());
			return (TCL_OK);
		}
	} else if(argc == 3) {
		if(strcmp(argv[1], "index_") == 0) {
			index_ = atoi(argv[2]);
			return TCL_OK;
		} else if(strcmp(argv[1], "log-target") == 0 || strcmp(argv[1], "tracetarget") == 0) {
			logtarget = (Trace*) TclObject::lookup(argv[2]);
			if(logtarget == 0) return TCL_ERROR;
			return TCL_OK;
		}
		else if (strcmp(argv[1], "uptarget") == 0) {
			if (*argv[2] == '0') {
				target_ = 0;
				return (TCL_OK);
			}
			uptarget_ = (NsObject*)TclObject::lookup(argv[2]);
			if (uptarget_ == 0) {
				tcl.resultf("no such object %s", argv[2]);
				return (TCL_ERROR);
			}
			return (TCL_OK);
		}
		else if (strcasecmp (argv[1], "port-dmux") == 0) { 
			TclObject *obj;
			port_dmux_ = (NsObject *) obj;
			return (TCL_OK);
		}
	}
	return Agent::command(argc, argv);
}

tsbst::tsbst(nsaddr_t id) : Agent(PT_MFLOOD), port_dmux_(0), etimer(this), rtimer(this), stimer(this), ftimer(this) {

	index_ = id;
	logtarget = 0;
	myseq_ = 0;
	T=new double[5];
	lastPacket=NULL;
	if(TIME_DRIFT)
		a = 1.0 + Random::uniform(-TIME_DRIFT, TIME_DRIFT);
	else
		a = 1.0;
	if(TIME_OFFSET)
		b = Random::uniform(-TIME_OFFSET, TIME_OFFSET);
	else
		b = 0.0;
	if(index_ == 0)
	{
		rtimer.resched(BEGIN_TIME);
		L=0;
	}
	else
	{
		L=-1;
	}
}


// Route Handling Functions
void
tsbst::rt_resolve(Packet *p) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_tsbst *eh = HDR_TSBST(p);
//	printf("%.9f, %d, %d\n", NOW, index_, eh->ID);
	if(L==-1)
	{
		T[2]=localTime();
		L=eh->L+1;
		fatherID = eh->ID;
		ftimer.resched(FLAG_DELAY);
		etimer.resched(Random::uniform(0.0, TIMER_VALUE));
		lastPacket = p;
	}
	else if(ih->daddr()==index_)
	{
		T[4]=localTime();
		double tmp = SRP(T[1], eh->RT, eh->ST, T[4], eh->ID);
//		printf("%.9f, %d->%d\n", NOW*eh->a+eh->b-localTime()-tmp, index_, eh->ID);
		packNotify(tmp, eh->RT);
		return;
	}
	else if(eh->ID==fatherID)
	{
		if(etimer.status()!=TIMER_IDLE)
			etimer.cancel();
//		printf("%.9f, %d->%d, L:%d\n", NOW*eh->a+eh->b-localTime()+eh->ET+T[2]-eh->CT, index_, eh->ID, L);
		FILE *fp = fopen("./Peleus_time_sync/tsbst/error.tr", "at");
		fprintf(fp, "-i %d -o %d -L %d e %.9f\n", index_, fatherID, L, NOW*eh->a+eh->b-localTime()+eh->ET+T[2]-eh->CT);
		fclose(fp);
		stimer.resched(Random::uniform(0.0, TIMER_VALUE));
	}
}


// Packet Reception Routines
void
tsbst::recv(Packet *p, Handler*) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);
	assert(initialized());

	if((ih->saddr() == index_) && (ch->num_forwards() == 0)) {	// Must be a packet I'm originating...		
		ch->size() += IP_HDR_LEN;		// Add the IP Header
		ih->ttl_ = NETWORK_DIAMETER;
		forward((tsbst_RTEntry*)1,p,0);
		return;
	} else if(ih->saddr() == index_) {	// I received a packet that I sent.  Probably a routing loop.
		drop(p, DROP_RTR_ROUTE_LOOP);
		return;
	} else {		// Packet I'm forwarding...
		if(--ih->ttl_ == 0) {	// Check the TTL.  If it is zero, then discard.
			drop(p, DROP_RTR_TTL);
	 		return;
		}
	}
	rt_resolve(p);
}


// Packet Transmission Routines
void
tsbst::forward(tsbst_RTEntry* rt, Packet *p, double delay) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);
	assert(ih->ttl_ > 0);
	assert(rt != 0);
//	assert(rt->rt_flags == RTF_UP);
//	ch->next_hop_ = -1;	//Broadcast address
	ch->addr_type() = NS_AF_INET;
	ch->direction() = hdr_cmn::DOWN;       //important: change the packet's direction
	if(delay > 0.0) {
 		Scheduler::instance().schedule(target_, p, Random::uniform(delay*2));
	} else {		// Not a broadcast packet, no delay, send immediately
 		Scheduler::instance().schedule(target_, p, 0.);
	}
}

void
tsbst::resetRTimer()
{
	rtimer.resched(TURN_ROUND_TIME);
}
void
tsbst::startSync()
{
	Packet *p = Packet::alloc();
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_tsbst *eh = HDR_TSBST(p);
	struct hdr_cmn *ch = HDR_CMN(p);
	//IP layer
	ih->saddr() = index_;
	ih->daddr() = IP_BROADCAST;
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
	//network layer
	eh->seq_ = myseq_++;
	eh->ID = index_;
	eh->L = L;
	eh->RT = -1;
	eh->ST = -1;
	eh->ET = -999.9;
	//MAC layer
	ch->next_hop_ = -1; // Broadcast message
//	printf("%d, %.9f\n", index_, NOW);
	T[1] = localTime();
	this->recv(p, NULL);
}

void
tsbst::packSync()
{
	Packet *p = Packet::alloc();
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_tsbst *eh = HDR_TSBST(p);
	struct hdr_cmn *ch = HDR_CMN(p);
	//	printf("%d's fatherID: %d\n", index_, fatherID);
	//IP layer
	ih->saddr() = index_;
	ih->daddr() = fatherID;
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
	//network layer
	eh->seq_ = myseq_++;
	eh->ID = index_;
	eh->L = L;
	eh->RT = T[2];
	eh->ST = T[3] = localTime();
	eh->ET = -999.9;
	eh->a = a;
	eh->b = b;
	//MAC layer
	ch->next_hop_ = fatherID;
//	printf("backbone %d\n", index_);
	this->recv(p, NULL);
}
void
tsbst::packNotify(double t, double c)
{
	Packet *p = Packet::alloc();
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_tsbst *eh = HDR_TSBST(p);
	struct hdr_cmn *ch = HDR_CMN(p);
	//IP layer
	ih->saddr() = index_;
	ih->daddr() = IP_BROADCAST;
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
	//network layer
	eh->seq_ = myseq_++;
	eh->ID = index_;
	eh->L = L;
	eh->RT = -1;
	eh->ST = -1;
	eh->ET = t;
	eh->CT = c;
	eh->a = a;
	eh->b = b;
	//MAC layer
	ch->next_hop_ = -1; // Broadcast message
//	printf("%d, %.9f\n", index_, NOW);
	this->recv(p, NULL);
}
void
tsbst::resetFlag()
{
	startSync();
}
void
tsbst::changeFlag()
{
	lastPacket=NULL;
	if(index_ == 0)
	{
		L=0;
	}
	else
	{
		L=-1;
	}
}
double
tsbst::localTime()
{
	return (a*NOW+b);
}

double
tsbst::RRP(double t1, double t2, nsaddr_t i)
{
	double time_offset = t2-t1;
//	printf("%d->%d, %.9f\n", index_, i, time_offset);
	return time_offset;
}
double
tsbst::SRP(double t1, double t2, double t3, double t4, nsaddr_t i)
{
	double time_offset = (t2+t3-t1-t4)/2.0;
//	printf("%d->%d, L=%d, %.9f\n", index_, i, L, time_offset);
	return time_offset;
}


