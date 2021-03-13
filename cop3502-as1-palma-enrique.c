#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "leak_detector_c.h"
#include "sp21_cop3502_as1.h"

//Enrique Palma Assignment[1]

//function prototypes
int get_next_nonblank_line(FILE *ifp, char *buf, int max_length);
void remove_crlf(char *s);
int number_of_monsters(FILE *ifp);
void read_monster(FILE *ifp, monster *ptr_m, int id);
void clear_monster(monster *ptr_m);
int number_of_regions(FILE *ifp);
void read_region(FILE *ifp, region *ptr_r, monster *ptr_m);
void clear_region(region *ptr_r);
int number_of_trainers(FILE *ifp);
void read_trainer(FILE *ifp, trainer *ptr_t, region *ptr_r);
void clear_trainer(trainer *ptr_t);
void computations(trainer *ptr_t, int ntrainers, int nregions, FILE *ofp);
//function prototypes

int main(void)
{
    //Memory Leak
    atexit(report_mem_leak);
    //Variables
    monster *monsters;
    int nmonsters;
    int nregions;
    int ntrainers;

    //file I/o
    FILE *ofp;
    FILE *ifp;
    ofp = fopen("cop3502-as1-output-Palma-Enrique.txt", "w");
    ifp = fopen("cop3502-as1-input.txt", "r");

    //checks connection with file
    if(ifp != NULL)
    {
        //printf("We successfully opened the input file.\n");
    }
    else
    {
        printf("NULL!!!\n");
        return 0;
    }

    //Monsters
    nmonsters = number_of_monsters(ifp);
    monsters = calloc(sizeof(monster), nmonsters);
    for(int i = 0; i < nmonsters; i++)
    {
        read_monster(ifp, &(monsters[i]), i);
    }

    //Regions
    nregions = number_of_regions(ifp);
    region *regions = calloc(sizeof(region), nregions);
    for(int j = 0; j < nregions; j++)
    {
        read_region(ifp, regions+j, monsters);
    }

    //trainers
    ntrainers = number_of_trainers(ifp);
    trainer *trainers = calloc(sizeof(trainer),ntrainers);
    for(int k=0; k<ntrainers; k++)
    {
        read_trainer(ifp, trainers+k, regions);
    }

    //computing
    computations(trainers,ntrainers,nregions,ofp);

    //dispose callocs and mallocs
    for(int h = 0; h < ntrainers; h++)
    {
        clear_trainer(trainers + h);
    }

    for(int m = 0; m < nregions; m++)
    {
        clear_region(regions +m);
    }

    for(int l = 0; l < nmonsters; l++)
    {
        clear_monster(monsters +l);
    }

    free(trainers);
    free(regions);
    free(monsters);

    //close files
    fclose(ifp);
    fclose(ofp);

    return 0;
}
/***********************************functions**********************************************/
//In class code Dr.Gerber
//This function will write the next line from ifp that isn't blank into buf, removing any
//newline characters.  It writes a zero length string at eof, and only then.  It returns
//true if it successfully read a line and false if it didn't.
int get_next_nonblank_line(FILE *ifp, char *buf, int max_length)
{
    buf[0] = '\0';

    while(!feof(ifp) && (buf[0] == '\0'))
    {
        fgets(buf, max_length, ifp);
        remove_crlf(buf);
    }

    if(buf[0] != '\0')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

//In class code Dr.Gerber
//This function will remove carriage-return and line-feed characters from the end of a string.
void remove_crlf(char *s)
{
    char *t = s + strlen(s);
    t--;

    while((t >= s) && (*t == '\n' || *t == '\r'))
    {
        *t = '\0';
        t--;
    }
}

/****************************************Monsters START****************************************/
//function to read number of monsters
int number_of_monsters(FILE *ifp)
{
    char buf[256];
    int nmonsters;
    get_next_nonblank_line(ifp, buf, 255);
    sscanf(buf, "%d", &nmonsters);
    return nmonsters;
}

//Monsters constructor
void read_monster(FILE *ifp, monster *ptr_m, int id)
{
    char buf[256];
    char namebuf[256];
    char elementbuf[256];
    int population;

    get_next_nonblank_line(ifp, buf, 255);
    sscanf(buf, "%s%s%d", namebuf, elementbuf, &population);

    ptr_m-> id = id ++;
    ptr_m-> name = strdup(namebuf);
    ptr_m-> element = strdup(elementbuf);
    ptr_m-> population = population;
}

//Monster destructor
void clear_monster(monster *ptr_m)
{
    free(ptr_m->name);
    free(ptr_m->element);
}
/****************************************Monsters END*********************************************/
/****************************************Regions START********************************************/
//Function to read number of regions
int number_of_regions(FILE *ifp)
{
    char buf[256];
    int nregions;
    get_next_nonblank_line(ifp, buf, 255);
    sscanf(buf, "%d", &nregions);
    return nregions;
}

//Region Constructor
void read_region(FILE *ifp, region *ptr_r, monster *ptr_m)
{
    //variables
    int nmonsters;
    char buf[256];
    char name_buf[256];
    char monster_buf[256];
    int j=0, i =0 ;

    //scanning
    get_next_nonblank_line(ifp, buf, 255);
    sscanf(buf, "%s\n", name_buf);
    ptr_r->name = strdup(name_buf);

    //scanning
    get_next_nonblank_line(ifp, buf, 255);
    sscanf(buf, "%d", &nmonsters);
    ptr_r->nmonsters = nmonsters;
    ptr_r-> monsters = calloc(sizeof(monster*), nmonsters);

    for(i =0; i < nmonsters; i++)
    {
        //scanning
        get_next_nonblank_line(ifp, buf, 255);
        sscanf(buf, "%s\n", monster_buf);

        ptr_r -> monsters[i] = malloc(sizeof(monster));

        while(strcmp(monster_buf, (ptr_m+j)->name) !=0)
        {
            j++;
        }
        ptr_r->monsters[i]->id = (ptr_m + j)->id;
        ptr_r->monsters[i]->name = (ptr_m + j)->name;
        ptr_r->monsters[i]->element = (ptr_m + j)->element;
        ptr_r->monsters[i]->population = (ptr_m + j)->population;
        ptr_r->total_population += (ptr_m + j)->population;
    }
}

//Regions Destructor
void clear_region(region *ptr_r)
{
    for(int i=0; i < ptr_r->nmonsters; i++)
    {
        free(ptr_r->monsters[i]);
    }
    free(ptr_r->monsters);
    free(ptr_r->name);
}
/****************************************Regions END**********************************************/
/****************************************Trainers START*******************************************/
//function to read number of trainers
int number_of_trainers(FILE *ifp)
{
    char buf[256];
    int ntrainers;
    get_next_nonblank_line(ifp, buf, 255);
    sscanf(buf, "%d", &ntrainers);
    return ntrainers;
}

//Trainer constructor
void read_trainer(FILE *ifp, trainer *ptr_t, region *ptr_r)
{
    //variables
    int nregions;
    char buf[256];
    char name_buf[256];
    char region_buf[256];
    int captures;
    int j=0, i =0 ;

    ptr_t->visits = malloc(sizeof(itinerary));
    //scanning
    get_next_nonblank_line(ifp, buf, 255);
    sscanf(buf, "%s\n", name_buf);
    ptr_t->name = strdup(name_buf);
    //scanning
    get_next_nonblank_line(ifp, buf, 255);
    sscanf(buf, "%d", &captures);
    ptr_t->visits->captures = captures;
    //scanning
    get_next_nonblank_line(ifp, buf, 255);
    sscanf(buf, "%d", &nregions);
    ptr_t->visits->nregions = nregions;

    ptr_t->visits->regions = calloc(sizeof(region*), nregions);

    for(i =0; i < nregions; i++)
    {
        //scanning
        get_next_nonblank_line(ifp, buf, 255);
        sscanf(buf, "%s\n", region_buf);
        ptr_t->visits->regions[i] = malloc(sizeof(region));

        while(strcmp(region_buf, (ptr_r+j)->name) != 0)
        {
            j++;
        }
        ptr_t->visits->regions[i]->name = (ptr_r + j)->name;
        ptr_t->visits->regions[i]->nmonsters = (ptr_r + j)->nmonsters;
        ptr_t->visits->regions[i]->total_population = (ptr_r + j)->total_population;
        ptr_t->visits->regions[i]->monsters = (ptr_r + j)->monsters;
    }
}

//Trainer Destructor
void clear_trainer(trainer *ptr_t)
{
    for(int j =0; j < ptr_t->visits->nregions; j++)
    {
        free(ptr_t->visits->regions[j]);
    }
    free(ptr_t->name);
    free(ptr_t->visits->regions);
    free(ptr_t->visits);
}
/****************************************Trainers END*******************************************/
/****************************************Computations START*************************************/
//function to calculate commonality
void computations(trainer *ptr_t, int ntrainers, int nregions, FILE *ofp)
{
    //variables
    int i, j, k, total;
    double commonality;

    //loops to calculate commonality
    for(i=0; i<ntrainers; i++)
    {
        fprintf(ofp,"%s\n", ptr_t[i].name);
        for(j=0; j <ptr_t[i].visits->nregions; j++)
        {
            fprintf(ofp, "%s\n", ptr_t[i].visits->regions[j]->name);
            for(k=0; k<ptr_t[i].visits->regions[j]->nmonsters; k++)
            {
                commonality = ((double)ptr_t[i].visits->regions[j]->monsters[k]->population / (double)ptr_t[i].visits->regions[j]->total_population) * (double)ptr_t[i].visits->captures;
                total = round(commonality);

                if(0 < total)
                {
                    fprintf(ofp, "%d %s\n", total, ptr_t[i].visits->regions[j]->monsters[k]->name);
                }
            }
        }

        fprintf(ofp, "\n");
    }
}
/****************************************End computations****************************************/
