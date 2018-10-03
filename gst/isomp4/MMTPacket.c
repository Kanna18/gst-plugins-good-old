/*******************************************************************************************
 *  License:This property belongs to Gaian soultions India pvt ltd.It can not be distributed.
 *  Project: ATSC 3.0
 *  Version: 1.0
 *  File Name: MMTPacket.c
 *  Created on: 12-Feb-2018
 *  Author: Trimurthulu Amaradhi
 *******************************************************************************************/

#include"MMTPacket.h"
#include "mmtsignal.h"
#include "MMTBitstream.h"
#include "qtdemux.h"
#include <stdlib.h>
#include <string.h>
//#include "parser.h"

int SigFlag=0;
unsigned short int AudioFlag;
unsigned short int VideoFlag;
struct MMTHSample mmthsample;
//FILE *fw;

unsigned int MmtpHDRCount;
struct PA_message PA_Msg;  //PA_Message
struct MPI_message MPI_Msg; //MPI_Message

void ParseAudioAsset(unsigned char* Bitstream)
{

}

void ParseMPU(unsigned char* Bitstream)
{
	struct MPUBOX Mpu;
	unsigned char Byte=Read8(Bitstream);
	unsigned char localByte=Byte;
	Mpu.is_complete=localByte&0x01;   //LSB considered first bit
	Mpu.is_adc_present=localByte&0x02;
	Mpu.reserved=localByte&0xFC;      //6 bits reserved.
	Mpu.mpu_sequence_number =Read32(Bitstream+1);
	GST_DEBUG_OBJECT(NULL,"  MPU Sequence Number %d \n ",Mpu.mpu_sequence_number);
	Mpu.asset_id_scheme=Read32(Bitstream+1+4);
	GST_DEBUG_OBJECT(NULL,"  MPU Sequence Number %d \n ",Mpu.asset_id_scheme);
	Mpu.asset_id_length=Read32(Bitstream+1+4+4);
	GST_DEBUG_OBJECT(NULL,"  Mpu.asset_id_length %d \n",Mpu.asset_id_length);
	Mpu.asset_id_value[Mpu.asset_id_length]=Read32(Bitstream+1+4+4+4);

}

unsigned char* MMTPacketStart(unsigned char* Bitstream)
{


	if((Bitstream[0]==0x40) && ((Bitstream[1]==0x00)  || (Bitstream[1]==0x02))  )
	{
		if(Bitstream[1]==0x02)
		{
			SigFlag=1;
			return Bitstream;

		}
	}
		return Bitstream;

}

void ParsePAsignal(unsigned char* Bitstream)
{
	struct PA_message PAMsg;
	PAMsg.PAmsg.message_id=Read16(Bitstream);
	Bitstream=Bitstream+2;
	PAMsg.PAmsg.version=Read8(Bitstream);
	Bitstream++;
	PAMsg.PAmsg.lenght=Read32(Bitstream);
	Bitstream=Bitstream+4;
	PAMsg.number_of_tables=Read8(Bitstream);
	if(PAMsg.number_of_tables>0)
		for(int i=0;i<PAMsg.number_of_tables;i++)
		{
			PAMsg.PATabs.table_id=Read8(Bitstream+i+1);
			PAMsg.PATabs.table_version=Read8(Bitstream+i+2);
			PAMsg.PATabs.table_length=Read16(Bitstream+i+3);
			//table()  // Need to understand better and add the table instace of the PA
		}
}

void ParseMPTTable(unsigned char* Bitstream )
{
	struct MPT_message MPTmsg;
	MPTmsg.message_id=Read16(Bitstream);
	Bitstream=Bitstream+2;
	MPTmsg.version=Read8(Bitstream);
	Bitstream++;
	MPTmsg.length=Read16(Bitstream);
	ParseMP_Table(Bitstream);
	GST_DEBUG_OBJECT(NULL," messageid: %u, version %u, length: %u\n", MPTmsg.message_id, MPTmsg.version, MPTmsg.length);

}

void ParseMP_Table(unsigned char* Bitstream)
{
	struct MP_Tables MP_Tab;
	unsigned char TempResv;
	int N1, N2, i;
	MP_Tab.table_id=Read8(Bitstream);
	Bitstream++;
	MP_Tab.version=Read8(Bitstream);
	Bitstream++;
	MP_Tab.lenght=Read16(Bitstream);
	Bitstream=Bitstream+2;
	TempResv=Read8(Bitstream);
	MP_Tab.reserved=(TempResv)&0xF8;
	MP_Tab.Mp_table_mode=(TempResv)&0x03;
	if(MP_Tab.table_id==0x20 || MP_Tab.table_id==0x11)
	{
		Bitstream++;
		N1=MMT_package_id;
		MP_Tab.MMT_package_id_length=Read8(Bitstream);
		for(i=0;i<N1;i++)
		{   Bitstream++;
			MP_Tab.MMT_package_id_byte=Read8(Bitstream);

		}
	}
	//MP Table Descriptios.
	Bitstream++;
	N2=MP_table_descriptors;
	MP_Tab.MP_table_descriptors_length=Read16(Bitstream);
	Bitstream=Bitstream+2;
	for(i=0;i<N2;i++)
	{
		MP_Tab.MP_table_descriptors_byte=Read8(Bitstream);
		Bitstream++;
	}

}
void ParseMMTPacket(unsigned char* Bitstream)
{
#if 0
	if(Bitstream[0] & 0x40)
		ParseMMTPV1Pack(Bitstream);
	else
		ParseMMTPV0Pack(Bitstream);
#endif

}


void ParseMMTPackage(unsigned char* Bitstream)
{



}

unsigned int ParseMMTPHDR(unsigned char* Bitstream)
{
	unsigned char FirstByte;
	int PackCountFlag=0;
	int byte_count=0;
	unsigned int PacketCounter;
	unsigned short int reliability_flag;
	unsigned char FECType;
	unsigned char SecByte;
	unsigned short int PacketID;
	unsigned int timestamp;
	unsigned int Seq_Number;
	//FILE *Param5;
	//Parse the Counter bit enabled or not
	//unsigned short int CounterBit = (FirstByte & 0x20);
	unsigned char PakCountBit;
	unsigned short int r_TB_DS_TP_flow_label, DS, Tp, Flow_label, Header_extension, generic_flag, payld_length;
	unsigned char TB;
	int HeaderExtFlag = Header_extension = generic_flag = 0;
	//switch(r_TB_DS_TP_flow_label&0x6000)
	//if(FirstByte & 0x20)
	reliability_flag = MmtpHDRCount=0;
	//Param5 = fopen( "Test2_MMTHeder_Values.csv" , "a");
	//if((Bitstream[0] != 0x40) && (Bitstream[0] != 0x42))
	if(0)
	{
		GST_DEBUG_OBJECT(NULL," IIIIIIIIIIII first 4 byte: %x %x %x %x\n", Bitstream[0], Bitstream[1], Bitstream[2], Bitstream[3]);
	   return 3;
	}
		GST_DEBUG_OBJECT(NULL," IIIIIIIIIIII first 4 byte: %x %x %x %x\n", Bitstream[0], Bitstream[1], Bitstream[2], Bitstream[3]);
	FirstByte=Read8(Bitstream);
	PakCountBit=(FirstByte & 0x20)>>5;
	if(PakCountBit)
	{

		GST_DEBUG_OBJECT(NULL,"  packet counter bit is enabled ");
		PackCountFlag=1;

	}
	else
	{
		GST_DEBUG_OBJECT(NULL," No packet counter bit \n ");
	}
	////fprintf(Param5,"MMTP Packet counter %d ,",PakCountBit);
	//Parse FEC combination type   00 .. 01 ..10 ..11
	//unsigned char FEC_type=Bitstream[0]  0x18;
	FECType=(FirstByte & 0x18);
	FECType=FECType>>3;
	//switch(FirstByte & 0x18)
	switch(FECType)
	{
		case 0:
			GST_DEBUG_OBJECT(NULL,"  MMTP Packet without Source_FEC_Field_ID \n");
		   //return 3;
			break;
		case 1:
			GST_DEBUG_OBJECT(NULL,"  MMTP Packet  Source_FEC_Pyaload_Field_ID");
			break;
		case 2:
			GST_DEBUG_OBJECT(NULL,"  MMTP Packet  for repair symbols for FEC Payload Mode 0");
			break;
		case 3:
			GST_DEBUG_OBJECT(NULL,"  MMTP Packet  for repair symbols for FEC Payload Mode 1");
			break;
		default:
			GST_DEBUG_OBJECT(NULL," None of the above");
			break;

	}
	////fprintf(Param5,"FEC type %d ,",FECType);
	//Parse X bit if it is 1  header extension is there

	//unsigned char HeaderExtBit=Bitstream & 0x04;
	if(FirstByte & 0x04)
	{
		GST_DEBUG_OBJECT(NULL,"  Header is extension bit is enabled.");
		HeaderExtFlag=1;
	}
	//fprintf(Param5,"FEC type %d ,",FirstByte & 0x04);
	//Parse R bit .. This indicates Random Access type (RAP) is there in the stream.
	//unsigned char RAPBit=Bitstream & 0x02;
	if(FirstByte & 0x02)
	{
		GST_DEBUG_OBJECT(NULL,"  RAP  bit is enabled \n");
	}
	 //TODO:// SAFWANN.. NEED TO HANDLE THIS CASE TOO
	//Q bit for the future.
	//unsigned char QBit=Bitstream & 0x01;
	if(FirstByte & 0x01)
	{
		GST_DEBUG_OBJECT(NULL,"  Q bit is enabled \n");
		  //return 3;
	}

	Bitstream=Bitstream+1;
	 byte_count +=1;
	MmtpHDRCount++;
	SecByte=Read8(Bitstream);

	if(SecByte&0x80)
	{
		GST_DEBUG_OBJECT(NULL," Flow bit is enabled \n");
	}

	if(SecByte&0x40)
	{
		GST_DEBUG_OBJECT(NULL," E bit  is enabled \n");
	}
	if(SecByte&0x20)
	{
		GST_DEBUG_OBJECT(NULL," B bit is enabled\n");
	}

	if(SecByte&0x10)
	{
		GST_DEBUG_OBJECT(NULL," I bit is enabled \n ");
	}

	//switch(SecByte&0x08) //Payload type
	switch(SecByte&0x0F)
	{
		case 0x0:   //MPU
			GST_DEBUG_OBJECT(NULL,"  PayLoad type MPU is  enabled \n");
			break;

		case 0x1:  //Generic object
			GST_DEBUG_OBJECT(NULL,"  PayLoad type Generic object is  enabled  return -5\n");
		   generic_flag = 1;
		   return 3;
			break;
		case 0x2:  //signalling message
			GST_DEBUG_OBJECT(NULL,"  PayLoad type signalling message is  enabled \n");
		  return 0x02;
			break;
		case 0x3:  //repair symbol
			GST_DEBUG_OBJECT(NULL,"  PayLoad type repair symbol message is  enabled \n");
		  return 0x04;
			break;
		case 0x4 ... 0x9: //0x4 to 0x9  reserved for ISO
			GST_DEBUG_OBJECT(NULL,"  PayLoad type 0x4 to 0x9  reserved for ISO is  enabled \n");
		  return 0x04;
			break;
		case 0xA ... 0xF:
			GST_DEBUG_OBJECT(NULL,"  PayLoad type 0xA to 0xF  private use \n");
		  return 0x04;
			break;  //0xA to 0xF  private use
		default:
			GST_DEBUG_OBJECT(NULL,"  PayLoad type none of the above use \n");
			break;  //none of the above.
	}
	Bitstream=Bitstream+1;
	 byte_count +=1;
	MmtpHDRCount++;
	PacketID=Read16(Bitstream);
	//fprintf(Param5,"MMTP Packet ID %d ,",PacketID);
	Bitstream=Bitstream+2;
	 byte_count +=2;
	MmtpHDRCount=MmtpHDRCount+2;
	 if((PacketID==0x0015) || (PacketID==0x000C))
	//if(PacketID==0x0015)
	{
		AudioFlag=1;			//FILE *Param2;
		//Param2 = fopen( "Test2_40000015.csv" , "a");

		//fprintf(Param5,"PakID %x ,",PacketID);
		timestamp=Read32(Bitstream);
		//fprintf(Param5,"timestamp %x ,",timestamp);
		GST_DEBUG_OBJECT(NULL," A timestamp %u ,",timestamp);
		Bitstream=Bitstream+4;
	   byte_count +=4;
		MmtpHDRCount=MmtpHDRCount+4;
		Seq_Number=Read32(Bitstream);
		//fprintf(Param5,"Seq_Number %x , ",Seq_Number);
		GST_DEBUG_OBJECT(NULL," Seq_Number %x , ",Seq_Number);

		if(PackCountFlag)
		{
			Bitstream=Bitstream+4;
	      byte_count +=4;
			MmtpHDRCount=MmtpHDRCount+4;
			PacketCounter=Read32(Bitstream);
		}
		//fclose(Param5);

	}
	else if((PacketID==0x0014) || (PacketID==0x000B))
	{  VideoFlag=1;

		//FILE *Params;
		//Params = fopen( "Test2_40000014.csv" , "a");
		//fprintf(Param5,"PakID %x ,",PacketID);
		timestamp=Read32(Bitstream);                
		//fprintf(Param5,"timestamp %x ,",timestamp);
		Bitstream=Bitstream+4;
	   byte_count +=4;
		MmtpHDRCount=MmtpHDRCount+4;
		Seq_Number=Read32(Bitstream);
		//fprintf(Param5,"Seq_Number %x ,",Seq_Number);

		if(PackCountFlag)
		{
			Bitstream=Bitstream+4;
	      byte_count +=4;
			MmtpHDRCount=MmtpHDRCount+4;
			PacketCounter=Read32(Bitstream);
		}
		//fclose(Param5);

	}

	//Read the two bytes to be read
	//r | TB| DS| TP | flow_label
	Bitstream=Bitstream+4;
	   byte_count +=4;
	MmtpHDRCount=MmtpHDRCount+4;
	r_TB_DS_TP_flow_label=Read16(Bitstream);

	if(r_TB_DS_TP_flow_label & 0x8000)
	{
		reliability_flag=1;
	}
	TB=(r_TB_DS_TP_flow_label&0x6000)>>13;
	fprintf(stderr, "PacketCounter: %u, reliability_flag: %u\n", PacketCounter, reliability_flag);
	//switch(r_TB_DS_TP_flow_label&0x6000)
	switch(TB)
	{
		case 00:
			GST_DEBUG_OBJECT(NULL,"  \n Constant bit Rate \n");
			break;
		case 01:
			GST_DEBUG_OBJECT(NULL,"  \n Variable Bit rate \n");
			break;
		case 10:
		case 11:
			GST_DEBUG_OBJECT(NULL,"  \n Reserved for the future use \n");
			break;
		default:
			break;
	}

	DS=r_TB_DS_TP_flow_label&0x1C00;
	DS=DS>>10;
	switch(DS) //DS: Delay_Sensitivity
	{
		case 7://111:
			GST_DEBUG_OBJECT(NULL," \n Conversational service \n");
			break;
		case 6://110:
			GST_DEBUG_OBJECT(NULL," \n Live streaming service \n");
			break;
		case 5://101:
			GST_DEBUG_OBJECT(NULL," \n Delay Sensitive inter-acitve service \n");
			break;
		case 4://100:
			GST_DEBUG_OBJECT(NULL," \n interactive service \n");
			break;
		case 3://011:
			GST_DEBUG_OBJECT(NULL," \n Streaming service \n");
			break;
		case 2://010:
			GST_DEBUG_OBJECT(NULL," \n non real time service \n");
			break;
		case 1://001:
		case 0://000:
			GST_DEBUG_OBJECT(NULL," \n reserved \n");
			break;

	}

	Tp =(r_TB_DS_TP_flow_label & 0x0380);  //Transmission_Priority
	GST_DEBUG_OBJECT(NULL," \n Tp Priority %d \n",Tp);

	Flow_label=r_TB_DS_TP_flow_label & 0x007F;
	//if header extension flag is set to 1.. i.e X=1
	if(HeaderExtFlag)
	{
		//TODO: SAFWAN NEED CHANGES HERE
		GST_DEBUG_OBJECT(NULL," Flow_label: %u, length: %u\n", Flow_label, Read16(Bitstream));
		Bitstream=Bitstream+2;
	   byte_count +=4;
		Header_extension=Read16(Bitstream);
		MmtpHDRCount=MmtpHDRCount+2;
	}
	 GST_DEBUG_OBJECT(NULL," Flow_label: %u, Header_extension: %u\n", Flow_label, Header_extension);
	//Payload header parsing

	//Payload Header length.


	Bitstream=Bitstream+2;
	   byte_count +=4;
	 if(generic_flag)
	 {
		  Bitstream = Bitstream+6;
	   byte_count +=4;
			GST_DEBUG_OBJECT(NULL,"  GGGGGGGGGGGGGGNNNNNNNNNNNN: start offset: %06lx byte_count: %d", Read48(Bitstream), byte_count);
	 }
	MmtpHDRCount=MmtpHDRCount+2;
	payld_length=Read16(Bitstream);   //Pay load length...starts the next MMTP Packet.
	if(SigFlag)
	{   Bitstream=Bitstream+2;
		MmtpHDRCount=MmtpHDRCount+2;
		payld_length=Read16(Bitstream);
		SigFlag=0;
		return payld_length;
	}
	//fprintf(Param5,"Payld %x \n ",payld_length);

	//fclose(Param5);
	return payld_length;


}


int ParseSigHdr(unsigned char*  Bitstream)
{

	int SigMsgLen, f_i, fragment_counter;
	unsigned char FirstByte;

	unsigned short Aggregation_flag;
	unsigned short H_flag;
	unsigned short res;
   FirstByte = Read8(Bitstream);
	f_i = (FirstByte&0xc0)>>6;
	MmtpHDRCount=0;
	SigMsgLen=0;
	FirstByte=Read8(Bitstream);
	Aggregation_flag = FirstByte&0x01 ;
	H_flag	= (FirstByte&0x02)>>1 ;
	res = (FirstByte&0x3c) >> 2 ;

	switch(f_i)
	{
		case 0:
			GST_DEBUG_OBJECT(NULL," Payload contains one or more complete signalling messages.\n");
			break;
		case 1:
			GST_DEBUG_OBJECT(NULL," Payload contains the last fragment of a signalling message.\n");
			break;
		case 2:
			GST_DEBUG_OBJECT(NULL," Payload contains the first fragment of a signalling message.\n");
			break;
		case 3:
			GST_DEBUG_OBJECT(NULL," Payload contains a fragment of a signalling message that is neither the first nor the last fragment.\n");
			break;
		default:
			GST_DEBUG_OBJECT(NULL," None of the above\n");
	}

	Bitstream = Bitstream +1;
	MmtpHDRCount = MmtpHDRCount+1;
	fragment_counter = Read8(Bitstream); // fragment_counter is 0 if aggregation_flag is set 1
	Bitstream = Bitstream +1;
	MmtpHDRCount = MmtpHDRCount+1;

	/*
	   if(H_flag)
	   {	
	   SigMsgLen = Read32(buffer+2);
	   Bitstream = Bitstream +4;
	   MmtpHDRCount = MmtpHDRCount+4;
	   }
	//else
	 */
	{
		SigMsgLen  = Read16(Bitstream);
		Bitstream = Bitstream +2;
		MmtpHDRCount = MmtpHDRCount+2;
	}

	//	FILE *file_sig=NULL;
	//file_sig = fopen("sig_file.csv","a");
	//fprintf(file_sig,"f_i=%d,res=%hd,Aggregation_flag=%hd,H_flag=%hd,",f_i,res,Aggregation_flag,H_flag );
	GST_DEBUG_OBJECT(NULL," f_i=%d,res=%hd,Aggregation_flag=%hd,H_flag=%hd, fragment_counter: %d",f_i,res,Aggregation_flag,H_flag, fragment_counter );
	//fprintf(file_sig,"fragment_counter=%hd,payld_length=%u\n",fragment_counter,SigMsgLen);
	//fclose(file_sig);

	return SigMsgLen;

}

#if 0 
int ParseSigPayldHDR(unsigned char*  Bitstream)
{

	unsigned char FirstByte, SecByte;
	int PackCountFlag;
	unsigned int timestamp, Seq_Number;
	int HeaderExtFlag;
	unsigned short int reliability_flag, PacketID, r_TB_DS_TP_flow_label, Flow_label, Header_extension;
   Header_extension = 0;

	MmtpHDRCount=0;
	Bitstream=Bitstream+1;
	MmtpHDRCount++;
	SecByte=Read8(Bitstream);
	Bitstream=Bitstream+1;
	MmtpHDRCount++;
	PacketID=Read16(Bitstream);
	Bitstream=Bitstream+2;
	MmtpHDRCount=MmtpHDRCount+2;
	timestamp=Read32(Bitstream);
	Bitstream=Bitstream+4;
	MmtpHDRCount=MmtpHDRCount+4;
	Seq_Number=Read32(Bitstream);
	FirstByte=Read8(Bitstream);
	PackCountFlag=0;
	HeaderExtFlag=0;
	reliability_flag=0;

	GST_DEBUG_OBJECT(NULL,"  \n MMTP signal Sequence Number %x \n",Seq_Number);

	//Read the two bytes to be read
	//r | TB| DS| TP | flow_label
	Bitstream=Bitstream+4;
	MmtpHDRCount=MmtpHDRCount+4;
	r_TB_DS_TP_flow_label=Read16(Bitstream);
	Flow_label=r_TB_DS_TP_flow_label & 0x007F;
	//if header extension flag is set to 1.. i.e X=1
	if(HeaderExtFlag)
	{
		Bitstream=Bitstream+2;
		MmtpHDRCount=MmtpHDRCount+2;
		Header_extension=Read16(Bitstream);
	}


	/********************************************************************************************/

	Bitstream=Bitstream+2;
	MmtpHDRCount=MmtpHDRCount+2;
	//file_sig = fopen("sig_file.csv","a");
	//fprintf(file_sig,"Signal PacketID=%hd,timestamp=%u,Seq_Number=%u,",PacketID,timestamp,Seq_Number );
	//fclose(file_sig);
   GST_DEBUG_OBJECT(NULL," FirstByte: %u, SecByte: %u, PackCountFlag: %u, Header_extension: %u, Flow_label: %u, PacketID: %u, reliability_flag: %u, timestamp: %u\n", FirstByte, SecByte, PackCountFlag, Header_extension, Flow_label, PacketID, reliability_flag, timestamp);

	return 0;

}
#endif

#if 0
struct MMTHSample MMTHSampleFun(struct MMTHSample mmthsample)
{
	return mmthsample;
}
#endif


int ParseMMTPV1Pack(GstQTDemux * qtdemux, unsigned char*  Bitstream, GstBuffer *gbuf)
{
	//MmtpHDRCount=0;
	unsigned int ref_offset = 0;
#if 1
	unsigned int lmoof_offset, incr;
#endif
	unsigned short int DU_HDR_ItemID;
	unsigned short int FT_T_f_i_A_Frag_Cnter, FT, f_i;
   unsigned char Fragemnt_counter;
	 unsigned int Mpu_Sequence_Number;
	//unsigned char FirstByte=Read8(Bitstream);
	unsigned int AggrFlag;
	unsigned int MPU_Timed_data=0;
	//unsigned int A=0;
	unsigned short int payld_length=Read16(Bitstream);   //Pay load length...starts the next MMTP Packet.
	 unsigned char * PayLoadBuf;
	 if(payld_length <= 1)
		  return -1;
#if 1
    lmoof_offset = 0;
	 for(incr = 0; incr < payld_length; incr++)
	 {
		 if(!memcmp(Bitstream+incr, "moof", 4))
		 {
			 lmoof_offset = incr;
			 break;
		 }
	 }
	 for(incr = 0; incr < payld_length; incr++)
	 {
		 if(!memcmp(Bitstream+incr, "ffffffffffffffffffffffff", 10))
		 {
			 GST_DEBUG_OBJECT(NULL, "CCCCCCCCCcorrupted packet, ignore");
		    return -1;
		 }
	 }
	// qtdemux->ref_offset +=2;
#endif
   GST_DEBUG_OBJECT( NULL, "SSSS==================== payld_length: %d %03x  ::::: \n", payld_length, payld_length);

	//unsigned short int PayHDRsize=20;//24;

	//copy the payload into the dynamic buffer.
	PayLoadBuf=(unsigned char*)malloc(sizeof(unsigned char)*(payld_length-6));
   if(PayLoadBuf == NULL)
		  GST_DEBUG_OBJECT(NULL, "PAYYYYYYYYLOAD, memory not allocated");
	//memset(PayLoadBuf,0,(payld_length-6));
	//unsigned char *PayLoadBuf=(unsigned char*)malloc(sizeof(unsigned char)*(payld_length-6));
	//unsigned int PayloadStartAddress=Bitstream+14+2;
	//TODO: why crash here?
	memset(PayLoadBuf,'\0',payld_length-6);


#if 1

	//Bitstream=Bitstream+2+6;
	memcpy(PayLoadBuf,(Bitstream+2+6),payld_length-6);
	 ref_offset += 8;

	//memcpy(PayLoadBuf,(Bitstream+8),payld_length);

#else
	memcpy(PayLoadBuf,(Bitstream+2+22),payld_length-22);
#endif
	Bitstream=Bitstream+2;
   ref_offset += 2;
	FT_T_f_i_A_Frag_Cnter=Read16(Bitstream);
	FT=FT_T_f_i_A_Frag_Cnter &  0xF000;
	GST_DEBUG_OBJECT(NULL,"  \n FT_T_f_i_A_Frag_Cnter %x \n ",FT_T_f_i_A_Frag_Cnter);
	GST_DEBUG_OBJECT(NULL,"  \n FT: %x \n ",FT);
	FT=FT>>12;

	switch(FT)
	{
		case 00:  //No additional DU headers
			GST_DEBUG_OBJECT(NULL,"  \n MPU: MPU Meta data ftyp,mmpu,mov,metaboxes etc \n ");
			break;
		case 01://No additional DU headers
#if 1
		   qtdemux->moof_offset = qtdemux->packet_offset+lmoof_offset;
			GST_DEBUG_OBJECT(NULL," \n Movie Fragment Meta data ....Mooof box and mdat box moof_offset: %"G_GUINT64_FORMAT, qtdemux->moof_offset);
#endif
			break;
		case 02:
			GST_DEBUG_OBJECT(NULL," \n MFU: contains a sample or subsample of timed media data or an item of non-timed data \n");
			break;
		case 03 ...15:
			GST_DEBUG_OBJECT(NULL," \n Reserved for private use \n");
			break;
		default:
			break;
	}

	if(FT_T_f_i_A_Frag_Cnter & 0x0800)
	{
		MPU_Timed_data=1;  // for non-timed data it is 0
	}
	f_i=((FT_T_f_i_A_Frag_Cnter & 0x0600)>>13);
	//switch(((FT_T_f_i_A_Frag_Cnter & 0x0600)>>7))
	switch(f_i)
	{
		case 00:
			GST_DEBUG_OBJECT(NULL," \n Payload contains one or more complete data units. \n");
			break;
		case 01:
			GST_DEBUG_OBJECT(NULL,"  \n Payload contains the first fragment of data unit \n");
			break;
		case 02:
			GST_DEBUG_OBJECT(NULL,"  \n Payload contains the fragment of data unit that is neither first nor the last data unit \n");
			break;
		case 03:
			GST_DEBUG_OBJECT(NULL,"  \n Payload contains last fragment of the data unit \n");
			break;

	}
	AggrFlag=FT_T_f_i_A_Frag_Cnter & 0x0100;
	if((AggrFlag)){
		GST_DEBUG_OBJECT(NULL,"  \n 1: More than one data unit is present in the payload  \n");
		//A=1;
	}
	if(!AggrFlag)
	{

	}

	Fragemnt_counter =FT_T_f_i_A_Frag_Cnter & 0x00FF;
	GST_DEBUG_OBJECT(NULL,"  \n 1: Fragemnt_counter %d \n",Fragemnt_counter);
	//Fragemnt_counter : No of Fragments of the same data MFUs.

	GST_DEBUG_OBJECT(NULL,"  \n XXXXXXXXXXXXX %04x %04x\n",Read32(Bitstream), Read32(Bitstream+4));
	Bitstream=Bitstream+2;
	 ref_offset +=2;
	GST_DEBUG_OBJECT(NULL,"  \n YYYYYYYYYYYY %04x %04x\n",Read32(Bitstream), Read32(Bitstream+4));
	Mpu_Sequence_Number=Read32(Bitstream);
	GST_DEBUG_OBJECT(NULL,"  \n 1: Mpu_Sequence_Number %d %04x %04x\n",Mpu_Sequence_Number, Mpu_Sequence_Number, Read32(Bitstream+4));
	//DU_Length
	Bitstream=Bitstream+4;
	 ref_offset += 4;
	if(AggrFlag)
		 Bitstream += 2;
	 
	//unsigned short int DU_Length=Read16(Bitstream);

	//Dumping from DU Header
	//unsigned char *Filedump=(unsigned char*)malloc(sizeof(unsigned char)*(payld_length-6));
	//unsigned char *Filedump=(unsigned char*)malloc(sizeof(unsigned char)*(payld_length-8));
	//memcpy(Filedump,(Bitstream+2),(payld_length-6));
	//memcpy(Filedump,(Bitstream+2),(payld_length-8));
	//unsigned char *Filedump=(unsigned char*)malloc(sizeof(unsigned char)*(payld_length-6));
	//memcpy(Filedump,(Bitstream),(payld_length-6));

	 GST_DEBUG_OBJECT(NULL," CHECKKK MPU_Timed_data: %d......... VideoFlag: %d AudioFlag: %d\n", MPU_Timed_data, VideoFlag, AudioFlag);
	if(MPU_Timed_data)
	{


		if(VideoFlag)
		{

			//FILE *Param3;
			//Param3 = fopen( "Test2_40000014_Video_MFU.csv" , "a");
			//For Timed data DU Header
			//Bitstream=Bitstream+2;
			//unsigned int movie_fragment_Seq_number=Read32(Bitstream);
			mmthsample.movie_fragment_sequence_number=Read32(Bitstream);
			//fprintf(Param3,"MFU Seq_number %d ,",mmthsample.movie_fragment_sequence_number);
			//GST_DEBUG_OBJECT(NULL,"  Movie Fragment Sequence number %x \n ",mmthsample.movie_fragment_sequence_number);
			Bitstream=Bitstream+4;
		   ref_offset += 4;
			//unsigned int sample_number=Read32(Bitstream);
			mmthsample.SampleNumber=Read32(Bitstream);
			//fprintf(Param3,"MFU Sample Number %d ,",mmthsample.SampleNumber);
			//GST_DEBUG_OBJECT(NULL,"  Sample Number %x \n ",mmthsample.SampleNumber);
			Bitstream=Bitstream+4;
		   ref_offset += 4;
			//unsigned int offset=Read32(Bitstream);
			mmthsample.offset=Read32(Bitstream);
			//fprintf(Param3,"MFU Sample offset %d ,",mmthsample.offset);
			//GST_DEBUG_OBJECT(NULL,"  offset %x \n ",mmthsample.offset);

			Bitstream=Bitstream+4;
		   ref_offset +=4;
#if 0
			mmthsample.length=Read32(Bitstream);
			//fprintf(Param3,"MFU length %x ",mmthsample.length);
			Bitstream=Bitstream+4;
#endif
			mmthsample.priority_dep_counter=Read16(Bitstream);
		   ref_offset +=2;
			//unsigned char priority=priority_dep_counter & 0xFF00;
			mmthsample.priority=mmthsample.priority_dep_counter & 0xFF00;
			mmthsample.dependency_counter=mmthsample.priority_dep_counter & 0x00FF;
			//GST_DEBUG_OBJECT(NULL,"  dep_counter  %x \n",mmthsample.dependency_counter);
			//fprintf(Param3,"MFU dep_counter %d , ",mmthsample.dependency_counter);
			//fprintf(Param3,"Fragemnt_counter %d , ",Fragemnt_counter);
			//fprintf(Param3,"Fragemnt_indicator %d , ",f_i);
			//fprintf(Param3,"Aggregation flag %d , ",AggrFlag);
			//fprintf(Param3,"Mpu_Sequence_Number %d  \n",Mpu_Sequence_Number);

			//fclose(Param3);
		}
		else if(AudioFlag)
		{

			//For Timed data DU Header
			//FILE *Param4;
			//Param4 = fopen( "Test2_40000015_MFU_Audio.csv" , "a");
			//For Timed data DU Header
			//Bitstream=Bitstream+2;
			//unsigned int movie_fragment_Seq_number=Read32(Bitstream);
			mmthsample.movie_fragment_sequence_number=Read32(Bitstream);
			//fprintf(Param4,"MFU Seq_number %d ,",mmthsample.movie_fragment_sequence_number);
			//GST_DEBUG_OBJECT(NULL,"  Movie Fragment Sequence number %x \n ",mmthsample.movie_fragment_sequence_number);
			Bitstream=Bitstream+4;
		   ref_offset +=4;
			//unsigned int sample_number=Read32(Bitstream);
			mmthsample.SampleNumber=Read32(Bitstream);
			//fprintf(Param4,"MFU Sample Number %d ,",mmthsample.SampleNumber);
			//GST_DEBUG_OBJECT(NULL,"  Sample Number %x \n ",mmthsample.SampleNumber);
			Bitstream=Bitstream+4;
		   ref_offset +=4;
			//unsigned int offset=Read32(Bitstream);
			mmthsample.offset=Read32(Bitstream);
			//fprintf(Param4,"MFU Sample offset %d ,",mmthsample.offset);
			//GST_DEBUG_OBJECT(NULL,"  offset %x \n ",mmthsample.offset);

			Bitstream=Bitstream+4;
		   ref_offset +=4;
#if 0
			mmthsample.length=Read32(Bitstream);
			//fprintf(Param4,"MFU length %x ",mmthsample.length);
			Bitstream=Bitstream+4;
#endif
			mmthsample.priority_dep_counter=Read16(Bitstream);
			//unsigned char priority=priority_dep_counter & 0xFF00;
			mmthsample.priority=mmthsample.priority_dep_counter & 0xFF00;
			mmthsample.dependency_counter=mmthsample.priority_dep_counter & 0x00FF;
		   ref_offset +=2;
			//GST_DEBUG_OBJECT(NULL,"  dep_counter  %x \n",mmthsample.dependency_counter);
			//fprintf(Param4,"mmthsample priority %d , ",mmthsample.priority);
			//fprintf(Param4,"MFU dep_counter %d , ",mmthsample.dependency_counter);
			//fprintf(Param4,"Fragemnt_counter %d , ",Fragemnt_counter);
			//fprintf(Param4,"Fragemnt_indicator %d , ",f_i);
			//fprintf(Param4,"Aggregation flag %d , ",AggrFlag);
			//fprintf(Param4,"Mpu_Sequence_Number %d \n",Mpu_Sequence_Number);

			//fclose(Param4);

		}
	   GST_DEBUG_OBJECT(NULL, "movie_fragment_sequence_number: %u, sample_number: %u, offset: %u", mmthsample.movie_fragment_sequence_number, mmthsample.SampleNumber, mmthsample.offset); 






	}
	else// Non-timed DU Header
	{
		Bitstream=Bitstream+2;
		ref_offset +=2;
		DU_HDR_ItemID=Read32(Bitstream);
		ref_offset +=4;
		  GST_DEBUG_OBJECT(NULL," DU_HDR_ItemID: %u\n", DU_HDR_ItemID);

	}
#if 0
	unsigned char *Filedump=(unsigned char*)malloc(sizeof(unsigned char)*(payld_length+10));
	//memcpy(Filedump,(Bitstream+2),(payld_length-6));  //only DU payload
	memcpy(Filedump,(Bitstream-14),(payld_length+10));  //only DU payload
#endif
	//MMTHSampleFun(mmthsample);
	//qtdemux->ref_offset += ref_offset;
	qtdemux->ref_offset += 8;
	GST_DEBUG_OBJECT (NULL, "HPHPHPHPHPHP offset:%x", (int)qtdemux->ref_offset);
	if(VideoFlag) //
	{

#if 0
		FILE *fw;
		//fw = fopen( "file_Audio.mmt" , "a");
		fw = fopen( "Test2_Video.mmt" , "a");
		if(fw!=NULL)
		{
			//fwrite(PayLoadBuf,payld_length,1,fw);
			fwrite(PayLoadBuf,(payld_length-6),1,fw);
			//fwrite(Filedump,(payld_length-8),1,fw);

		}
		fclose(fw);
#else
		GainMMTdemux(qtdemux, PayLoadBuf,payld_length-6,FT,0, gbuf);
			VideoFlag = 0;
		//GainMMTVideo(PayLoadBuf,payld_length-6,FT);
		//GainMMTVideo(Filedump,payld_length-8,FT);
		//GainMMTVideo(Filedump,payld_length+10,FT);


#endif

	}
	else if(AudioFlag) //
	{

#if 0

		FILE *fwA;
		//fwA = fopen( "file_Video.mmt" , "a");
		fwA = fopen( "Test2_Audio.mmt" , "a");
		if(fwA!=NULL)
		{
			//fwrite(PayLoadBuf,payld_length,1,fwA);
			fwrite(PayLoadBuf,(payld_length-6),1,fwA);
			//fwrite(Filedump,(payld_length-8),1,fwA);
		}

		fclose(fwA);
#else

		GainMMTdemux(qtdemux, PayLoadBuf,payld_length-6,FT, 1, gbuf); 
			AudioFlag = 0;
		//GainMMTAudio(PayLoadBuf,payld_length-6,FT);
		// GainMMTAudio(Filedump,payld_length+10,FT);


#endif
	}
	 return 0;

}

void ParseMMTPV0Pack(unsigned char*  Bitstream )
{


}


void ParseGenSignal(unsigned char* Bitstream )
{

}

void ParseSignalling_Msg(unsigned char* Bitstream)
{
	struct signalling_message Signal;
	Bitstream=Bitstream+2;
	Signal.message_id=Read16(Bitstream);
	Bitstream=Bitstream+2;
	Signal.version=Read8(Bitstream);
	Bitstream=Bitstream+1;
	if((Signal.message_id !=PA_Msg.PAmsg.message_id) && (Signal.message_id !=MPI_Msg.message_id) )
	{

		Signal.lenght=Read16(Bitstream);
	}
	else
	{
		Signal.lenght=Read32(Bitstream);
	}


}



