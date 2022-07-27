PART1 IMPLEMENTATION

First of all we instantiated all queues, mutexes, threads in main.

We used the template which is given to us and implemented pad, consumer and control tower functions for threads. Both consumers, producers and also cotrol tower checks the time and if time is less then sim time they keeps working.

 For producers, they first sleep then checks the random probability condition. For example  80 % of the time landing job  producer function produces a job and fills its struct fields which are given in the queue.c file. Then it uses mutex locks and puts this job to the landing job queue. Other producers have the same way of functioning. 
 
 For consumers, they first check if there are any jobs in the PAD queues. If there are any jobs in the Pad queues they get the job duration of the job in the pad queue and sleep which means the job is being done. Then Pad function removes the job from the pad queue and writes information about the job to the log file. 


For control tower, we have landing favored condition. So we check if landing queue is empty first. If it is not we need to handle landing jobs. We check which pad has less jobs and put landing job from landing queue to the Pad queue with less jobs.  After landing jobs are handled, we check if launching jobs are waiting in the queue and if there are not any landing being done in pad A. If both conditions hold we make a launching job happen. we use  Enqueue(queue_padA,Dequeue(queue_launch)); function to put the jobs to the pad queues from the job queues. Same happens for assembly with the difference of padB instead of padA. Also we use mutex locks and unlocks to prevent race conditions between threads. 


PART2 IMPLEMENTATION
For this part we wanted to check if there more than or equal to 3 jobs in the assembly and launching jobs. If there are not +3 jobs in any of these queues  program works like in part 1 with landing is favored. However, if there are more than 3 jobs in any of the queues the pogram enters  the if statement and executes inside if. We check 3 conditions. first +3 launching but -3 assembly, second -3 launcing and +3 assembly, third both launching and assembly +3 in the corresponding queues. We put these jobs to the corresponding PAD queues to make them happen. 
This way we prevent starvation of assembly and launching jobs because whenever there are 3+ jobs in the assembly or launching queues we put the oldest job of these to the PAD queues to be consumed. 



STARVATION PROBLEM FOR PART2
If we increase the probaability of assembly jobs to happen we might observe starvation. Because, whenever there are more than 3 assembly jobs in the queue we want our rogram to handle it. Since, the time it takes for assembly job to take place s 12 seconds. Program will always be busy with assembly jobs and landing jobs might starve.

In order to solve this issue, we can add a condition to the landing statement. We can handle landing if there are more than 5 in the queue. This might be a useful apporach. Another approach might include increasing pririty of th threads that are older than other. We saw this method with processes in the class. Increasing the priority with aging of threads might be another useful approach to solve this starvation issue.  


   




PART3 IMPLEMENTATION

To implement part 3 we added a addEmergencyJobNew function to the queue.c This function uses a temp queue and puts the emergency job at the head of the PADA and PAD b queues. Also we impelemented job emergency in the project_2.c file to produce 2 emergency jobs at the 80th second of the program. In the control tower we check if there are more than 0 elements in the emergency queue . If there are we immediately put these jobs to the padA and padB queues and make emergency landing happen. 
