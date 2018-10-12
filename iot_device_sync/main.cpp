/*
 * noname1.cpp
 *
 * Author
 *     martin.wang     2018/6/20
 *
 */

#include "iotdevicesyncmgr.h"
#include <string.h>
#include <stdlib.h>
#include "baselib/logging.h"

typedef struct{
	char* pszProductKey;
	char* pszDeviceName;
	char* pszDeviceSecret;
	bool isProVersion;
}IotDeviceIdentity;

int gDeviceSelect = 0;
IotDeviceIdentity gIdentities[] = {
	{ "a1onpCjBsoK", "bim_music_control_btn1", "jOx2X75wVuTzannfUOMzrLTiOBkhiZSr", true }
};

void InitLogSystem()
{
	LogMessage::ConfigureLog("timestamp verbose thread", "test.log");
	LOG_F(LS_INFO) << "===========>Start logging service<===========";
}

int main()
{
	InitLogSystem();

	IotDeviceSyncMgr mgr(gIdentities[gDeviceSelect].pszProductKey,
		gIdentities[gDeviceSelect].pszDeviceName,
		gIdentities[gDeviceSelect].pszDeviceSecret,
		gIdentities[gDeviceSelect].isProVersion);
	mgr.StartMessager();

	printf("Usage: \n");
	printf("1: Send IoT device message\n");
	printf("2: exit\n");
	while (1){
		int c = getchar();

		if (c == '1'){
			mgr.LightControl(1);
		}else if( c == '2' ){
			break;
		}
	}
	return 0;
}



