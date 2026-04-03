#include "pipe.h"
#include "debug.h"

using namespace std;

Descriptor::Descriptor(char* desType,char* permission,char* buffPtr){
    buff = buffPtr;
    this->desType = desType;
    this->persimission = permission;
    this->currSeekPos = 0;
}

Descriptor::~Descriptor(){
    delete[] buff;
}

PipeDescriptors::PipeDescriptors(){
    for(int i=0;i<MAX_NUM_DESCRIPTORS;i++){
        descriptors[i] = NULL;
    }
}

int PipeDescriptors::createDes(int* x,int* y,char* desType){
    char* buf = new char[BUFFER_SIZE];
    int parentPd = -1,childPd = -1;
    int freeDes_cnt = 0;

    for(int i=0;i<MAX_NUM_DESCRIPTORS;i++){
        if(descriptors[i] == NULL) freeDes_cnt++;
    }

    if(freeDes_cnt < 2) return -1;

    Descriptor* parentDes = new Descriptor(desType,"w",buf);
    Descriptor* childDes = new Descriptor(desType,"r",buf);

    for(int i=0;i<MAX_NUM_DESCRIPTORS;i++){
        if(descriptors[i] == NULL){ parentPd = i; break; }
    }

    for(int i=0;i<MAX_NUM_DESCRIPTORS;i++){
        if(descriptors[i] == NULL && i != parentPd){ childPd = i; break; }
    }

    descriptors[parentPd] = parentDes;
    descriptors[childPd] = childDes;

    *x = parentPd;
    *y = childPd;

    return 1;
}

int PipeDescriptors::readDes(int des, char* buf, int numBytes) {
    if (des < 0 || des >= MAX_NUM_DESCRIPTORS) return -1;
    int numRead = 0;
    Descriptor* pDes = descriptors[des];
    char* pipeBuf = pDes->buff;

    for (int i = 0; i < numBytes; i++) {
        buf[i] = pipeBuf[i];
        numRead++;
    }
    return numRead;
}

int PipeDescriptors::writeDes(int id,char* buff,int size){
    if(id < 0 || id>=MAX_NUM_DESCRIPTORS) return -1;
    Descriptor* pDes = descriptors[id];

    int numWrite = 0;
    for(int i=0;i<size;i++){
        pDes->buff[i+pDes->currSeekPos] = buff[i];
        numWrite++;
    }
    pDes->currSeekPos += numWrite;
    return numWrite;
}

void PipeDescriptors::test_method(){
    int des1,des2;
    char* buf = new char[BUFFER_SIZE];
    createDes(&des1,&des2,"pipe");
    writeDes(des1,"Hello world",11);
    readDes(des2,buf,11);
    delete[] buf;
}