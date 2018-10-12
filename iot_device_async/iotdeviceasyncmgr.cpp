/*
 * iotdeviceasyncmgr.cpp
 *
 * Author
 *     martin.wang     2018/8/6
 *
 */

#include "iotdeviceasyncmgr.h"
#include <time.h>
#include <string.h>
#include <sstream>
#include <stdlib.h>

#ifdef ENABLE_RASPBERRY_FUNC
#include <bcm2835.h>
#endif

#include "json/json.h"
#include "baselib/logging.h"
#include "baselib/timehelper.h"
#include "baselib/cryptoutil.h"
#include "baselib/stringhelper.h"

#define IOT_MQTT_CLIENT_ID	"iot_client"
#define IOT_MQTT_HOST_URL	"iot-as-mqtt.cn-shanghai.aliyuncs.com"

#ifdef POSIX
#define _stricmp strcasecmp
#endif

/////////////////////////////////////////////////////////////////
// IotDeviceAsyncMgr
IotDeviceAsyncMgr::IotDeviceAsyncMgr(const std::string& strProductKey, const std::string& strDeviceName, 
	const std::string& strDeviceSecret, bool isProVersion)
{
	m_bUseTls = false;

	m_strDeviceName = strDeviceName;
	m_bUsingProVersion = isProVersion;
	initMqttInfo(strProductKey, strDeviceName, strDeviceSecret);
	m_strHost = strProductKey + "." + IOT_MQTT_HOST_URL;


	if( isProVersion ){
		std::string strTopicHeader = "/sys/" + strProductKey;
		strTopicHeader += "/";
		strTopicHeader += strDeviceName;

		m_strSetTopic = strTopicHeader + "/thing/service/property/set";
		m_strPostTopic = strTopicHeader + "/thing/event/property/post";
	}else{
		std::string strTopicHeader = "/" + strProductKey;
		strTopicHeader += "/";
		strTopicHeader += strDeviceName;

		m_strPostTopic = strTopicHeader + "/update";
		m_strSetTopic = strTopicHeader + "/get";
	}

#ifdef ENABLE_RASPBERRY_FUNC
	bcm2835_init();
#endif
}

IotDeviceAsyncMgr::~IotDeviceAsyncMgr()
{

}

void IotDeviceAsyncMgr::StartMessager()
{
	BuildMqttClient();
	connect2Service();
}

/*
	clientId    : client id, use a constant instead
	securemode  : 2 ssl connection; 3 tcp connection
	mqttClientId: clientId+"|securemode=3,signmethod=hmacsha1,timestamp=132323232|"
	mqttUsername: deviceName+"&"+productKey
	mqttPassword: sign_hmac(deviceSecret,content)
*/
void IotDeviceAsyncMgr::initMqttInfo(const std::string& strProductKey, const std::string& strDeviceName, const std::string& strDeviceSecret)
{
	// 1. calculate clientid
	std::string strTimeStamp = StringHelper::ValueOf(time(0));
	std::string strClientId = strProductKey + "." + strDeviceName;;

	m_strClientId = strClientId + "|securemode=3,signmethod=hmacsha1,timestamp=" + strTimeStamp + "|";
	m_strUsername = strDeviceName + "&" + strProductKey;

	std::string strContent;
	std::ostringstream oss;

	oss << "clientId" << strClientId << "deviceName" << strDeviceName
		<< "productKey" << strProductKey << "timestamp" << strTimeStamp;
	strContent = oss.str();

	// 2. calculate password
	unsigned char *sha1Buffer = NULL;
	unsigned int mac_length = 0;
	CryptoUtil::HmacEncode("sha1", strDeviceSecret.c_str(), strDeviceSecret.length(),
		strContent.c_str(), strContent.length(), sha1Buffer, mac_length);

	// 3. switch to hex string
	if( sha1Buffer ){
		m_strPassword = StringHelper::ToHexString(sha1Buffer, mac_length);
		free(sha1Buffer);
	}
}

void IotDeviceAsyncMgr::BuildMqttClient()
{
    std::string strHostUrl;
    if( m_bUseTls ) {
		strHostUrl = "ssl://" + m_strHost + ":1883";
    } else {
		strHostUrl = "tcp://" + m_strHost + ":1883";
    }

	int result = MQTTAsync_create(&m_pClient, strHostUrl.c_str(), m_strClientId.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
	LOG_F(LS_INFO) << "mqtt client create result: " << result;
}

void IotDeviceAsyncMgr::connect2Service()
{
	MQTTAsync_connectOptions opts = MQTTAsync_connectOptions_initializer;

	opts.keepAliveInterval = 30;
	opts.cleansession = 0;
	opts.username = m_strUsername.c_str();
	opts.password = m_strPassword.c_str();
	opts.MQTTVersion = MQTTVERSION_3_1_1;

	//opts.ssl = &sslopts;
	int result = MQTTAsync_setCallbacks(m_pClient, this, &IotDeviceAsyncMgr::MQTTClient_connectionLost,
		&IotDeviceAsyncMgr::MQTTClient_messageArrived, &IotDeviceAsyncMgr::MQTTClient_deliveryComplete);

	opts.onSuccess = &IotDeviceAsyncMgr::MQTTClient_onConnected;
	opts.onFailure = NULL;
	opts.context = this;

	MQTTAsync_connect(m_pClient, &opts);
}

void IotDeviceAsyncMgr::MQTTClient_onConnected(void* context, MQTTAsync_successData* response)
{
	MQTTAsync c = (MQTTAsync)context;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

	LOG_F(LS_INFO) << "MQTT Client is connected";
	opts.onSuccess = NULL;
	opts.context = c;

	IotDeviceAsyncMgr* pMgr = (IotDeviceAsyncMgr*)c;
	std::string& strSetTopic = pMgr->GetSetTopic();

	pMgr->LightControl(1);
	MQTTAsync_subscribe(pMgr->GetMqttClient(), strSetTopic.c_str(), 1, &opts);
}


void IotDeviceAsyncMgr::SendMqttMessage(int nLightSwitch, const char* test_topic)
{
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;

	char* topicName = NULL;
	int i = 0;
	int rc;

	Json::Value root;
	Json::Value params;
	Json::FastWriter writer;

	root["id"] = 1;
	root["version"] = "1.0";
	root["method"] = "thing.event.property.post";

	static int deviceIndex = 0;
	static int deviceIndexArray[] = {1, 2, 3, 4, 5, 6, -5};

	params["LightSwitch"] = nLightSwitch;
	params["deviceName"] = m_strDeviceName;
	params["deviceIndex"] = deviceIndexArray[deviceIndex++ % 7];
	params["deviceState"] = StringHelper::ValueOf(nLightSwitch);
	params["gmtCreate"] = StringHelper::TimeString();

	root["params"] = params;

	std::string strResult;
	if( m_bUsingProVersion ){
		strResult = writer.write(root);
	}else{
		strResult = writer.write(params);
	}	
	pubmsg.payload = (void*)strResult.c_str();
	pubmsg.payloadlen = strResult.size();
	pubmsg.qos = 1;
	pubmsg.retained = 0;

	rc = MQTTAsync_sendMessage(m_pClient, test_topic, &pubmsg, NULL);
	if (rc == MQTTASYNC_SUCCESS){
		LOG_F(LS_INFO) << "Topic: " << test_topic;
		LOG_F(LS_VERBOSE) << "Send message success, body:" << strResult;
	}
}

void IotDeviceAsyncMgr::MQTTClient_connectionLost(void* context, char* cause)
{
	LOG_F(LS_WARNING) << "MQTT Client connect lost";
}

int IotDeviceAsyncMgr::MQTTClient_messageArrived(void* context, char* topicName, int topicLen, MQTTAsync_message* message)
{
	LOG_F(LS_VERBOSE) << "Incoming message: " << topicName;

	IotDeviceAsyncMgr* pMgr = (IotDeviceAsyncMgr*)context;
	std::string& strSetTopic = pMgr->GetSetTopic();

	if( _stricmp(topicName, strSetTopic.c_str()) == 0 ){
		Json::Value root;
		Json::Reader reader;

		if (reader.parse((const char*)message->payload, (const char*)((char*)message->payload + message->payloadlen), root, false)){
			std::string strMethod = root["method"].asString();
			std::string strVersion = root["version"].asString();

			Json::Value params = root["params"];
			int lightSwitch = params["LightSwitch"].asInt();

			LOG_F(LS_VERBOSE) << "Method: " << strMethod << ", version: " << strVersion
				<< ", light switch: " << lightSwitch;
			pMgr->LightControl(1);
		}else{
			LOG_F(LS_WARNING) << "Not a valid json string";
		}
	}

	return 1;
}

void IotDeviceAsyncMgr::MQTTClient_deliveryComplete(void* context, MQTTAsync_token token)
{
	LOG_F(LS_INFO) << "MQTT Client delivery complete, dt:" << token;
}

void IotDeviceAsyncMgr::LightControl(int nLightSwitch)
{
	LOG_F(LS_VERBOSE) << "Start to enable light: " << nLightSwitch;
#ifdef ENABLE_RASPBERRY_FUNC
	int pin = RPI_V2_GPIO_P1_08;
	
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
	if( nLightSwitch ){
    	bcm2835_gpio_write(pin, HIGH);
	}else{
    	bcm2835_gpio_write(pin, LOW);
	}
#endif
	SendMqttMessage(nLightSwitch, m_strPostTopic.c_str());
}


