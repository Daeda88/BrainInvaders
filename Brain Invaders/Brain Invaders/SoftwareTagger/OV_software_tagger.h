#ifndef ov_software_tagger_h
#define ov_software_tagger_h

#include <sys/timeb.h>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/thread/thread.hpp>

using namespace boost::interprocess;

//OpenVibe stimulations
//Row stimulation base
#define OVTK_StimulationId_Label_00                          0x00008100
//add +1 for each row
//row 1 = OVTK_StimulationId_Label_00 + 1

//Column stimulation base
#define OVTK_StimulationId_Label_07                          0x00008107
//add +1 for each column
//column 1 = OVTK_StimulationId_Label_07 + 1

typedef unsigned int uint32;
typedef unsigned long long uint64;

const char* mq_name="clinet_to_ov";
const int chunk_length=3; //must be the same in client and server
const int max_message=5000;

uint64 chunk[chunk_length];

message_queue* mq;

void ST_Clear()
{
	 message_queue::remove(mq_name);
}

void ST_Init()
{
	message_queue::remove(mq_name);

	try
	{
		mq=new message_queue
			 (create_only             //only create
			 ,mq_name                 //name
			 ,max_message             //max message number
			 ,sizeof(chunk)           //max message size 
			 );
	}
	catch(interprocess_exception &ex)
	 {
        std::cout << ex.what() << std::endl;
      //return 1;
	 }
}

//Send Flash
void ST_TagNow(uint64 stimulation_identifier)
{
     struct timeb tmb;
 
     ftime(&tmb);

     printf("tmb.time     = %ld (seconds) :", tmb.time);
     printf("tmb.millitm  = %d (mlliseconds)\n", tmb.millitm);

	 uint64 flash_time  = tmb.time * 1000 + tmb.millitm;

	 chunk[0] = 1;//random value currently
	 chunk[1] = stimulation_identifier;
	 chunk[2] = flash_time;

	 try
	 {
        mq->send(&chunk, sizeof(chunk), 0);
	 }
	 catch(interprocess_exception &ex)
	 {
        std::cout << ex.what() << std::endl;
      //return 1;
	 }
}

#endif 
