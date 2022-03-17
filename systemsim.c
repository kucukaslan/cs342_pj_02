#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>

#include "shared.c"

// declare arguments
enum algorithm alg;
int Q;
int T1;
int T2;
enum burst_distribution burst_dist;
int burstlen;
int min_burst;
int max_burst;
double p0;
double p1;
double p2;
double pg;
int MAXP;
int ALLP;
int outmode;

// declare global variables
struct pcb_queue ready_queue = {NULL, NULL, 0};
int toBeRun_pid = -1;

pthread_mutex_t pcb_queue_mutex;
pthread_cond_t cpu_cond_var;

// declare methods!
void *process(void *arg);
void *p_sched(void *arg);
int pcb_queue_dequeue();
void mutex_queue_add(struct PCB pcb);
int mutex_queue_rm();
void pcb_queue_insert(struct PCB pcb);

int main(int argc, char **argv)
{
    // READ THE arguments
    if (argc != 16)
    {
        printf("Usage: %s <message queue name> arg. count %d\n", argv[0], argc);
        exit(1);
    }
    // <ALG> <Q> <T1> <T2> <burst-dist> <burstlen> <min-burst> <max-burst> <p0> <p1> <p2> <pg> <MAXP> <ALLP> <OUTMODE>
    char *tmp_alg = argv[1];

    if (strcmp(tmp_alg, "FCFS") == 0)
    {
        alg = FCFS;
    }
    else if (strcmp(tmp_alg, "SJF") == 0)
    {
        alg = SJF;
    }
    else if (strcmp(tmp_alg, "RR") == 0)
    {
        alg = RR;
    }
    else
    {
        printf("Invalid algorithm %s\n", tmp_alg);
        exit(1);
    }

    printf("Algorithm: %s\n", tmp_alg);

    Q = -1;
    if (alg == RR)
    {
        Q = atoi(argv[2]);
    }

    T1 = atoi(argv[3]);
    T2 = atoi(argv[4]);

    char *tmp_burst_dist = argv[5];
    if (strcmp(tmp_burst_dist, "uniform") == 0)
    {
        burst_dist = UNI;
    }
    else if (strcmp(tmp_burst_dist, "exponential") == 0)
    {
        burst_dist = EXP;
    }
    else if (strcmp(tmp_burst_dist, "fixed") == 0)
    {
        burst_dist = FIX;
    }
    else
    {
        printf("Invalid burst distribution %s\n", tmp_burst_dist);
        exit(1);
    }

    burstlen = atoi(argv[6]);
    min_burst = atoi(argv[7]);
    max_burst = atoi(argv[8]);
    p0 = atof(argv[9]);
    p1 = atof(argv[10]);
    p2 = atof(argv[11]);
    pg = atof(argv[12]);
    MAXP = atoi(argv[13]);
    ALLP = atoi(argv[14]);
    outmode = atoi(argv[15]);

    // todo implement constraints
    // check T1 more than 30 less than 100
    if (T1 < 30 || T1 > 100)
    {
        printf("T1 must be than 30 less than 100\n");
        exit(1);
    }
    // check T2 at least 100 and at most 300
    if (T2 < 100 || T2 > 300)
    {
        printf("T2 must be at least 100 and at most 300\n");
        exit(1);
    }

} // END main function

struct pgen_arg
{
};

void *p_gen(void *arg)
{
    // create ready queue
    int maxp_count = 0;
    int allp_count = 0;
    for (int i = 0; i < 10; i++)
    {
        pthread_t tid;
        // TODO add tid to ready queue
        maxp_count++;
        allp_count++;

        struct PCB the_pcb = {.pid = allp_count, .tid = 0, .state = READY, .next_burst_length = 0, .remaining_burst_length = 0, .num_bursts = 0, .time_in_ready_list = 0, .IO_device1 = 0, .IO_device2 = 0, .start_time = 0, .finish_time = 0, .total_execution_time = 0};
        pthread_create(&tid, NULL, process, (void *)the_pcb);
    }

    while (allp_count <= ALLP)
    {
        pthread_t tid;
        // sleep for 5ms
        usleep(5000);
        float rn = frandom();
        if (rn > pg)
        {
            pthread_create(&tid, NULL, process, NULL);
            // TODO add tid to rdy queue
            maxp_count++;
            allp_count++;
        }
    }
    // TODO wait for all processes to finish
    pthread_exit(NULL);
}

void *p_sched(void *arg)
{
    toBeRun_pid = mutex_queue_rm();
    if (toBeRun_pid == -1)
    {
        // empty queue what to do?
    }
    else
    {
        // TODO run process
        pthread_cond_broadcast(&cpu_cond_var);
    }

    // TODO wait for all processes to finish
    pthread_exit(NULL);
}

void *process(void *arg)
{
    // todo implement process
    // pthread_self()
    enum task_type task = CPU;
    float rn;
    // cast *arg to PCB
    struct PCB my_pcb = (struct PCB)arg;
    // update pcb datas
    my_pcb.tid = pthread_self();

    while (task != TERMINATE)
    {
        mutex_queue_add(my_pcb);

        while (toBeRun_pid != my_pcb.pid)
        {
            // todo what mutex should be here?
            pthread_cond_wait(&cpu_cond_var, &pcb_queue_mutex);
        }
        
    //        pthread_cond_wait(&cpu_cond_var, &mut);
        //  if (state == Ready)
        buf_add(buf, index, i);

        if (cpu)
        {
            while (notmyturn)
            {
                pthread_cond_wait(cpu);
            }
            usleep(PCB.sleep_time);
            // todo update PCB
            determine next
        }
        else if (dev1)
        {
            while (notmyturn)
            {
                pthread_cond_wait(dev1);
            }
            // todo use dev1
        }
        else if (dev2)
        {
            while (notmyturn)
            {
                pthread_cond_wait(dev2);
            }
            // todo use dev2
        }
        rn = frandom();
        if (rn < p0)
        {
            task = TERMINATE;
        }
        else if (rn < p0 + p1)
        {
            task = IO1;
        }
        else if (rn <= 1)
        {
            task = IO2;
        }
    }

    pthread_exit(NULL);
}

void mutex_queue_add(struct PCB pcb)
{
    // lock mutex
    pthread_mutex_lock(&pcb_queue_mutex);
    // add pcb to queue
    pcb_queue_insert(pcb);
    // unlock mutex
    pthread_mutex_unlock(&pcb_queue_mutex);
};

int mutex_queue_rm()
{
    // lock mutex
    pthread_mutex_lock(&pcb_queue_mutex);
    // select next pcb
    int pid = pcb_queue_dequeue();
    // unlock mutex
    pthread_mutex_unlock(&pcb_queue_mutex);
    return pid;
};

void pcb_queue_insert(struct PCB pcb)
{
    enum algorithm ins_alg = alg;
    struct pcb_node *node = malloc(sizeof(struct pcb_node));
    node->pcb = pcb;
    node->next = NULL;

    if (ready_queue.count == 0)
    {
        ready_queue.head = node;
        ready_queue.tail = node;
    }
    else if (ins_alg == SJF)
    {
        struct pcb_node *curr = ready_queue.head;
        struct pcb_node *prev = NULL;
        while (curr != NULL && curr->pcb.next_burst_length < node->pcb.next_burst_length)
        {
            prev = curr;
            curr = curr->next;
        }
        if (prev == NULL)
        {
            node->next = ready_queue.head;
            ready_queue.head = node;
        }
        else
        {
            prev->next = node;
            node->next = curr;
        }
        if (curr == NULL)
            ready_queue.tail = node;
    }
    else
    {
        ready_queue.tail->next = node;
        ready_queue.tail = node;
    }
    ready_queue.count++;
}

int pcb_queue_dequeue()
{
    if (ready_queue.count == 0)
    {
        return -1;
    }
    else
    {
        struct pcb_node *node = ready_queue.head;
        ready_queue.head = node->next;
        ready_queue.count--;
        if (ready_queue.count == 0)
            ready_queue.tail = NULL;
        return node->pcb.pid;
    }
}

float frandom()
{
    return (float)rand() / RAND_MAX;
}