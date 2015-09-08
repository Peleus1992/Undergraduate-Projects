#ifndef _tpsn_packet_h__
#define _tpsn_packet_h__

/*
 * JFlood Routing Protocol Header Macros
 */
#define HDR_TPSN(p)		((struct hdr_tpsn*)hdr_tpsn::access(p))

/*
 * General JFlood Header
 */
struct hdr_tpsn {
	u_int32_t	seq_;
	u_int32_t	dst_group;		// destination's group id
	u_int32_t   ID;
	u_int16_t   L;
	double 		RT;
	double		ST;
	// time_offset and time drift parameters
	double 		a;
	double 		b;
	// Header access methods
	static int offset_; // required by PacketHeaderManager
	inline static int& offset() { return offset_; }
	inline static hdr_tpsn* access(const Packet* p) {
		return (hdr_tpsn*) p->access(offset_);
	}
};

#endif /* __tpsn_packet_h__ */
