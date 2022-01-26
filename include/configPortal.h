struct tsdbConfig {
	String host;
	String port;
	String login;
	String password;
	String location;
};

void configPortalSetup();
void configPortalHandleClient();
tsdbConfig getCurrentTSDBconfig();