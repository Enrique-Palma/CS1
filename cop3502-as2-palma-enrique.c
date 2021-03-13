#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leak_detector_c.h"

// cop3502-as2 Enrique Palma

/***********************************STRUCT*************************************************/
struct failFish_struct
{

    int sq_number;
    struct failFish_struct *next;
    struct failFish_struct *prev;
};
typedef struct failFish_struct failFish;

struct failFish_queue_struct
{

    int pond_num; //pond numbers
    char *pond_name;
    int n; //roof for fish
    int e; // iterator (skips to remove)
    int th; // remainder from first round
    failFish *head;
    failFish *tail;
};
typedef struct failFish_queue_struct failFish_queue;
/***********************************STRUCT*************************************************/

/***********************************functions**********************************************/
//failFish struct
failFish *create_failFish(int sq_number)
{
    failFish *ff = malloc(sizeof(failFish));
    ff->next = NULL;
    ff->prev = NULL;
    ff->sq_number = sq_number;
    return ff;
}

//failFish destructor
void remove_failFish(failFish *ff)
{
    free(ff);
}

//failFish_queue struct
failFish_queue *create_failFish_queue(failFish_queue *fq, int pond_num, char *pond_name, int n, int e, int th)
{
    fq->head = NULL;
    fq->tail = NULL;

    fq->pond_num = pond_num;
    fq->pond_name = strdup(pond_name);
    fq->n = n;
    fq->e = e;
    fq->th = th;
    return fq;
}

//failFish_queue destructor
void remove_failFish_queue(failFish_queue *fq)
{
    free(fq->pond_name);
    free(fq);
}

//reads number of ponds
int read_num_ponds(FILE *ifp, FILE *ofp)
{
    char buf[256];
    int num_pond;
    //get_next_nonblank_line(ifp, buf, 255);
    fgets(buf, 255, ifp);
    sscanf(buf, "%d", &num_pond);
    //printf("%d\n", num_pond);
    //fprintf(ofp, "%d\n", num_pond);
    return num_pond;
}

//reads and store file input
void read_input_failFish_queue(FILE *ofp, FILE *ifp, failFish_queue *fq)
{
    int pond_num, n, e, th;
    char pond_name[256];
    char buf[236];

    fgets(buf, 255, ifp);
    sscanf(buf, "%d%s%d%d%d", &pond_num, pond_name, &n, &e, &th);
    //printf("%d %s %d %d %d\n", pond_num, pond_name, n, e, th);
    //fprintf(ofp, "%d %s %d %d %d\n",pond_num, pond_name, n, e, th);

    fq = create_failFish_queue(fq, pond_num, pond_name, n, e, th);
}

//sorts course one
void sort_queues(failFish_queue *fq, int num_ponds)
{
    int i, j;
    failFish_queue *temp = malloc(sizeof(failFish));

    for(i = 0; i < (num_ponds - 1); i++)
    {
        failFish_queue temp = fq[i];

        for(j = 0; j < (num_ponds -1); j++)
        {
            if(fq[j].pond_num > fq[j +1].pond_num)
            {
                temp = fq[j +1];
                fq[j + 1] = fq[j];
                fq[j] = temp;
            }
        }
    }

    free(temp);
}

//prints initial pond satus
void print_initial(FILE *ofp, failFish *ff, failFish_queue *fq, int num_ponds)
{
    printf("Initial Pond Status\n");
    fprintf(ofp, "Initial Pond Status\n");
    int i;

    for(i = 0; i < num_ponds; i++)
    {
        printf("%d %s", fq[i].pond_num, fq[i].pond_name);
        fprintf(ofp, "%d %s", fq[i].pond_num, fq[i].pond_name);

        for(int j = 0; j < fq[i].n; j++)
        {
            printf(" %d", j + 1);
            fprintf(ofp, " %d", j + 1);
        }
        printf("\n");
        fprintf(ofp, "\n");
    }
}

//prints final pond satus
void print_final(FILE *ofp, failFish *ff, failFish_queue *fq, int num_ponds)
{
    printf("\nEnd of Course Pond Status");
    fprintf(ofp, "\nEnd of Course Pond Status");
    int i;

    for(i = 0; i < num_ponds; i++)
    {
        printf("\n%d %s", fq[i].pond_num, fq[i].pond_name);
        fprintf(ofp, "\n%d %s", fq[i].pond_num, fq[i].pond_name);
        ff = fq[i].head;

        for(int j = 0; j < fq[i].th; j++)
        {
            printf(" %d", ff->sq_number);
            fprintf(ofp, " %d", ff->sq_number);
            ff = ff->next;
        }
    }
}

//failFish queue
void enqueue(failFish_queue *fq)
{
    failFish *ff;

    for(int i = 0; i < fq->n; i++)
    {
        ff = create_failFish(i + 1);

        if(fq->tail == NULL)
        {
            fq->head = ff;
            fq->tail = ff;

            ff->prev = ff;
            ff->next = ff;
        }
        else
        {
            ff->next = fq->head;
            ff->prev = fq->tail;

            fq->head->prev = ff;
            fq->tail->next = ff;
            fq->tail = ff;
        }
    }
}

//failFish dequeue
void dequeue(failFish_queue *fq, failFish *ff)
{
    if(fq->head == NULL)
    {
        return;
    }
    if((fq->head == fq->tail) && (fq->head == ff))
    {
        fq->head = NULL;
        fq->tail = NULL;
        return;
    }
    if(ff == fq->head)
    {
        fq->head = fq->head->next;
        fq->head->prev = fq->tail;
        fq->tail->next = fq->head;
        return;
    }
    if(ff == fq->tail)
    {
        fq->tail = fq->tail->prev;
        fq->tail->next = fq->head;
        fq->head->prev = fq->tail;
        return;
    }

    ff->prev->next = ff->next;
    ff->next->prev = ff->prev;
}

//dispose failFish Queue
void dispose_failFish_queue_all(failFish_queue *fq)
{
    failFish *ff = fq->head;
    failFish *next;

    if(ff == NULL)
    {
        remove_failFish_queue(fq);
        return;
    }
    do
    {
        next = ff->next;
        remove_failFish(ff);
        ff = next;
    }
    while(ff != fq->head);

    remove_failFish_queue(fq);
}

//First meal
void courseOne(FILE *ofp, failFish_queue *fq, int num_ponds)
{
    failFish *ff;

    printf("\nFirst Course\n");
    fprintf(ofp, "\nFirst Course\n");

    for(int i = 0; i < num_ponds; i++)
    {
        printf("\n");
        printf("Pond %d: %s\n", fq[i].pond_num, fq[i].pond_name);
        fprintf(ofp, "\n");
        fprintf(ofp, "Pond %d: %s\n", fq[i].pond_num, fq[i].pond_name);

        ff = fq[i].head;

        for(int j =0; j < fq[i].e-1; j++)
        {
            ff = ff->next;
        }

        printf("failFish %d eaten\n", ff->sq_number);
        fprintf(ofp, "failFish %d eaten\n", ff->sq_number);

        dequeue(fq+i, ff);

        for(int k = 0; k < (fq[i].n - fq[i].th)-1; k++)
        {
            for(int j = 0; j < fq[i].e; j++)
            {
                ff = ff->next;
            }
            printf("FailFish %d eaten\n", ff->sq_number);
            fprintf(ofp, "FailFish %d eaten\n", ff->sq_number);

            dequeue(fq + i, ff);

        }
    }

}
//Checks if list is empty
int is_queue_empty(failFish_queue *fq)
{

    if(fq->head == NULL)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//peeks queue
failFish *peek(failFish_queue *fq)
{
    return fq->head;
}

//second course Sort
void sort_secondCourse_queues(failFish_queue *fq, int num_ponds)
{
    int i, j;

    for(i = 0; i < (num_ponds - 1); i++)
    {
        failFish_queue temp = fq[i];

        for(j = 0; j < (num_ponds -1); j++)
        {
            if(fq[j].head->sq_number < fq[j +1].head->sq_number)
            {
                temp = fq[j +1];
                fq[j + 1] = fq[j];
                fq[j] = temp;
            }
        }
    }
}

//second meal course
void secondCourse(FILE *ofp, failFish_queue *fq, int num_ponds)
{
    failFish *ff;
    int i = 0;

    printf("\n\nSecond Course\n\n");
    fprintf(ofp, "\n\nSecond Course\n\n");

    for(i = 0; i < num_ponds - 1; i++)
    {
        ff = peek(fq + i);

        printf("Eaten: FailFish %d from pond %d\n", ff->sq_number, fq[i].pond_num);
        fprintf(ofp, "Eaten: FailFish %d from pond %d\n", ff->sq_number, fq[i].pond_num);

        dequeue(fq+i, ff);

        for(int k = 0; k < fq[i].th -1; k++)
        {
            ff = ff->next;
            printf("Eaten: FailFish %d from pond %d\n", ff->sq_number, fq[i].pond_num);
            fprintf(ofp, "Eaten: FailFish %d from pond %d\n", ff->sq_number, fq[i].pond_num);
            dequeue(fq+i, ff);
        }
    }

    ff = peek(fq + i);

    for(int j = 0; j < (fq[i].n - fq[i].th); j++)
    {
        fprintf(ofp, "Eaten: FailFish %d from pond %d\n", ff->sq_number, fq[i].pond_num);
        printf("Eaten: FailFish %d from pond %d\n", ff->sq_number, fq[i].pond_num);
        dequeue(fq+i, ff);
        ff = ff->next;
    }
    ff = ff->next;

    printf("\nFailFish %d from pond %d remains\n", ff->sq_number, fq[i].pond_num);
    fprintf(ofp, "\nFailFish %d from pond %d remains\n", ff->sq_number, fq[i].pond_num);
}
/***********************************functions**********************************************/


int main(void)
{
    //Memory Leak
    atexit(report_mem_leak);

    //file I/o
    FILE *ifp;
    FILE *ofp;
    ifp = fopen("cop3502-as2-input.txt", "r");
    if(ifp == NULL)
    {
        printf("NULL!!!\n");
    }

    ofp = fopen("cop3502-as2-output-Palma-Enrique.txt", "w");
    //file I/o

    //variables pointers and malloc
    int i;
    failFish *ff;

    int num_ponds;
    num_ponds = read_num_ponds(ifp, ofp);

    failFish_queue *fq = malloc(sizeof(failFish_queue)*num_ponds);

    //loop number of ponds to read failFish queue
    for(int i = 0; i < num_ponds; i++)
    {
        read_input_failFish_queue(ofp, ifp, fq+i);
    }

    //calls sort Course one
    sort_queues(fq, num_ponds);

    //initial status of pond
    print_initial(ofp, ff, fq, num_ponds);

    //loop to enqueue
    for(i = 0; i < num_ponds; i++)
    {
        enqueue(fq+i);
    }

    //calls course one
    courseOne(ofp, fq, num_ponds);
    //prints final status pond
    print_final(ofp, ff, fq,num_ponds);

    //sorts second course
    sort_secondCourse_queues(fq, num_ponds);

    //calls second course
    secondCourse(ofp, fq, num_ponds);

    //removes fish queue
    remove_failFish_queue(fq);

    //close file I/o
    fclose(ifp);
    fclose(ofp);
}


