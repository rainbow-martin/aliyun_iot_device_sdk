/*
 * iotdeviceasyncmgr.h
 *
 * Author
 *     martin.wang     2018/8/6
 *
 */

#ifndef __IOT_MQTT_MGR_H__
#define __IOT_MQTT_MGR_H__

#include "MQTTAsync.h"
#include <string>

/////////////////////////////////////////////////////////////////
// IotMqttMgr
class IotDeviceAsyncMgr{
public:
	IotDeviceAsyncMgr(const std::string& strProductKey, const std::string& strDeviceName, const std::string& strDeviceSecret, bool isProVersion);
	~IotDeviceAsyncMgr();

public:
	void StartMessager();
	void LightControl(int nLightControl);

	std::string& GetSetTopic()		{ return m_strSetTopic; }
	std::string& GetPostTopic()		{ return m_strPostTopic; }
	MQTTAsync GetMqttClient()		{ return m_pClient; }

public:
	static void MQTTClient_connectionLost(void* context, char* cause);
	static int MQTTClient_messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message);
	static void MQTTClient_deliveryComplete(void* context, MQTTAsync_token token);
	static void MQTTClient_onConnected(void* context, MQTTAsync_successData* response);

protected:
	void initMqttInfo(const std::string& strProductKey, const std::string& strDeviceName, const std::string& strDeviceSecret);
	void BuildMqttClient();
	void connect2Service();
	void SendMqttMessage(int nLightSwitch, const char* test_topic);

	
protected:
	bool m_bUseTls;
	std::string m_strHost;
	std::string m_strClientId;
	std::string m_strUsername;
	std::string m_strPassword;

	std::string m_strDeviceName;
	bool m_bUsingProVersion;
	std::string m_strSetTopic;
	std::string m_strPostTopic;

	int m_nPort;
	
	MQTTAsync m_pClient;
};

#endif

