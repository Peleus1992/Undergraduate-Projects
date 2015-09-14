package myhadoop.com.util;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Random;

public class RandomName {
	private static StringBuffer sb;
	public static StringBuffer getRandomName(String ip) {
		sb = new StringBuffer("");
		//The following three methods should be at order;
		addZero(ip);
		addTimestamp();
		addRandom();
		return sb;
	}
	private static void addZero(String ip) {
		String args[] = ip.split("\\.");
		for(int i = 0; i < 4; i++) {
			int c = 3 - args[i].length();
			
			for(int j = 0; j < c; j++) {
				sb.append("0");
			}
			sb.append(args[i]);
		}
	}
	private static void addTimestamp() {
		SimpleDateFormat sdf = new SimpleDateFormat("yyyyMMddHHmmssSSS");
		sb.append(sdf.format(new Date()));
	}
	private static void addRandom() {
		Random ran = new Random();
		for(int i = 0; i < 3; i++) {
			sb.append(ran.nextInt(10));
		}
	}
}
