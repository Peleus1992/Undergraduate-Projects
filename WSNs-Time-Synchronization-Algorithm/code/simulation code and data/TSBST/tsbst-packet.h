#ifndef _tsbst_packet_h__
#define _tsbst_packet_h__

/*
 * JFlood Routing Protocol Header Macros
 */
#define HDR_TSBST(p)		((struct hdr_tsbst*)hdr_tsbst::access(p))

/*
 * General JFlood Header
 */
struct hdr_tsbst {
	u_int32_t	seq_;
	u_int32_t	dst_group;		// destination's group id
	u_int32_t   ID;
	u_int16_t   L;
	double 		RT;
	double		ST;
	double 		CT;				// coordinate time
	double 		ET;				// estimated time offset
	// time_offset and time drift parameters
	double 		a;
	double 		b;
	// Header access methods
	static int offset_; // required by PacketHeaderManager
	inline static int& offset() { return offset_; }
	inline static hdr_tsbst* access(const Packet* p) {
		return (hdr_tsbst*) p->access(offset_);
	}
};

#endif /* __tsbst_packet_h__ */
