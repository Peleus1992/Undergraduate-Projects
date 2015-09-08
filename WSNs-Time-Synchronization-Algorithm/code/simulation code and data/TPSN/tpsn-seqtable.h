#ifndef _tpsn_seqtable_h__
#define _tpsn_seqtable_h__


#include <assert.h>
#include <sys/types.h>
#include <config.h>
#include <lib/bsd-list.h>
#include <scheduler.h>

#define INFINITY 0xff
#define RTF_DOWN 0
#define RTF_UP 1

#define REM_SEQ_COUNT 5000

// Route Table Entry
class tpsn_RTEntry {
	friend class tpsn_RTable;
	friend class tpsn;

public:
	tpsn_RTEntry();
	tpsn_RTEntry(nsaddr_t src,u_int32_t seq);
	bool 	isNewSeq(u_int32_t seq);	// old -> false, new->true
	void		addSeq(u_int32_t seq);	// add a seqno to seqno array(rt_seqnos)
	
protected:
	LIST_ENTRY(tpsn_RTEntry) rt_link;

	nsaddr_t src_;
//	u_int32_t seq_;

	u_int32_t		rt_seqnos[REM_SEQ_COUNT];	//seqno array
	u_int32_t       	max_seqno;	//max seqno 
	u_int32_t       	min_seqno;	//max seqno 
	u_int16_t		seq_it;	// seqno's iterator
};


// The Routing Table
class tpsn_RTable {
	friend class tpsn;
public:
	tpsn_RTable() { LIST_INIT(&rthead); }
	void rt_delete(nsaddr_t id);
	tpsn_RTEntry* rt_lookup(nsaddr_t id);

	void rt_print();
private:
	LIST_HEAD(, tpsn_RTEntry) rthead;
};

#endif
