#ifndef _etsbst_packet_h__
#define _etsbst_packet_h__

/*
 * JFlood Routing Protocol Header Macros
 */
#define HDR_ETSBST(p)		((struct hdr_etsbst*)hdr_etsbst::access(p))

/*
 * General JFlood Header
 */
struct hdr_etsbst {
	u_int32_t	seq_;
	u_int32_t	dst_group;		// destination's group id
	u_int32_t   ID;
	u_int32_t   FatherID;
	u_int16_t   L;
	double 		RT;
	double		ST;
	// time_offset and time drift parameters
	double 		a;
	double 		b;
	// Header access methods
	static int offset_; // required by PacketHeaderManager
	inline static int& offset() { return offset_; }
	inline static hdr_etsbst* access(const Packet* p) {
		return (hdr_etsbst*) p->access(offset_);
	}
};

#endif /* __etsbst_packet_h__ */
