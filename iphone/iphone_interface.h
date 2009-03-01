#ifndef _IPHONE_INTERFACE_H_
#define _IPHONE_INTERFACE_H_

#define MAX_SIMULTANEOUS_TOUCHES 5

extern struct iphone_Mouse {
  void *data;
  int l,x,y;
} iphone_mouses[MAX_SIMULTANEOUS_TOUCHES];

#endif //_IPHONE_INTERFACE_H_

