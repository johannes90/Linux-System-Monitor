#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
 // initialize prev values with 0
    float prevTotal = 0;
    float prevIdle  = 0;
};

#endif