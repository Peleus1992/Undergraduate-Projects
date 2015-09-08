#include "etsbst.h"
#include "etsbst-packet.h"
#include <random.h>
#include <cmu-trace.h>
//#include <iostream>

// New packet type
int hdr_etsbst::offset_;
//packet binding; exclaim an area of hdr_etsbst size; packet.h only know the size, not the content.
static class etsbstHeaderClass : public PacketHeaderClass {
public:
	etsbstHeaderClass() : PacketHeaderClass("PacketHeader/etsbst",
					      sizeof(hdr_etsbst)) {
		bind_offset(&hdr_etsbst::offset_);
	}
} class_etsbsthdr;

// TCL Hooks
//
static class etsbstclass : public TclClass {
public:
// bind tcl and cpp; when set rp to etsbst in tcl file, system will call function create.
	etsbstclass() : TclClass("Agent/etsbst") {}
	TclObject* create(int argc, const char*const* argv) {
		assert(argc == 5);
		return (new etsbst((nsaddr_t) atoi(argv[4])));	// PBO agrv[4] is index_}
	}
} class_rtProtoetsbst;
//two timers
void etsbstTimer::expire(Event* e) {
	agent_->packSync();
}
void roundTimer::expire(Event* e) {
	FILE *fp = fopen("./Peleus_time_sync/etsbst/error.tr", "at");
	fprintf(fp, "%d\n", no++);
	printf("%d\n", no);
	fclose(fp);
	agent_->startSync();
	agent_->resetRTimer();
}
void resetTimer::expire(Event* e) {
	agent_->resetFlag();
}
// use tcl instruction to call cpp function here
int
etsbst::command(int argc, const char*const* argv) {
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

etsbst::etsbst(nsaddr_t id) : Agent(PT_MFLOOD), port_dmux_(0), etimer(this), rtimer(this), stimer(this) {

	index_ = id;
	logtarget = 0;
	myseq_ = 0;
	T=new double[5];
	lastPacket=NULL;
	isRecv=true;
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
etsbst::rt_resolve(Packet *p) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_etsbst *eh = HDR_ETSBST(p);
	if(index_==0)
		printf("%.9f, %d\n", p->txinfo_.RxPr, eh->ID);
//	printf("%.9f, %d, %d\n", NOW, index_, eh->ID);
	if(index_==0)
	{
		T[3]=localTime();
		if(eh->FatherID==index_)
		{
			double tmp = SRP(T[2], eh->RT, eh->ST, T[3], eh->ID);
//			printf("%.9f\n", NOW*eh->a+eh->b-localTime()-tmp);
			FILE *fp = fopen("./Peleus_time_sync/etsbst/error.tr", "at");
			fprintf(fp, "-i %d -o %d -L %d e %.9f\n", eh->ID, index_, eh->L, tmp-(NOW*eh->a+eh->b-localTime()));
			fclose(fp);
		}
		return;
	}
	if((lastPacket == NULL))
	{
		T[1]=localTime();
		stimer.resched(FLAG_DELAY);
		etimer.resched(Random::uniform(0.0, TIMER_VALUE));
		lastPacket = p;
	}
	else
	{
//		printf("%d into else\n", index_);
		struct hdr_etsbst *leh = HDR_ETSBST(lastPacket);
//		printf("%d into else2\n", index_);
		T[3]=localTime();
		if ((L==-1) && (eh->FatherID==leh->ID))
		{
			etimer.cancel();
			fatherID=eh->ID;
			double tmp = RRP(T[1], eh->RT, fatherID);

//			printf("%.9f\n", NOW*eh->a+eh->b-localTime()-tmp);
			FILE *fp = fopen("./Peleus_time_sync/etsbst/error.tr", "at");
			fprintf(fp, "-i %d -o %d -L %d e %.9f\n", index_, fatherID, L, NOW*eh->a+eh->b-localTime()-tmp);
			fclose(fp);
			isRecv=false;
			drop(p, DROP_IFQ_QFULL);
			drop(lastPacket, DROP_IFQ_QFULL);
		}
		else if ((L>=0) && (eh->FatherID==index_))
		{
//			printf("%d into else if\n", index_);
			double tmp = SRP(T[2], eh->RT, eh->ST, T[3], eh->ID);
//			printf("%.9f\n", NOW*eh->a+eh->b-localTime()-tmp);
			FILE *fp = fopen("./Peleus_time_sync/etsbst/error.tr", "at");
			fprintf(fp, "-i %d -o %d -L %d e %.9f\n", eh->ID, index_, eh->L, tmp-(NOW*eh->a+eh->b-localTime()));
			fclose(fp);
			drop(p, DROP_IFQ_QFULL);
		}
//		printf("%d out else\n", index_);
	}

/////////////////////////////////this is entry table section; not used in this algorithm
//	etsbst_RTEntry* rt;
//	rt = rtable_.rt_lookup(ih->saddr());
//	if(rt == NULL) {
//		rt = new etsbst_RTEntry(ih->saddr(), eh->seq_);
//		LIST_INSERT_HEAD(&rtable_.rthead,rt,rt_link);
//		forward(rt,p,FORWARD_DELAY);
//		rtable_.rt_print();
//	}
//	else if(rt->isNewSeq(eh->seq_) )
//	{
//		forward(rt, p, FORWARD_DELAY);
//		rt->addSeq(eh->seq_);
//		rtable_.rt_print();
//	}
//	else
//	{
//		drop(p, "LOWSEQ");
//	}
}


// Packet Reception Routines
void
etsbst::recv(Packet *p, Handler*) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);
	assert(initialized());

	if((ih->saddr() == index_) && (ch->num_forwards() == 0)) {	// Must be a packet I'm originating...		
		ch->size() += IP_HDR_LEN;		// Add the IP Header
		ih->ttl_ = NETWORK_DIAMETER;
		forward((etsbst_RTEntry*)1,p,0);
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
	if(!isRecv)
	{
		return;
	}
	rt_resolve(p);
}


// Packet Transmission Routines
void
etsbst::forward(etsbst_RTEntry* rt, Packet *p, double delay) {
	struct hdr_cmn *ch = HDR_CMN(p);
	struct hdr_ip *ih = HDR_IP(p);
	assert(ih->ttl_ > 0);
	assert(rt != 0);
//	assert(rt->rt_flags == RTF_UP);
	ch->next_hop_ = -1;	//Broadcast address
	ch->addr_type() = NS_AF_INET;
	ch->direction() = hdr_cmn::DOWN;       //important: change the packet's direction
	if(delay > 0.0) {
 		Scheduler::instance().schedule(target_, p, Random::uniform(delay*2));
	} else {		// Not a broadcast packet, no delay, send immediately
 		Scheduler::instance().schedule(target_, p, 0.);
	}
}

void
etsbst::resetRTimer()
{
	rtimer.resched(TURN_ROUND_TIME);
}

void
etsbst::startSync()
{
	Packet *p = Packet::alloc();
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_etsbst *eh = HDR_ETSBST(p);
	//IP layer
	ih->saddr() = index_;
	ih->daddr() = IP_BROADCAST;
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
	//network layer
	eh->seq_ = myseq_++;
	eh->ID = index_;
	eh->FatherID = -1;
	eh->L = 0;
	eh->RT = -1;
	eh->ST = T[2] = localTime();
//	printf("%d, %.9f\n", index_, NOW);
	this->recv(p, NULL);

}

void
etsbst::packSync()
{
	Packet *p = Packet::alloc();
	struct hdr_etsbst *leh = HDR_ETSBST(lastPacket);
	struct hdr_ip *ih = HDR_IP(p);
	struct hdr_etsbst *eh = HDR_ETSBST(p);

	//last packet
	L=leh->L+1;
	fatherID = leh->ID;
//	printf("%d's fatherID: %d\n", index_, fatherID);
	//IP layer
	ih->saddr() = index_;
	ih->daddr() = IP_BROADCAST;
	ih->sport() = RT_PORT;
	ih->dport() = RT_PORT;
	//network layer
	eh->seq_ = myseq_++;
	eh->ID = index_;
	eh->FatherID = fatherID;
	eh->L = L;
	eh->RT = T[1];
	eh->ST = T[2] = localTime();
	eh->a = a;
	eh->b = b;
//	printf("backbone %d\n", index_);
	this->recv(p, NULL);
}
void
etsbst::resetFlag()
{
	drop(lastPacket, DROP_IFQ_QFULL);
	lastPacket=NULL;
	isRecv=true;
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
etsbst::localTime()
{
	return (a*NOW+b);
}

double
etsbst::RRP(double t1, double t2, nsaddr_t i)
{
	double time_offset = t2-t1;
//	printf("%d->%d, %.9f\n", index_, i, time_offset);
	return time_offset;
}
double
etsbst::SRP(double t1, double t2, double t3, double t4, nsaddr_t i)
{
	double time_offset = (t2+t3-t1-t4)/2.0;
//	printf("%d->%d, L=%d, %.9f\n", index_, i, L, time_offset);
	return time_offset;
}


