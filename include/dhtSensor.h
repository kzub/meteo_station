struct dhtInfo {
	float h;
 	float t;
 	float hi;
 	bool ok;
};

void dhtSetup();
dhtInfo dhtRead();
