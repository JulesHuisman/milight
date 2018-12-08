/*
  Library for queueing commands for the milight hub
  Created by Jules Huisman, November 26, 2018.
*/

#ifndef Queue_h
#define Queue_h

#define QUEUE_SIZE 50

class Queue
{
  public:
    inline Queue() { };
    void add(uint8_t command[]);
    void set(uint8_t index, uint8_t command[]);
    bool isEmpty();
    void increaseIndex();
    uint8_t* get();
    void print();
    uint8_t activeIndex = 0;

  private:
      uint8_t queue[QUEUE_SIZE][22] = { {0} };
};

#endif
