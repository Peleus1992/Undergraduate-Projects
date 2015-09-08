#include "tpsn.h"
#include "tpsn-packet.h"
#include <random.h>
#include <cmu-trace.h>
//#include <iostream>

// New packet type
int hdr_tpsn::offset_;
//packet binding; exclaim an area of hdr_tpsn size; packet.h only know the size, not the content.
static class tpsnHeaderClass : public PacketHeaderClass {
public:
	tpsnHeaderClass() : PacketHeaderClass("PacketHeader/tpsn",
					      sizeof(hdr_tpsn)) {
		bind_offset(&hdr_tpsn::offset_);
	}
} class_tpsnhdr;

// TCL Hooks
//
static class tpsnclass : public TclClass {
public:
// bind tcl and cpp; when set rp to tpsn in tcl file, system will call function create.
	tpsnclass() : TclClass("Agent/tpsn") {}
	TclObject* create(int argc, const char*const* argv) {
		assert(argc == 5);
		return (new tpsn((nsaddr_t) atoi(argv[4])));	// PBO agrv[4] is index_}
	}
} class_rtPrototpsn;
//two timers
void tpsnTimer::expire(Event* e) {
	agent_->packSync();
}
void tpsnRoundTimer::expire(Event* e) {
	FILE *fp = fopen("./Peleus_time_sync/tpsn/error.tr", "at");
	fprintf(fp, "%d\n", no++);
	printf("%d\n", no);
	fclose(fp);
	agent_->resetRTimer();
	agent_->startSync();
}
void tpsnResetTimer::expire(Event* e) {
	agent_->resetFlag();
}
void tpsnFlagTimer::expire(Event* e) {
	agent_->changeFlag();
}
// use tcl instruction to call cpp function here
int
tpsn::command(int argc, const char*const* argv) {
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

tpsn::tpsn(nsaddr_t id) : Agent(PT_MFLOOD), port_dmux_(0), etimer(this), rtimer(this), stimer(this), ftimer(this) {

	index_ = id;
	logtarget = 0;
	myseq_ = 0;
	T=new double[5];
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
tpsn::rt_resolve(Packet *p) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_tpsn *eh = HDR_TPSN(p);
//	printf("%.9f, %d, %d\n", NOW, index_, eh->ID);
	if(L==-1)
	{
		L=eh->L+1;
		fatherID=eh->ID;
		stimer.resched(Random::uniform(0.0, TIMER_VALUE));
		etimer.resched(Random::uniform(TIMER_VALUE, SYNC_TIME));
		ftimer.resched(FLAG_TIME);
	}
	else if((ih->daddr()==index_)&&(eh->ID!=fatherID))
	{
		T[3]=T[2]=localTime();
		replySync(T[2], T[3], eh->ID);
	}
	else if((ih->daddr()==index_)&&(eh->ID==fatherID))
	{
		T[4] = localTime();
		double tmp = SRP(T[1], eh->RT, eh->ST, T[4], eh->ID);
		FILE *fp = fopen("./Peleus_time_sync/tpsn/error.tr", "at");
		fprintf(fp, "-i %d -o %d -L %d e %.9f\n", index_, eh->ID, L, NOW*eh->a+eh->b-localTime()-tmp);
		fclose(fp);
	}

}


// Packet Reception Routines
void
tpsn::recv(Packet *p, Handler*) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);
	assert(initialized());

	if((ih->saddr() == index_) && (ch->num_forwards() == 0)) {	// Must be a packet I'm originating...		
		ch->size() += IP_HDR_LEN;		// Add the IP Header
		ih->ttl_ = NETWORK_DIAMETER;
		forward((tpsn_RTEntry*)1,p,0);
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
tpsn::forward(tpsn_RTEntry* rt, Packet *p, double delay) {
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
tpsn::resetRTimer()
{
	rtimer.resched(TURN_ROUND_TIME);
}
void
tpsn::changeFlag()
{
	if(index_ == 0)
	{
		L=0;
	}
	else
	{
		L=-1;
	}
}

void
tpsn::startSync()
{
	Packet *p = Packet::alloc();
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_tpsn *eh = HDR_TPSN(p);
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
	//MAC layer
	ch->next_hop_ = -1; //broadcast
//	printf("%d, %.9f\n", index_, NOW);
	this->recv(p, NULL);

}

void
tpsn::packSync()
{
	Packet *p = Packet::alloc();
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_tpsn *eh = HDR_TPSN(p);
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
	eh->a = a;
	eh->b = b;
	//MAC layer
	ch->next_hop_ = fatherID;
//	printf("backbone %d\n", index_);
	T[1] = localTime();
	this->recv(p, NULL);
}
void
tpsn::replySync(double t2, double t3, nsaddr_t i)
{
	Packet *p = Packet::alloc();
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_tpsn *eh = HDR_TPSN(p);
	struct hdr_cmn *ch = HDR_CMN(p);
//	printf("%d's fatherID: %d\n", index_, fatherID);
	//IP layer
	ih->saddr() = index_;
	ih->daddr() = i;
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
	//network layer
	eh->seq_ = myseq_++;
	eh->ID = index_;
	eh->a = a;
	eh->b = b;
	eh->RT = t2;
	eh->ST = t3;
	//MAC layer
	ch->next_hop_ = i;
//	printf("backbone %d\n", index_);
	this->recv(p, NULL);
}
void
tpsn::resetFlag()
{
	startSync();
}
double
tpsn::localTime()
{
	return (a*NOW+b);
}

double
tpsn::RRP(double t1, double t2, nsaddr_t i)
{
	double time_offset = t2-t1;
//	printf("%d %d %.9f\n", index_, i, time_offset);
	return time_offset;
}
double
tpsn::SRP(double t1, double t2, double t3, double t4, nsaddr_t i)
{
	double time_offset = (t2+t3-t1-t4)/2.0;
	return time_offset;
}


