//this stuff needs work but i really dont want to do it now.
//really, its for flexibility.
//and what i really need right now is GO-POWER

////the InfiniQueue will increase (and never reduce) its buffer size
////if you are a productionmodule, and need to generate samples
////for a sink, send the samples for the other sinks into an
////InfiniQueue and it will take care of buffering them
////no matter how much buffering needs to be done
////hopefully it will be small amounts..
//class OAKRA_InfiniQueue : public OAKRA_BasicModule {
//public:
//	short *queueBuf;
//	int sampleSize;
//	int queueSize = 0;
//	int queueHead, queueTail;
//	OAKRA_InfiniQueue(int sampleSize) { queueSize = 0; queueBuf = 0; this->sampleSize = sampleSize; }
//
//	int getQueueChunk() {
//		if(queueLen == 0) return 0;
//
//	}
//
//	void generate(int samples, void *buf, OAKRA_Module *who) {
//		int chunk = getQueueChunk();
//		if(chunk == 0) { source->generate(samples,buf,this); return; }
//		/*else {
//			int amt = queueSize - queuePos;
//			if(samples < amt) amt = samples;
//			memcpy(buf,queueBuf,sampleSize*amt);
//			queuePos += amt;
//			if(queuePos==0)
//		}*/
//	}
//	void receive(int samples, void *buf) {
//		if(queueSize == 0) {
//			queueSize = samples;
//			queueBuf = (short *)malloc(samples*sampleSize);
//		}
//	}
//};
//
//class OAKRA_ProductionModule : public OAKRA_Module {
//public:
//	OAKRA_Module *source;
//	OAKRA_Module **sinks;
//	OAKRA_InfiniQueue **queues;
//	int sinkCount;
//	OAKRA_ReductionModule() { source = 0; sinks = 0; sinkCount = 0; }
//	virtual void setSinkCount(int count) { 
//		if(sinks) { 
//			for(int i=0;i<sinkCount;i++)
//				if(queues[i])
//					delete queues[i];
//			delete[] sinks; 
//			delete[] queues;
//		}
//		sinkCount = count;
//		sinks = new OAKRA_Module *[count];
//		queues = new OAKRA_InfiniQueue *[count];
//		for(int i=0;i<sinkCount;i++)
//			sinks[i] = 0;
//	}
//	void setSink(int index, OAKRA_Module *sink) {
//		sinks[index] = sink;
//		if(queues[index]) delete queues[index];
//		queues[index] = new OAKRA_InfiniQueue();
//	}
//}
//
//
//class OAKRA_Module_2S16_to_1S16 : public OAKRA_ProductionModule {
//public:
//	//todo apply the bit-reversal thinking we wasted so many hours on
//	//to maybe do this in place
//	short stereobuf[OAKRA_MAX_SAMPLES*2];
//	short tempbuf[OAKRA_MAX_SAMPLES];
//	OAKRA_Module_2S16_to_1S16() { setSinkCount(2); }
//	void generate(int samples, void *buf, OAKRA_Module *who) {
//		source->generate(samples,stereobuf,this);
//		for(int i=0;i<samples;i++) {
//			tempbuf[i] = stereobuf[i*2];
//			stereobuf[i] = stereobuf[i*2+1];
//		}
//		queues[0]->receive(samples,tempbuf);
//		queues[1]->receive(samples,stereobuf);
//	}
//};