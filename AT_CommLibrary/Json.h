#ifndef __JSON_H
#define __JSON_H

/* JSON 数据包长度 */
#define JSON_BUF_SIZE		4096

typedef struct
{
	char buf[JSON_BUF_SIZE];
	uint32_t index;

}JsonStruct;

/* 使用此模块下的 JsonStructure 即可,完全够用,因为本结构下的发送状态机是顺序执行 */
extern JsonStruct  JsonStructure;


void Json_Create(JsonStruct *json);

void Json_End(JsonStruct *json);

void Json_AddString(JsonStruct *json, const char *name, const char *string);

void Json_AddNum(JsonStruct *json, const char *name, uint32_t num);


void Json_CreateArray(JsonStruct *json, const char *name);

void Json_EndArray(JsonStruct *json);


void Json_Start_ArrayElement(JsonStruct *json);

void Json_Stop_ArrayELement(JsonStruct *json);


/**
  * @brief  提取Json字段;
  * @param  buf, 待解析标识的起始地址;
  * @param  tag, 待解析的标识符起始地址;
  * @param  item, 解析完后存入的地方;
  * @param  length,限制长度;
  * @retval 解析成功返回写入的长度, 失败返回 0;
  */
uint32_t Extract_JsonData(char *buf, const char *tag, char *item, uint32_t length);




/* 使用示例 

	JsonStruct  JsonStructure;																				// 建立json结构体
	Json_Create(&JsonStructure);																			// {
	Json_AddNum(&JsonStructure, "msgType", 110);											// "msgType":110,
	Json_AddString(&JsonStructure, "devID", "HCXJ12345601");					// "devId":"HCXJ00446001",
	Json_AddString(&JsonStructure, "txnNo", "时间戳");								// "txnNo":"1567508825531",
	Json_AddString(&JsonStructure, "iccid", "iccid");									// "iccid":"898608421024C0580928",
	Json_AddNum(&JsonStructure, "devType", 5);												// "devType":5,
	Json_AddNum(&JsonStructure, "devAttr", 1);												// "devAttr":1,
	Json_AddNum(&JsonStructure, "devElecAttr", 2);										// "devElecAttr":2,
	Json_AddString(&JsonStructure, "softVersion", "软件版本号");			// "softVersion":"wtc",
	Json_AddString(&JsonStructure, "hardVersion", "硬件版本号");			// "hardVersion":"V1.0",
	Json_AddString(&JsonStructure, "protocolVersion", "协议版本号");	// "protocolVersion":"V1",
	
	Json_CreateArray(&JsonStructure, "devList");											// "devList":[
	for(uint8_t i; i<10; i++)
	{
		Json_Start_ArrayElement(&JsonStructure);												// {
		Json_AddString(&JsonStructure, "devID", "HCXJ123456 i");				// "devId":"HCXJ004460 i"
		Json_Stop_ArrayElement(&JsonStructure);													// }
	}
	Json_EndArray(&JsonStructure);																		// ]
	Json_End(&JsonStructure);																					// }
*/



//		"\n{\n"
//			"\"msgType\":110,\n"
//			"\"devId\":\"%s\",\n"
//			"\"txnNo\":\"%s\",\n"
//			"\"iccid\":\"%s\",\n"
//			"\"devType\":5,\n"
//			"\"devAttr\":1,\n"
//			"\"devElecAttr\":2,\n"
//			"\"softVersion\":\"%s\",\n"
//			"\"hardVersion\":\"v1.0\",\n"
//			"\"protocolVersion\":\"V1\",\n"
//			"\"devList\":\n"
//			"[\n\n"

//					"{ \"devId\":\"%s\" }\n\n"

//			"]\n"
//		"}\n",


#endif
