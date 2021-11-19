#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
     
    return 0.0; 
}

//proc/stat

// numbers after cpu tag = clock ticks -> add those up and devide by total time that passed 
// we do not need individual cpu utilization (cpu0, cpu1, ...)
// more info on that in "11. Processor Data" 
