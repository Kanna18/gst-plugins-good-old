
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <parser.h>
#include"MMTPacket.h"
#include"MMTTables.h"

#define MAX_SIZE 4000 

extern unsigned int MmtpHDRCount;

unsigned long int sig_counter=0;


int ParseSignal(unsigned char * buffer,int Rsize )
{

	int offset=0;

	//unsigned char buffer[MAX_SIZE] = {'\0'};
	//unsigned char TempBuf[MAX_SIZE] = {'\0'};
	unsigned int PayldrLength;

	unsigned int SigMsgLen = 0;
	unsigned char SigMsgPayload[MAX_SIZE] = {'\0'};

	memset(SigMsgPayload,0,sizeof(SigMsgPayload));

	if( buffer[offset+0]==0x40 && buffer[offset+1]==0x02 && buffer[offset+2]==0x00 && buffer[offset+3]==0x14)
	{
		GST_DEBUG_OBJECT(NULL,"qtdemux.c [0x14]Signalling Messaging\n ");			
#if 0		
if (Rsize-offset < 21)
		{
			/*
			unsigned char TempBuf[1024];
			memset(TempBuf,'\0',sizeof(TempBuf));
			int Remain_bytes = Rsize-offset;
			memcpy(TempBuf,buffer+offset,Remain_bytes);
			memset(buffer,0,sizeof(buffer));
			memcpy(buffer,TempBuf,Remain_bytes);
			Rsize = fread(buffer+Remain_bytes,1,MAX_SIZE-Remain_bytes,InputFile);
			offset = 0;
			*/

			return 0;

		}
#endif

		PayldrLength =  ParseSigPayldHDR(&buffer[offset+0]);
		offset = offset + MmtpHDRCount+3 ;
		SigMsgLen = ParseSigHdr(&buffer[offset+0]);
		GST_DEBUG_OBJECT(NULL,"qtdemux.c signal message payload length %d\n",SigMsgLen);
		offset = offset + 4 ; //signal payload header

		if ( Rsize >= SigMsgLen )
		{
			memcpy(SigMsgPayload,buffer+offset,SigMsgLen);
		}
		else
		{ //read and copy remaing bytes of payload

			/*
			unsigned char TempBuf[1024];
			int bytes_available = Rsize-offset;
			memcpy(SigMsgPayload,buffer+offset,bytes_available);
			int Remain_bytes = SigMsgLen - (Rsize-offset);
			memset(TempBuf,'\0',sizeof(TempBuf));
			Rsize = fread(TempBuf,1,Remain_bytes,InputFile);
			memcpy(SigMsgPayload+bytes_available+1,TempBuf,Remain_bytes);
			
			return 0; */

		}

		//ParseSignal(&SigMsgPayload);
		offset = offset + SigMsgLen ;
		sig_counter++;
		GST_DEBUG_OBJECT(NULL,"qtdemux.c signal: %2x %2x %2x %2x\n",buffer[offset],buffer[offset+1],buffer[offset+2],buffer[offset+3]);


	}
	else if( buffer[offset+0]==0x40 && buffer[offset+1]==0x02 && buffer[offset+2]==0x00 && buffer[offset+3]==0x15)
	{
		GST_DEBUG_OBJECT(NULL,"qtdemux.c [0x15]Signalling Messaging\n ");
	#if 0	
                if(Rsize-offset < 21 )
		{/*
			unsigned char TempBuf[1024];
			memset(TempBuf,'\0',sizeof(TempBuf));
			int Remain_bytes = Rsize-offset;
			memcpy(TempBuf,buffer+offset,Remain_bytes);
			memset(buffer,0,sizeof(buffer));
			memset(buffer,TempBuf,Remain_bytes);
			Rsize = fread(buffer+Remain_bytes,1,MAX_SIZE-Remain_bytes,InputFile);
			offset = 0;
			*/
			return 0;

		}
	#endif 

		PayldrLength =   ParseSigPayldHDR(&buffer[offset+0]);
		offset = offset + MmtpHDRCount + 3;
		SigMsgLen = ParseSigHdr(&buffer[offset+0]);
		GST_DEBUG_OBJECT(NULL,"qtdemux.c signal message payload length %d\n",SigMsgLen);
		offset = offset + 4; //signal payload header

		if ( Rsize >= SigMsgLen )
		{
			memcpy(SigMsgPayload,buffer+offset,SigMsgLen);
		}
		else
		{  //read and copy remaing bytes of payload
			/*
			unsigned char TempBuf[1024];
			int bytes_available = Rsize-offset;
			memcpy(SigMsgPayload,buffer+offset,bytes_available);
			int Remain_bytes = SigMsgLen - (Rsize-offset);
			memset(TempBuf,'\0',sizeof(TempBuf));
			Rsize = fread(TempBuf,1,Remain_bytes,InputFile);
			memcpy(SigMsgPayload+bytes_available+1,TempBuf,Remain_bytes);
			*/
			//return 0;

		}
		//ParseSignal(&SigMsgPayload);

		offset = offset + SigMsgLen  ;
		sig_counter++;
		GST_DEBUG_OBJECT(NULL,"qtdemux.c signal: %2x %2x %2x %2x\n",buffer[offset],buffer[offset+1],buffer[offset+2],buffer[offset+3]);

	}
	else if( buffer[offset+0]==0x40 && buffer[offset+1]==0x02 && buffer[offset+2]==0x00 && buffer[offset+3]==0x00)
	{
		GST_DEBUG_OBJECT(NULL,"qtdemux.c [0x00]Signalling Messaging \n");
		if(Rsize-offset < 21)
		{
			/*
			unsigned char TempBuf[1024];
			memset(TempBuf,'\0',sizeof(TempBuf));
			int Remain_bytes = Rsize-offset;
			memcpy(TempBuf,buffer+offset,Remain_bytes);
			memset(buffer,0,sizeof(buffer));
			memcpy(buffer,TempBuf,Remain_bytes);
			Rsize = fread(buffer+Remain_bytes,1,MAX_SIZE-Remain_bytes,InputFile);
			offset = 0;
			*/
			//return 0;

		}

		PayldrLength =   ParseSigPayldHDR(&buffer[offset+0]);
		offset = offset + MmtpHDRCount +3;
		SigMsgLen = ParseSigHdr(&buffer[offset+0]);
		GST_DEBUG_OBJECT(NULL,"qtdemux.c signal message payload length %d PayldrLength: %d\n",SigMsgLen, PayldrLength);
		offset = offset + 4 ;
		if ( SigMsgLen > 0)
		{
			if ( Rsize >= SigMsgLen )
			{
				memcpy(SigMsgPayload,buffer+offset,SigMsgLen);
			}
			else
			{  //read and copy remaing bytes of payload
				/*
				unsigned char TempBuf[MAX_SIZE];
				int bytes_available = Rsize-offset;
				memcpy(SigMsgPayload,buffer+offset,bytes_available);
				int Remain_bytes = SigMsgLen - (Rsize-offset);
				memset(TempBuf,'\0',sizeof(TempBuf));
				Rsize = fread(TempBuf,1,Remain_bytes,InputFile);
				memcpy(SigMsgPayload+bytes_available+1,TempBuf,Remain_bytes);
				*/
				//return 0;

			}
			//ParseSignal(&SigMsgPayload);
			offset = offset + SigMsgLen  ;
			GST_DEBUG_OBJECT(NULL,"qtdemux.c signal[00 00] known : %2x %2x %2x %2x\n",buffer[offset],buffer[offset+1],buffer[offset+2],buffer[offset+3]);
		}
		else if (SigMsgLen == 0)
		{
			#if 0
			offset = offset + SigMsgLen  ;
			//int tempbytes = 0;
			//int index = offset;
			#else
			while(1)
			{
				if (Rsize != offset)
				{
					if(buffer[offset] == 0x40 && buffer[offset+1]==0x00 )
					{
					//	fwrite(buffer+index,1,tempbytes,sig_file);
						break;
					}
					offset ++;
				//	tempbytes++;
				}
				else if(Rsize == offset) {
					/*
					fwrite(buffer+index,1,tempbytes,sig_file);
					memset(buffer,0,sizeof(buffer));
					Rsize = fread(buffer,1,MAX_SIZE,InputFile);
					index = 0;offset=0;
					tempbytes =0;
					*/
					 GST_DEBUG_OBJECT(NULL, "OOOOOOOOOOFFSETSIZE: %d", offset);
					//return 0;
					break;
				}
			}
		  #endif
			GST_DEBUG_OBJECT(NULL,"qtdemux.c signal[00 00]: %2x %2x %2x %2x\n",buffer[offset],buffer[offset+1],buffer[offset+2],buffer[offset+3]);
		}
		sig_counter++;
	}
	else if(buffer[offset+0]==0x6D && buffer[offset+1]==0x6D && buffer[offset+2]==0x74)
	{
		offset++;
		offset++;
		offset++;
	}
	else
	{
		offset ++;
	}

	//fprintf(sig_file,"\n ########Total signal=%d\n",sig_counter);

	return offset;
}

