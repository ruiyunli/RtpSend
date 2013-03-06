
/*
 *****************************************
 * RTP�������̣�                                                                    *
 * 1��������Ϣ							 *
 * 	     �������ض˿ڣ�Ŀ��˿ڣ�Ŀ��ip��             *
 * 	     �Լ����͵����ݰ�����				 *
 * 2��ip���ݽṹת��						 *
 * 3��ʱ�������							 *
 * 4�������Ự							 *
 * 5�����ͼ��������ݰ�					 *
 *****************************************
 *
 *******************************************
 * Send:  	192.168.0.102--->192.168.0.101 *
 * 			7000--->8000				   *
 * Recv:	192.168.0.101--->192.168.0.102 *
 * 			8000--->7000				   *
 *******************************************
 *
 */

#include <jni.h>
#include "rtpsession.h"
#include "rtpudpv4transmitter.h"
#include "rtpipv4address.h"
#include "rtpsessionparams.h"
#include "rtperrors.h"
#include <netinet/in.h>	//not win32
#include <arpa/inet.h>	//not win32
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>

//
#ifdef __cplusplus
extern "C" {
void Java_com_example_rtpsend_MainActivity_RtpTest(JNIEnv *, jobject);
}
#endif

/**** ����̨��� ****/
#include <android/log.h>
#define  LOG_TAG "RTP_JNI"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG  , LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO   , LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN   , LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR  , LOG_TAG, __VA_ARGS__)


//������Ϣ
#define LOCAL_PORT 7000
#define DEST_PORT 8000
#define DEST_IP "192.168.0.101"
#define PACKET_NUM 10
#define TIME_STAMP 1.0/10.0


void Java_com_example_rtpsend_MainActivity_RtpTest(JNIEnv* env, jobject thiz)
{
 	RTPSession sess;
	uint16_t portbase,destport;
	uint32_t destip;
	std::string ipstr;
	int status,i,num;

    //1��������Ϣ
	//�������ض˿ڣ�Ŀ��˿ڣ�Ŀ��ip�����͵����ݰ�����
	portbase=LOCAL_PORT;
    ipstr=DEST_IP;
    destport=DEST_PORT;
	num=PACKET_NUM;

	//2��ip���ݽṹת��
	destip = inet_addr(ipstr.c_str());
	if (destip == INADDR_NONE)
	{
		LOGE("BAD IP ADDR");
		return ;
	}
	//BYTE ORDER��host byte orderת��
	destip = ntohl(destip);
	
	RTPUDPv4TransmissionParams transparams;
	RTPSessionParams sessparams;
	
	//3��ʱ�������
	sessparams.SetOwnTimestampUnit(TIME_STAMP);
	sessparams.SetAcceptOwnPackets(true);
	transparams.SetPortbase(portbase);

	//4�������Ự
	status = sess.Create(sessparams,&transparams);	
//	checkerror(status);
	RTPIPv4Address addr(destip,destport);
	status = sess.AddDestination(addr);
//	checkerror(status);

	LOGD("LocalPort:%d",portbase);
	LOGD("DestPort:%d",destport);
	LOGD(DEST_IP);
	LOGD("-ready!");

	//5�����ͼ��������ݰ�
	for (i = 1 ; i <= num ; i++)
	{
		// send the packet
		status = sess.SendPacket((void *)"1234567890",10,0,false,10);

		LOGD("send packet:%d",i);

#ifndef RTP_SUPPORT_THREAD
		status = sess.Poll();
//		checkerror(status);
#endif // RTP_SUPPORT_THREAD
		
		RTPTime::Wait(RTPTime(1,0));
	}
	
	sess.BYEDestroy(RTPTime(10,0),0,0);


	return;
}

