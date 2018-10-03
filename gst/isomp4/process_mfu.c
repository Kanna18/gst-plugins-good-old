/*********************************************************************************************
 *  License: This property belongs to Gaian soultions India pvt ltd.It can not be distributed.
 *  Project: ATSC 3.0
 *  Version: 1.0
 *  File Name: process_mfu.c
 *  Created on: 04-MAY-2018
 *  Author: Mohammed Safwan Sada 
 *******************************************************************************************/

  


#include <stdio.h>
#include <string.h>
#include "pthread.h"
#include "MMTPacket.h"
#include "atoms.h"
#include "qtdemux.h"
#include "parser.h"
#include "MMTBitstream.h"

#define ASSET_LENGTH 16
//#define BUF_SIZE 4000
#define DUMP_VIDEO 0
#define DUMP_VIDEO1 0
//mfu_box mfu_b;
//struct MMTHSample mmthsample;

void process_mfus(GstQTDemux * qtdemux, unsigned char* data, int size, int mediaType, GstBuffer *gbuf)
{
	int i, offset, mfu_offset, ret, is_muli;
	static int need_data[2], sample_size[2], count[2], sample_number[2];
	static unsigned int rem_size[2];
static GstClockTime pts_ref[2];
#if DUMP_VIDEO
	GstMapInfo map;
	static FILE *fp1 = NULL;
#endif
#if DUMP_VIDEO1
	GstMapInfo map;
	static FILE *fp = NULL;
#endif
	GstBuffer *mfu_buf, *out;
	mfu_buf = out = NULL;
	is_muli = offset = mfu_offset = ret = 0;
	qtdemux->mfu_offset = 0;
#if DUMP_VIDEO1
		  if(fp == NULL)
			fp = fopen("video.data", "w");
#endif
	for(i = 0; i < size; ++i)
	{
		if(memcmp(data+i,"muli",4) == 0)
		{
			count[mediaType] = 0;
			is_muli = 1;
			mfu_offset = i+Read32(data+(i-4))-4;
			qtdemux->mfu_offset = mfu_offset;
			rem_size[mediaType] = size - mfu_offset;
			offset = i- 27;
			parse_mfu(data+offset, rem_size[mediaType]);
			qtdemux->mfu_size = mfu_b.length;
			if(rem_size[mediaType] >= mfu_b.length) 
				need_data[mediaType] = 0;
			else
			{
				sample_size[mediaType] = mfu_b.length;
				sample_number[mediaType] = mfu_b.samplenumber;
				need_data[mediaType] = 1;
			}

			break;
		}
	}
#if DUMP_VIDEO
	 if(fp1 == NULL)
	fp1 = fopen("discrete_video.data", "w");
#endif

	if(is_muli)
	{
		qtdemux->ref_offset +=mfu_offset;
	   if(gbuf)
		{
#if 1
			//mfu_buf = gst_buffer_copy_region (gbuf, GST_BUFFER_COPY_MEMORY, mfu_offset+22, rem_size[mediaType]);
			//mfu_buf = gst_buffer_copy_region (gbuf, GST_BUFFER_COPY_FLAGS | GST_BUFFER_COPY_META | GST_BUFFER_COPY_MEMORY | GST_BUFFER_COPY_TIMESTAMPS, mfu_offset+22, rem_size[mediaType]);
			//mfu_buf = gst_buffer_copy_region (gbuf, GST_BUFFER_COPY_MEMORY | GST_BUFFER_COPY_TIMESTAMPS, mfu_offset+22, rem_size[mediaType]);
		  //mfu_buf = gst_buffer_new_and_alloc(rem_size[mediaType]);
		  //gst_buffer_copy_into (mfu_buf, gbuf, GST_BUFFER_COPY_MEMORY | GST_BUFFER_COPY_META, mfu_offset+22, rem_size[mediaType]);
			mfu_buf = gst_buffer_copy_region (gbuf, GST_BUFFER_COPY_ALL, mfu_offset+22, rem_size[mediaType]);
		//	mfu_buf = gst_buffer_copy_region (gbuf, GST_BUFFER_COPY_META | GST_BUFFER_COPY_FLAGS | GST_BUFFER_COPY_MEMORY | GST_BUFFER_COPY_TIMESTAMPS, mfu_offset+22, rem_size[mediaType]);
		   mfu_buf->pts = gbuf->pts;
	      pts_ref[mediaType] = gbuf->pts;
		  mfu_buf->dts = gbuf->dts;
		  // mfu_buf->offset = gbuf->offset;
		  // mfu_buf->offset_end = gbuf->offset_end;
		   mfu_buf->duration = gbuf->duration;
		   GST_DEBUG_OBJECT (qtdemux,
      "AGAIN CHECK buffer pts:%" GST_TIME_FORMAT " dts:%" GST_TIME_FORMAT
      " offset:%" G_GUINT64_FORMAT " size:%" G_GSIZE_FORMAT " demux offset:%"
      G_GUINT64_FORMAT, GST_TIME_ARGS (GST_BUFFER_PTS (mfu_buf)),
      GST_TIME_ARGS (GST_BUFFER_DTS (gbuf)), GST_BUFFER_OFFSET (gbuf),
      gst_buffer_get_size (mfu_buf), qtdemux->offset);
#else
		  gst_adapter_flush(qtdemux->adapter, mfu_offset+22);
		  mfu_buf = gst_adapter_take_buffer (qtdemux->adapter, rem_size[mediaType]);
		  gst_adapter_flush(qtdemux->adapter, rem_size[mediaType]);
#endif
			GST_DEBUG_OBJECT(NULL, "1DDSIZEEEE %"G_GSIZE_FORMAT "offset:%"G_GUINT64_FORMAT"&& mbuf %"G_GSIZE_FORMAT "mfu_offset: %d", gst_buffer_get_size (gbuf), GST_BUFFER_OFFSET(gbuf), gst_buffer_get_size (mfu_buf), mfu_offset);
		}
		else
		{
			GST_DEBUG_OBJECT(qtdemux, "ELEMENTARY STREAM START: %x, size %d", (int)qtdemux->ref_offset, rem_size[mediaType]);
			gst_qtdemux_pull_atom (qtdemux, qtdemux->ref_offset, rem_size[mediaType], &mfu_buf);
		}
#if DUMP_VIDEO
			if(mediaType == 0 && is_muli)
			{
				gst_buffer_map (mfu_buf, &map, GST_MAP_READ);
				fwrite(map.data, 1, rem_size[mediaType], fp1);
				//fwrite("===", 1, 2, fp1);
				//fclose(fp1);
				gst_buffer_unmap (mfu_buf, &map);
			}
#endif
		}
	else
	{
		count[mediaType]++;
		if(qtdemux->mdatbuffer && (rem_size[mediaType]+size > sample_size[mediaType]))
		{
			GST_DEBUG_OBJECT(qtdemux," GGGSIZEE %d :: mfu_b.length: %d..................... count:%d", rem_size[mediaType]+size-14, sample_size[mediaType], count[mediaType]);
		}
		if(gbuf)
		{
#if 1
			mfu_buf = gst_buffer_copy_region (gbuf, GST_BUFFER_COPY_ALL, 14+22, size-14);
			//mfu_buf = gst_buffer_copy_region (gbuf, GST_BUFFER_COPY_MEMORY, 14+22, size-14);
			//mfu_buf = gst_buffer_copy_region (gbuf, GST_BUFFER_COPY_META | GST_BUFFER_COPY_FLAGS | GST_BUFFER_COPY_MEMORY | GST_BUFFER_COPY_TIMESTAMPS, 14+22, size-14);
			//mfu_buf = gst_buffer_copy_region (gbuf,GST_BUFFER_COPY_MEMORY | GST_BUFFER_COPY_TIMESTAMPS, 14+22, size-14);
		  //mfu_buf = gst_buffer_new_and_alloc(size-14);
		  //gst_buffer_copy_into (mfu_buf, gbuf, GST_BUFFER_COPY_MEMORY | GST_BUFFER_COPY_META, 14+22, size-14);
		   mfu_buf->pts = gbuf->pts;
		   mfu_buf->dts = gbuf->dts;
		   //mfu_buf->offset = gbuf->offset;
		   //mfu_buf->offset_end = gbuf->offset_end;
		   mfu_buf->duration = gbuf->duration;
	 GST_DEBUG_OBJECT (qtdemux,"FSYSYSYSYS mfu pts=%" GST_TIME_FORMAT, GST_TIME_ARGS(mfu_buf->pts));
#else
		  gst_adapter_flush(qtdemux->adapter, 14+22);
		  mfu_buf = gst_adapter_take_buffer (qtdemux->adapter, size-14);
		  gst_adapter_flush(qtdemux->adapter, size-14);
#endif
		   GST_DEBUG_OBJECT(NULL, "DDSIZEEEE %"G_GSIZE_FORMAT "&& mbuf %"G_GSIZE_FORMAT, gst_buffer_get_size (gbuf), gst_buffer_get_size (mfu_buf));
		}
		else
		{
			GST_DEBUG_OBJECT(qtdemux, "ELEMENTARY STREAM START: %x, size: %d", (int)qtdemux->ref_offset+14, size-14);
			gst_qtdemux_pull_atom (qtdemux, qtdemux->ref_offset+14, size-14, &mfu_buf);
		}
#if DUMP_VIDEO
		if(mediaType == 0 && rem_size[mediaType] )
		{
			gst_buffer_map (mfu_buf, &map, GST_MAP_READ);
			fwrite(map.data, 1, size-14, fp1);
			//fwrite("====", 1, 4, fp1);
			//fclose(fp1);
			gst_buffer_unmap (mfu_buf, &map);
		}
#endif
	}
	GST_DEBUG_OBJECT(qtdemux,"FILE_OFFSET  needdata %d file_offset:%x"/*G_GUINT64_FORMAT*/ , need_data[mediaType], is_muli?(int)qtdemux->ref_offset:(int)qtdemux->ref_offset+14);
	if(need_data[mediaType])
	{
		if(is_muli)
		{
			if(qtdemux->mdatbuffer)
			{
				gst_mmtdemux_state_movie(qtdemux, NULL, 0, mediaType, 0);
				//gst_buffer_unref (qtdemux->mdatbuffer);
				qtdemux->mdatbuffer = NULL;
			}
		}
		if(qtdemux->mdatbuffer == NULL)
			qtdemux->mdatbuffer = mfu_buf;
		else
			qtdemux->mdatbuffer = gst_buffer_append(qtdemux->mdatbuffer, mfu_buf);

		if(!is_muli)
			rem_size[mediaType] +=(size-14);	 

		GST_DEBUG_OBJECT(qtdemux," SIZEEEE %d :: mfu_b.length: %d count:%d", rem_size[mediaType], sample_size[mediaType], count[mediaType]);
		if(rem_size[mediaType] == sample_size[mediaType])
		{
			GST_DEBUG_OBJECT(qtdemux," UUUUSIZE %d :: mfu_b.length: %d..................... count:%d", rem_size[mediaType], sample_size[mediaType], count[mediaType]);
#if DUMP_VIDEO1
		  if(fp == NULL)
				fp = fopen("video.data", "a");
			if(fp != NULL)
			{
				gst_buffer_map (qtdemux->mdatbuffer, &map, GST_MAP_READ);
				fwrite(map.data, 1, rem_size[mediaType], fp);
				//fwrite("==========================", 1, 10, fp);
				gst_buffer_unmap (qtdemux->mdatbuffer, &map);
				//gst_buffer_unref (qtdemux->mdatbuffer);
			}
#endif
	 GST_DEBUG_OBJECT (qtdemux,"YSYSYSYSYS mdat pts=%" GST_TIME_FORMAT ", mfu pts=%" GST_TIME_FORMAT, GST_TIME_ARGS (qtdemux->mdatbuffer->pts), GST_TIME_ARGS(pts_ref[mediaType]));


		  //qtdemux->mdatbuffer->pts = mfu_buf->pts;
	 GST_DEBUG_OBJECT (qtdemux,"1YSYSYSYSYS mdat pts=%" GST_TIME_FORMAT ", dts mdat=%"GST_TIME_FORMAT", mfu pts=%" GST_TIME_FORMAT, GST_TIME_ARGS (qtdemux->mdatbuffer->pts), GST_TIME_ARGS (qtdemux->mdatbuffer->dts), GST_TIME_ARGS(pts_ref[mediaType]));
			gst_mmtdemux_state_movie(qtdemux, qtdemux->mdatbuffer, 0, mediaType, sample_number[mediaType]);
			need_data[mediaType] = 0;
			//gst_buffer_unref (qtdemux->mdatbuffer);
			qtdemux->mdatbuffer = NULL;
		}
	   else if(rem_size[mediaType] > sample_size[mediaType])
	   {
				GST_DEBUG_OBJECT(qtdemux," UNKNOWN PACKET..... UUUUSIZE %d :: mfu_b.length: %d..................... count:%d", rem_size[mediaType], sample_size[mediaType], count[mediaType]);
				gst_mmtdemux_state_movie(qtdemux, NULL, 0, mediaType, 0);
				//gst_buffer_unref (qtdemux->mdatbuffer);
				qtdemux->mdatbuffer = NULL;
	   }
	}
	else if (is_muli && (rem_size[mediaType] == mfu_b.length))
	{
#if DUMP_VIDEO1
		  if(fp == NULL)
				fp = fopen("video.data", "a");
			if((fp != NULL) && (mediaType == 0))
			{
				gst_buffer_map (mfu_buf, &map, GST_MAP_READ);
				fwrite(map.data, 1, mfu_b.offset, fp);
				gst_buffer_unmap (qtdemux->mdatbuffer, &map);
			}
#endif
	//	GST_DEBUG_OBJECT (qtdemux,"SAFWANSADA len: %d n_stream: %d", rem_size[mediaType], qtdemux->n_streams);
	 GST_DEBUG_OBJECT (qtdemux,"1YSYSYSYSYS mdat pts=%" GST_TIME_FORMAT ", dts mdat=%"GST_TIME_FORMAT", mfu pts=%" GST_TIME_FORMAT, GST_TIME_ARGS (mfu_buf->pts), GST_TIME_ARGS (mfu_buf->dts), GST_TIME_ARGS(pts_ref[mediaType]));
		gst_mmtdemux_state_movie(qtdemux, mfu_buf, (guint64 )mfu_b.offset, mediaType, mfu_b.samplenumber);
	}
	else
	{
		  GST_DEBUG_OBJECT (qtdemux,"KHADSAF no muli");
		  //gst_mmtdemux_state_movie(qtdemux, mfu_buf, size, mediaType);
	}
}


void parse_mfu(unsigned char *data, int length)
{
	unsigned char byte8;
	unsigned short byte16;
	unsigned int byte32;
	//mfu_box mfu_b;
	mfu_b.sequence_number = Read32(data);
	mfu_b.trackrefindex = Read8(data+4);
	mfu_b.movie_fragment_sequence_number = Read32(data+5);
	mfu_b.samplenumber = Read32(data+9);
	mfu_b.priority = Read8(data+13);
	mfu_b.dependency_counter = Read8(data+14);
	mfu_b.offset = Read32(data+15);
	mfu_b.length = Read32(data+19);

	GST_DEBUG_OBJECT (NULL," KKK sequence_number: %u, trackrefindex: %u, movie_fragment_sequence_number: %u, samplenumber: %u, priority: %u, dependency_counter: %u, offset: %u, length: %u\n\n", mfu_b.sequence_number, mfu_b.trackrefindex, mfu_b.movie_fragment_sequence_number, mfu_b.samplenumber, mfu_b.priority, mfu_b.dependency_counter, mfu_b.offset, mfu_b.length);

	//fprintf(fp,"mfu sequence_number: %d, ",mfu_b.movie_fragment_sequence_number);
	//fprintf(fp,"trackrefIndex: %d, ",mfu_b.trackrefindex);
	//fprintf(fp,"samplenumber: %d, ",mfu_b.samplenumber);
	//fprintf(fp,"offset: %d, ",mfu_b.offset);
	byte8 = Read8(data+23);
	mfu_b.mul_linfo.multilayer_flag = byte8 & 0x01;
	if(mfu_b.mul_linfo.multilayer_flag)
	{
	   byte32 = Read32(data+24);
		mfu_b.mul_linfo.dependency_id = byte32 & 0x07;
		mfu_b.mul_linfo.depth_flag = byte32 & 0x08;
		mfu_b.mul_linfo.temporal_id = byte32 & 0x0700;
		mfu_b.mul_linfo.quality_id = byte32 & 0xF000;
		byte16 = Read16(data+28);
		mfu_b.mul_linfo.priority_id =  byte16 & 0x3F;
		mfu_b.mul_linfo.view_id = byte16 & 0xFFC;
	}
   else
	{
		byte16 = Read16(data+24);
		mfu_b.mul_linfo.layer_id = byte16 & 0x3F;
		mfu_b.mul_linfo.temporal_id = byte16 & 0x1E0;
	}
}


int pullatom(unsigned char *data, char *buf)
{
    unsigned int length = 0;
	 int offset = 0;
    length = Read32(data);
    //if(!length || (length > 4096))
    if(!length && (length > 4096))
	 {
		  GST_DEBUG_OBJECT (NULL,"INVALIDDDDDDDDDDD length: %d\n", length);
        return 0;
	 }
    offset += 4;
    memcpy(buf, data+offset, 4);
    GST_DEBUG_OBJECT (NULL,"\t length: %d, box: %s\n", length, buf);
    return length;
}

int process_init_data(GstQTDemux * qtdemux, unsigned char *buf,int size)
{
   int length;
   static unsigned char Data[4096];
   static int tempsize/*, once*/;
   static int offset = 0;
   char boxType[4] = {"0"};
   int localOffset;
   GST_DEBUG_OBJECT (qtdemux, "HHHHHHHHHH %s size: %d \n", __func__, size);
#if 0
   if(offset == 0)
      Data=(unsigned char*)malloc(sizeof(unsigned char)*size);
   else
      Data=(unsigned char*)realloc(Data, sizeof(unsigned char)*size);
#endif
   memcpy(Data+offset,buf,size);
   size += tempsize;
   tempsize = size;
   localOffset=0;
   while(localOffset<size)
   {
      length = pullatom(Data+localOffset, boxType);
      if(length > (size-localOffset) )
         goto DataInSufficient;  // 0  for insufficient data
      else if(length <= 0)
         goto reset_buf;  // 0  for insufficient data
      if(!strncmp(boxType,FTYP,4))
      {
	        qtdemux_parse_ftyp (qtdemux, Data+localOffset, length);
      }
      else if(!strncmp(boxType,MMPU,4))
      {
		        qtdemux_parse_mmpu (qtdemux, Data+localOffset, length);

      }
      else if(!strncmp(boxType,MOOV,4))
      {
			  qtdemux_parse_moov (qtdemux, Data+localOffset, length);
			  qtdemux_parse_tree(qtdemux);
      }
      localOffset+=length;
   }
reset_buf:
   tempsize = 0;
   offset = 0;
   return 1;
DataInSufficient:
   GST_DEBUG_OBJECT(qtdemux," MOSS: Insufficient data required length: %d available: %d\n", length, size);
   if(length > 4096)
	{
		tempsize = 0;
		offset = 0;
	   return 0;
	}
   offset+=size;
   return 0;
}

int GainMMTdemux(GstQTDemux * qtdemux, unsigned char *buf,int size,int DataType, int mediaType, GstBuffer *gbuf)
{
	static short once, once_vid, once_moof_vid;
	GST_DEBUG_OBJECT (qtdemux, "FCHECK %s size: %d DataType: %d, mediaType:%s total stream:%d\n", __func__, size, DataType, mediaType==1?"audio":"video", qtdemux->n_streams);
	if(!once_vid && !mediaType && !DataType)
	{
			once_vid = 1;
	}
   if(!once_vid)
   {
		 return -1;
   }
/*
   if(qtdemux->sample_reset)
	{
		once = 0;
		qtdemux->sample_reset = 0;
	}*/

	switch(DataType)
	{
		case 00:
			{
				if(!qtdemux->got_moov)
				process_init_data(qtdemux, buf, size);
				break;
			}
		case 01:
			{
				#if 1
				if(!once_moof_vid && !mediaType)
					 once_moof_vid = !once_moof_vid;
		      if(once_moof_vid)
					 qtdemux_parse_moof (qtdemux, buf, size, /*qtdemux->moof_offset*/0, NULL);
				//if(!once && once_vid && (mediaType == 1))
				if(!once && (mediaType == 1) && (qtdemux->n_streams == 2))
				{
				   if(qtdemux->exposed == FALSE)
					{
						qtdemux_prepare_streams (qtdemux);
						qtdemux_expose_streams (qtdemux);
					}
					once = !once;
				}
				#else
				qtdemux_parse_moof (qtdemux, buf, size, qtdemux->moof_offset, NULL);
				#endif
				break;
			}
		case 02:
			{
				if(once)
					 process_mfus(qtdemux, buf, size, mediaType, gbuf);
				break;
			}
	}
	return 1;
}
