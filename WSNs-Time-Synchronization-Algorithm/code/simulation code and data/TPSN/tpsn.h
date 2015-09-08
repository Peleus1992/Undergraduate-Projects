// PBO ================================================================
// I have added some lines into ~ns2/tcl/lib/ns-lib.tcl
// I copy the file ns-lib.tcl to this directory for reference
// PBO ================================================================

#ifndef _tpsn_h__
#define _tpsn_h__

#include <sys/types.h>
#include <cmu-trace.h>
#include <priqueue.h>
#include "tpsn-seqtable.h"
#include <list>
#define u_int_32 int
#define NOW (Scheduler::instance().clock())
// Should be set by the user using best guess (conservative)
#define NETWORK_DIAMETER 30	// 30 hops
 
// The followings are used for the forward() function. Controls pacing.
#define FORWARD_DELAY 0.01		// random delay
#define TIMER_VALUE 0.5  //2.0	// random time
#define NO_DELAY -1.0			// no delay
#define BEGIN_TIME 0.5			// the time of sending first Sync by node 0
#define SYNC_TIME 20.0
#define FLAG_TIME 50.0
#define TURN_ROUND_TIME 100.0	// time taken to execute a round of synchronization
#define TIME_DRIFT 0.01			//used for the definition of value a
#define TIME_OFFSET 0.1			// used to determine the value of b
class tpsn;
//timer for decide backbone node
class tpsnTimer : public TimerHandler {
	public:
		tpsnTimer(tpsn* agent) : TimerHandler() {
			agent_ = agent;
		}
	protected:
		tpsn* agent_;
		virtual void expire(Event* e);
};

//timer for start synchronization
class tpsnRoundTimer : public TimerHandler {
	public:
		tpsnRoundTimer(tpsn* agent) : TimerHandler() {
			no=0;
			agent_ = agent;
		}
	protected:
		int no;
		tpsn* agent_;
		virtual void expire(Event* e);
};
//for the value 'isRecv' to be set to true;
class tpsnResetTimer : public TimerHandler {
public:
	tpsnResetTimer(tpsn* agent) : TimerHandler() {
		agent_ = agent;
	}
protected:
	tpsn* agent_;
	virtual void expire(Event* e);
};
//for the value 'isRecv' to be set to true;
class tpsnFlagTimer : public TimerHandler {
public:
	tpsnFlagTimer(tpsn* agent) : TimerHandler() {
		agent_ = agent;
	}
protected:
	tpsn* agent_;
	virtual void expire(Event* e);
};
// The Routing Agent
class tpsn: public Agent {
	friend class tpsn_RTEntry;
	friend class tpsnTimer;
	friend class tpsnRoundTimer;
	friend class tpsnResetTimer;
	friend class tpsnFlagTimer;
public:
	tpsn(nsaddr_t id);
	void recv(Packet *p, Handler *);

protected:
	int command(int, const char *const *);
	inline int initialized() { return 1 && target_; }

	// Route Table Management
	void rt_resolve(Packet *p);

	// Packet TX Routines
	void forward(tpsn_RTEntry *rt, Packet *p, double delay);

	nsaddr_t index_;                // IP Address of this node
	nsaddr_t fatherID;
	int L;							// level of the node(only backbone node possess it)
	double *T;						// array storing timestamps of receiving and sending message
	// Routing Table
	tpsn_RTable rtable_;

	//Timer
	tpsnTimer etimer;				// timer for decide backbone node
	tpsnRoundTimer rtimer;				// timer for start synchronization
	tpsnResetTimer stimer;				// for the value 'isRecv' to be set to true;
	tpsnFlagTimer ftimer;
	void resetRTimer();				// to reschedule a new round of synchronization

	void startSync();				// Start Sync message

	void packSync();				// Construct Sync message

	void replySync(double t2, double t3, nsaddr_t i);

	void resetFlag();				// reset isRecv
	void changeFlag();
	double localTime();				// Return local time

									//RRP (receiver-to-receiver protocol)
	double RRP(double t1, double t2, nsaddr_t i);
									//SRP (sender-to-receiver protocol)
	double SRP(double t1, double t2, double t3, double t4, nsaddr_t i);

	// A mechanism for logging the contents of the routing
	Trace *logtarget;
	NsObject *uptarget_;
	NsObject *port_dmux_;
  private:
  	u_int32_t myseq_;
  	double a;						// parameters used to simulate the clock error in real sensor node.
  	double b;						// local time == real time * a + b;
};

#endif 

