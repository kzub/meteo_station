struct mhInfo {
 	int co2ppm;
	int temp;
	bool ok;
};

void mhSetup();
mhInfo ppmRead();
