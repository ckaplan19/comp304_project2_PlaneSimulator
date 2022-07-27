#include "queue.c"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>


#define LAUNCH_TIME 4
#define LAND_TIME 2
#define ASSEMBLY_TIME 12


int maxWaitTime=10; 
int simulationTime = 120;    // simulation time
int seed = 10;               // seed for randomness
int emergencyFrequency = 40; // frequency of emergency
float p = 0.2;               // probability of a ground job (launch & assembly)
int n = 30;                  // set to 30 seconds to start printing queues
double randomP = 0.0;         // initialises the random number
time_t simFinishTime ;
time_t simStartTime = 0;
void* job_landing(void *arg);
void* job_launching(void *arg);
void* job_emergency(void *arg);
void* job_assembly(void *arg);
void* ControlTower(void *arg);
void* PadA(void *arg);
void* PadB(void *arg);
void openLog();
void* printAllQueues();
void* printQueue(Queue *p);


// mutexes for each queue are initialized here.
pthread_mutex_t mutex_landing;
pthread_mutex_t mutex_launching;
pthread_mutex_t mutex_assembly;
pthread_mutex_t mutex_padA;
pthread_mutex_t mutex_padB;
pthread_mutex_t mutex_emergency;
pthread_mutex_t mutex_log;

// all queues are initialized here
Queue  *queue_land;
Queue  *queue_launch ;
Queue  *queue_assembly ;
Queue  *queue_padA ;
Queue  *queue_padB ;
 Queue  *queue_emergency;

// pthread sleeper function
int pthread_sleep (int seconds)
{
    pthread_mutex_t mutex;
    pthread_cond_t conditionvar;
    struct timespec timetoexpire;
    if(pthread_mutex_init(&mutex,NULL))
    {
        return -1;
    }
    if(pthread_cond_init(&conditionvar,NULL))
    {
        return -1;
    }
    struct timeval tp;
    //When to expire is an absolute time, so get the current time and add it to our delay time
    gettimeofday(&tp, NULL);
    timetoexpire.tv_sec = tp.tv_sec + seconds; timetoexpire.tv_nsec = tp.tv_usec * 1000;

    pthread_mutex_lock (&mutex);
    int res =  pthread_cond_timedwait(&conditionvar, &mutex, &timetoexpire);
    pthread_mutex_unlock (&mutex);
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&conditionvar);

    //Upon successful completion, a value of zero shall be returned
    return res;
}

int main(int argc,char **argv){
    // -p (float) => sets p
    // -t (int) => simulation time in seconds
    // -s (int) => change the random seed
    for(int i=1; i<argc; i++){
        if(!strcmp(argv[i], "-p")) {p = atof(argv[++i]);}
        else if(!strcmp(argv[i], "-t")) {simulationTime = atoi(argv[++i]);}
        else if(!strcmp(argv[i], "-s"))  {seed = atoi(argv[++i]);}
        else if(!strcmp(argv[i], "-n")) {n = atoi(argv[++i]);}
    }

    srand(seed); // feed the seed

    /* Queue usage example
        Queue *myQ = ConstructQueue(1000);
        Job j;
        j.ID = myID;
        j.type = 2;
        Enqueue(myQ, j);
        Job ret = Dequeue(myQ);
        DestructQueue(myQ);
    */
    simStartTime = time(NULL); // Start time of the simulation.
    simFinishTime = time(NULL) + simulationTime;  //time(null ) gives number of seconds passed since 1970

    // Initialisation of the queues.
    queue_land = ConstructQueue(1000);
    queue_launch = ConstructQueue(1000);
    queue_assembly = ConstructQueue(1000);
    queue_padA = ConstructQueue(1000);
    queue_padB = ConstructQueue(1000);
    queue_emergency = ConstructQueue(1000);
    openLog();

    Job first;
    first.ID = 1;
    first.type = 2;
    first.jobDuration = LAUNCH_TIME;
    first.initTime = time(NULL) - simStartTime;
    first.status = 'D';
    Enqueue(queue_launch, first);

    // Initialisation of the threads.
    pthread_t thread_land;
    pthread_t thread_launch;
    pthread_t thread_assembly;
    pthread_t thread_controlTower;
    pthread_t thread_padA;
    pthread_t thread_padB;
    pthread_t thread_printAllQueues;
     pthread_t thread_emergency;

    // Mutex initialisation.
    pthread_mutex_init(&mutex_padA, NULL);
    pthread_mutex_init(&mutex_padB, NULL);
    pthread_mutex_init(&mutex_landing, NULL);
    pthread_mutex_init(&mutex_launching, NULL);
    pthread_mutex_init(&mutex_assembly, NULL);
    pthread_mutex_init(&mutex_emergency, NULL);
    pthread_mutex_init(&mutex_log, NULL);
-
    // Creation of the threads.
    pthread_create(&thread_land, NULL, job_landing, NULL);
    pthread_create(&thread_launch, NULL, job_launching, NULL);
    pthread_create(&thread_assembly, NULL, job_assembly, NULL);
    pthread_create(&thread_controlTower, NULL, ControlTower, NULL);
    pthread_create(&thread_padA, NULL, PadA, NULL);
     pthread_create(&thread_emergency, NULL, job_emergency, NULL);
    pthread_create(&thread_padB, NULL, PadB, NULL);
    pthread_create(&thread_printAllQueues, NULL, printAllQueues, NULL);

    // Joining the threads.
    pthread_join(thread_land, NULL);
    pthread_join(thread_launch, NULL);
    pthread_join(thread_assembly, NULL);
    pthread_join(thread_controlTower, NULL);
    pthread_join(thread_padA, NULL);
    pthread_join(thread_padB, NULL);
    pthread_join(thread_printAllQueues, NULL);
     pthread_join(thread_emergency, NULL);


    // Destroying the queues.
    DestructQueue(queue_land);
    DestructQueue(queue_launch);
    DestructQueue(queue_assembly);
    DestructQueue(queue_padA);
    DestructQueue(queue_padB);
     DestructQueue(queue_emergency);

    return 0;
}

// the function that creates plane threads for landing kontrol et gerçek dosyadaki ismini
void* job_landing(void *arg){
    while(time(NULL) < simFinishTime){
        // sleep for 2 seconds
        pthread_sleep(2);
        randomP = (double)rand() / (double)RAND_MAX;
        // We implement the rocket becomes ready to land part with P(Land) = (1- p).
        if(randomP <= 1-p){
            Job j;
            j.ID = rand()%1000;
            j.type = 1; //landing type is 1
            j.jobDuration = LAND_TIME;
            j.initTime = time(NULL) - simStartTime;
            j.status = 'L';
            // Lock queue_land to add job J to the queue.
            pthread_mutex_lock(&mutex_landing);
            
            Enqueue(queue_land, j);
            // Unlock the queue_land after the enqueue process.
            pthread_mutex_unlock(&mutex_landing);
        }
        }
        return NULL;
}

// the function that creates plane threads for departure
void* job_launching(void *arg){
    while(time(NULL) < simFinishTime){
        // sleep for 2 seconds
        pthread_sleep(2);
        randomP = (double)rand() / (double)RAND_MAX;
        // We implement the rocket becomes ready to be launched part with P(Launch) = (p/2).
        if(randomP <= p/2){
            Job j;
            j.ID = rand()%1000;
            j.type = 2; //launching type is 2
            j.jobDuration = LAUNCH_TIME;
            j.initTime = time(NULL) - simStartTime;
            j.status = 'D';
            // Lock queue_launch to add job J to the queue.
            pthread_mutex_lock(&mutex_launching);
            
            Enqueue(queue_launch, j);
            // Unlock the queue_launch after the enqueue process.
            pthread_mutex_unlock(&mutex_launching);
        }
    }
    return NULL;
}

// the function that creates plane threads for emergency landing

void* job_emergency(void *arg){
while(time(NULL) < simFinishTime){
        // sleep for 40t time which is 80 seconds
        pthread_sleep(80); 
        // We implement the emeregency planes that want to land on both pads periodically (40t).
            Job j;
            j.ID = rand()%1000;
            j.type = 4; //emergency type is 4
            j.jobDuration = 2;
            j.status='E';
            j.initTime = time(NULL) - simStartTime;
            // Lock queue_emergency to add job J to the queue.
            pthread_mutex_lock(&mutex_emergency);
            
            Enqueue(queue_emergency, j);
            Enqueue(queue_emergency, j);
            // Unlock the queue_emergency after the enqueue process.
            pthread_mutex_unlock(&mutex_emergency);

    }
    return NULL;
}


// the function that creates plane threads for assembly
void* job_assembly(void *arg){
    while(time(NULL) < simFinishTime){
        // sleep for 2 seconds
        pthread_sleep(2);
        randomP = (double)rand() / (double)RAND_MAX;
        // We implement the rocket becomes ready for assemble part with P(Assembly) = (p/2).
        if(randomP <= p/2){
            Job j;
            j.ID = rand()%1000;
            j.type = 3;  //assembly type is 3.
            j.jobDuration = ASSEMBLY_TIME;
            j.initTime = time(NULL) - simStartTime;
            j.status = 'A';
            // Lock queue_assembly to add job J to the queue.
            pthread_mutex_lock(&mutex_assembly);
            
            
            Enqueue(queue_assembly, j);
            // Unlock the queue_assembly after the enqueue process.
            pthread_mutex_unlock(&mutex_assembly);
        }
    }
    return NULL;
}

// the function that controls the air traffic
void* ControlTower(void *arg){
    while(time(NULL)< simFinishTime){
      

if(queue_emergency->size > 0) {  // PART 3 checks if there are any emergency jobs in the emergency queue. If there are any, addEmergencyJobNew function puts emergency job at the head of the padA and padB queues by moving every element by one in the queue immediately for emergency landing.
  pthread_mutex_lock(&mutex_emergency);
  pthread_mutex_lock(&mutex_padA);
  pthread_mutex_lock(&mutex_padB);
  addEmergencyJobNew(queue_padA, Dequeue(queue_emergency));
  addEmergencyJobNew(queue_padB, Dequeue(queue_emergency));
  pthread_mutex_unlock(&mutex_padA);
  pthread_mutex_unlock(&mutex_padB);
  pthread_mutex_unlock(&mutex_emergency);
}

if((queue_assembly->size>=3) ||(queue_launch->size>=3)) {  // PART 2  checks if there are more then 3 jobs in assembly or launch queues. if there are we need to consume them first before as we donot want landing to be favored in this case.


  if((queue_assembly->size >= 3) && (queue_launch->size < 3)) { // the condition where there are more than or equal to 3 asssembly jobs waiting but not launching jobs.
//printf(" THERE ARE EQUAL TO OR MORE THAN 3 ASSEMBLY  NOW AND THE NUMBER IS  %d \n" , queue_assembly->size ); for debug
  while(queue_assembly->size >= 3 ){
    pthread_mutex_lock(&mutex_padB);
    pthread_mutex_lock(&mutex_assembly);
  
        Enqueue(queue_padB,Dequeue(queue_assembly));
  
    // Unlock mutex_padB and mutex_assembly after enqueue and dequeue process.
    pthread_mutex_unlock(&mutex_padB);
    pthread_mutex_unlock(&mutex_assembly);
  }
  }
if((queue_assembly->size < 3) && (queue_launch->size >= 3)){
  printf(" THERE ARE EQUAL TO OR MORE THAN 3 LAUNCH  NOW AND THE NUMBER IS  %d \n" , queue_launch->size );
  while(queue_launch->size >= 3 ) {// the condition where there are more than or equal to 3 launching and assembly jobs
  pthread_mutex_lock(&mutex_padA);
  pthread_mutex_lock(&mutex_launching);
  
      Enqueue(queue_padA,Dequeue(queue_launch));
 
  // Unlock mutex_padA and mutex_launching after enqueue and dequeue process.
  pthread_mutex_unlock(&mutex_padA);
  pthread_mutex_unlock(&mutex_launching);

}
}
if((queue_assembly->size >= 3 ) && (queue_launch->size >= 3 )){
  printf(" THERE ARE EQUAL TO OR MORE THAN 3 LAUNCH AND ASSEMBLY   NOW AND THE NUMBERS ARE %d  %d \n",queue_launch->size,queue_assembly->size);
  while(queue_assembly->size >=3 ){
    pthread_mutex_lock(&mutex_padB);
    pthread_mutex_lock(&mutex_assembly);
  
        Enqueue(queue_padB,Dequeue(queue_assembly));
  
    // Unlock mutex_padB and mutex_assembly after enqueue and dequeue process.
    pthread_mutex_unlock(&mutex_padB);
    pthread_mutex_unlock(&mutex_assembly);
  }
  while(queue_launch->size >=3 ) {// the condition where there are more than or equal to 3 launching and assembly jobs
  pthread_mutex_lock(&mutex_padA);
  pthread_mutex_lock(&mutex_launching);
//  if(isEmpty(queue_padA) && !isEmpty(queue_launch)){
      Enqueue(queue_padA,Dequeue(queue_launch));
  //}
  // Unlock mutex_padA and mutex_launching after enqueue and dequeue process.
  pthread_mutex_unlock(&mutex_padA);
  pthread_mutex_unlock(&mutex_launching);

}



}
}
else{// if there are less than 3 jobs in launch and assembly queues landing is fovored PART1
        // Lock mutex_landing for checking if there is a item waiting in queue_land which is prioritised.
        pthread_mutex_lock(&mutex_landing);
        while(!isEmpty(queue_land) && queue_assembly->size < 3 && queue_launch->size < 3 ){
            // Lock both mutex_padA and mutex_padB for checking which pad has shorter queue duration in order to consume queue_land.
            pthread_mutex_lock(&mutex_padA);
            pthread_mutex_lock(&mutex_padB);
            if(queue_padA->queueDuration <= queue_padB->queueDuration){
                Enqueue(queue_padA, Dequeue(queue_land));
            }
            else{
                Enqueue(queue_padB, Dequeue(queue_land));
            }
            // Unlock mutex_padA and mutex_padB after the enqueue and dequeue process.
            pthread_mutex_unlock(&mutex_padA);
            pthread_mutex_unlock(&mutex_padB);
        }
        // Unlock mutex_landing after the enqueue and dequeue process.
        pthread_mutex_unlock(&mutex_landing);

        // Lock mutex_padA and mutex_launching for consuming the queue_launch in padA.
        pthread_mutex_lock(&mutex_padA);
        pthread_mutex_lock(&mutex_launching);
        if( !isEmpty(queue_launch) &&  isEmpty(queue_padA) ){
            Enqueue(queue_padA,Dequeue(queue_launch));
        }
        // Unlock mutex_padA and mutex_launching after enqueue and dequeue process.
        pthread_mutex_unlock(&mutex_padA);
        pthread_mutex_unlock(&mutex_launching);

        // Lock mutex_padB and mutex_assembly for consuming the queue_assembly in padB.
        pthread_mutex_lock(&mutex_padB);
        pthread_mutex_lock(&mutex_assembly);
        if(!isEmpty(queue_assembly) &&  isEmpty(queue_padB)){
            Enqueue(queue_padB, Dequeue(queue_assembly));
        }
        // Unlock mutex_padB and mutex_assembly after enqueue and dequeue process.
        pthread_mutex_unlock(&mutex_padB);
        pthread_mutex_unlock(&mutex_assembly);

        }



  }
  if(simFinishTime-time(NULL)<1){
printf("  ASSEMBLY  NOW AND THE NUMBER IS  %d \n" , queue_assembly->size );
printf("  LAUNCH  NOW AND THE NUMBER IS  %d \n" , queue_launch->size );
printf("  LAND  NOW AND THE NUMBER IS  %d \n" , queue_land->size );
}
}

    void* PadA(void *arg){    // padA cosumes what is put in to the padA_queue (consumer)
        while (time(NULL) < simFinishTime) {
        pthread_mutex_lock(&mutex_padA);
        if(isEmpty(queue_padA)){
            pthread_mutex_unlock(&mutex_padA);
            pthread_sleep(2);
        }
        else{
            int sleepDuration = queue_padA->head->data.jobDuration;
            pthread_mutex_unlock(&mutex_padA);
            pthread_sleep(sleepDuration);
            pthread_mutex_lock(&mutex_padA);
            Job job = Dequeue(queue_padA); // critical section
            pthread_mutex_unlock(&mutex_padA);

          
            pthread_mutex_lock(&mutex_log);
            FILE *fp = fopen("rocket_Log.txt","a");  //we write to the rocket_log file here and below after we lock mutex_log
fprintf(fp,"%d  %c %d  %ld  %ld  %c \n", job.ID, job.status, job.initTime, time(NULL)- simStartTime ,   time(NULL)- simStartTime- job.initTime , 'A');              fclose(fp);
              pthread_mutex_unlock(&mutex_log);

        }
    }
    return NULL;
}
void* PadB(void *arg){    // padB cosumes what is put in to the padA_queue (consumer)
		while (time(NULL) < simFinishTime) {
      pthread_mutex_lock(&mutex_padB);
      if(isEmpty(queue_padB)){
        pthread_mutex_unlock(&mutex_padB);
        pthread_sleep(2);
      }
      else{
        int sleepDuration = queue_padB->head->data.jobDuration;
        pthread_mutex_unlock(&mutex_padB);
        pthread_sleep(sleepDuration);
        pthread_mutex_lock(&mutex_padB);
        Job job = Dequeue(queue_padB); // critical section
        pthread_mutex_unlock(&mutex_padB);

       
        pthread_mutex_lock(&mutex_log);
        FILE *fp = fopen("rocket_Log.txt","a");
fprintf(fp,"%d  %c %d  %ld  %ld  %c \n", job.ID, job.status, job.initTime, time(NULL)- simStartTime ,   time(NULL)- simStartTime- job.initTime , 'B');          fclose(fp);
          pthread_mutex_unlock(&mutex_log);
      }
    }
    return NULL;
}

void openLog() {   //method to create rocket.log file
  FILE *fp = fopen("rocket_Log.txt", "wb");
  fprintf(fp, "EventId  Status  RequestTime EndTime TurnaroundTime  Pad \n");
  fclose(fp);
}

void* printAllQueues(){  //function to print all queues with mutex lock and unlocks.
    int simCurrentTime;
    while(time(NULL) < simFinishTime){
        pthread_sleep(1);
        simCurrentTime = time(NULL) - simStartTime;
        if(simCurrentTime >= n){
            pthread_mutex_lock(&mutex_landing);
            printf("At %d seconds, Landıng  :", simCurrentTime);
            printQueue(queue_land);
            pthread_mutex_unlock(&mutex_landing);

            pthread_mutex_lock(&mutex_launching);
            printf("At %d seconds, Launch   :", simCurrentTime);
            printQueue(queue_launch);
            pthread_mutex_unlock(&mutex_launching);

            pthread_mutex_lock(&mutex_assembly);
            printf("At %d seconds, Assembly :", simCurrentTime);
            printQueue(queue_assembly);
            pthread_mutex_unlock(&mutex_assembly);
        }
    }
}

void* printQueue(Queue *p){  //function to print queue 
    if (isEmpty(p)){
        printf("empty\n");
    }
    else{
        NODE *temp = p->head;
        while (temp != NULL)
        {
            printf("%d, ", temp->data.ID);
            temp = temp->prev;
        }
        printf("\n");
    }
}