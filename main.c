#include <stdio.h>
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>   /* malloc, free, rand */

int Fsize=50; /* max string length of formulas*/
int inputs=10;
int ThSize=100;/* maximum size of set of formulas, if needed*/
int TabSize=500; /*maximum length of tableau queue, if needed*/


/* A set will contain a list of words. Use NULL for emptyset.  */
struct set{
    char* item;/*first word of non-empty set*/
    struct set* tail;/*remaining words in the set*/
};

/* A tableau will contain a list of pointers to sets (of words).  Use NULL for empty list.*/
struct tableau {
    struct set* S; /* pointer to first set in non-empty list */
    struct tableau* rest; /*list of pointers to other sets*/
};

/*put all your functions here. You will need
int parse(char *g) which returns 1 if a propos ition, 2 if neg, 3 if binary, ow 0
Of course you will almost certainly need other functions.

For binary formulas you will also need functions that return the first part and the second part of the binary formula.
*/

/*You need to change this next bit and include functions for parse, closed and complete.*/ 



/*----------------------------------------------------Section 1: Parser----------------------------------------------------------------*/
// Part one of binary formula
char* partone(char* g){
    int count = 0;
    int split = -1;
    int isProp = 0;
    for (int i = 0; i < strlen(g); ++i) {
        if (g[i] == '('){
            count++;
        }else if (g[i] == ')'){
            count--;
            if (count == 1) {
                split = i + 1;
                break;
            }
        }else if (g[i] == 'p' || g[i] == 'q' || g[i] == 'r'){
            isProp = 1;
        }else if (g[i] == '^' || g[i] == '>' || g[i] == 'v'){
            if (count == 1 && isProp){
                split = i;
                break;
            }
        }
    }
    char* result = (char*) malloc(sizeof(char) * (split));
    for (int i = 1; i < split; ++i) {
        result[i - 1] = g[i];
    }
    return result;
}

// Part two of binary formula
char* parttwo(char* g){
    int count = 0;
    int split = -1;
    int isProp = 0;
    for (int i = 0; i < strlen(g); ++i) {
        if (g[i] == '('){
            count++;
        }else if (g[i] == ')'){
            count--;
            if (count == 1) {
                split = i + 1;
                break;
            }
        }else if (g[i] == 'p' || g[i] == 'q' || g[i] == 'r'){
            isProp = 1;
        }else if (g[i] == '^' || g[i] == '>' || g[i] == 'v'){
            if (count == 1 && isProp){
                split = i;
                break;
            }
        }
    }
    char* result = (char*) malloc(sizeof(char) * (strlen(g) - split));
    for (int i = split + 1, k = 0; i < strlen(g) - 1; ++i, ++k) {
        result[k] = g[i];
    }
    return result;
}

// Parse funtion
int parse(char* name){
    unsigned len = strlen(name);
    int count = 0;
    int pairPara = 0;
    int numOfProp = 0;
    for (int i = 0; i < len; ++i) {
        char temp = name[i];
        if (temp == '('){
            count++;
        }else if(temp == 'p' || temp == 'q' || temp == 'r'){
            numOfProp++;
        }else if(temp == ')'){
            count--;
            pairPara++;
        }
    }

    if (count == 0 && numOfProp == pairPara + 1){
        if(len == 1)
            return 1;
        else if (name[0] == '-')
            return 2;
        else
            return 3;

    } return 0;
}

/*----------------------------------------------------Section 2: Tableau----------------------------------------------------------------*/

// obtain the current operator when doing tableau
char getOperator(char* g){
    int count = 0;
    int split = -1;
    int isProp = 0;
    for (int i = 0; i < strlen(g); ++i) {
        if (g[i] == '('){
            count++;
        }else if (g[i] == ')'){
            count--;
            if (count == 1) {
                split = i + 1;
                break;
            }
        }else if (g[i] == 'p' || g[i] == 'q' || g[i] == 'r'){
            isProp = 1;
        }else if (g[i] == '^' || g[i] == '>' || g[i] == 'v'){
            if (count == 1 && isProp){
                split = i;
                break;
            }
        }
    }
    return g[split];
}

char* auxNegation(char* name, int* size){
    while (*size > 2 && name[1] == '-') {
        name += 2;
        *size -= 2;
    }
    return name;
}

char* doNegation(char* name){
    int i = strlen(name);
    int* size = &i;
    char* newName = (char*) malloc(sizeof(char) * Fsize);
    if (name[1] == '-')
        name = auxNegation(name, size);
    if (name[0] == '-' && i != 2){
        while (name[0] == '-' && *size > 2){

            if (getOperator(name) == '^'){
                name++;
                char* p1 = partone(name);
                char* p2 = parttwo(name);
                strcat(newName, "(");
                strcat(newName, "-");
                strcat(newName, p1);
                strcat(newName, "v");
                strcat(newName, "-");
                strcat(newName, p2);
                strcat(newName, ")");
            }else if (getOperator(name) == 'v'){
                name++;
                char* p1 = partone(name);
                char* p2 = parttwo(name);
                strcat(newName, "(");
                strcat(newName, "-");
                strcat(newName, p1);
                strcat(newName, "^");
                strcat(newName, "-");
                strcat(newName, p2);
                strcat(newName, ")");
            } else if (getOperator(name) == '>'){
                name++;
                char* p1 = partone(name);
                char* p2 = parttwo(name);
                strcat(newName, "(");
                strcat(newName, p1);
                strcat(newName, "^");
                strcat(newName, "-");
                strcat(newName, p2);
                strcat(newName, ")");
            }
        }
        return newName;
    }else {
        return name;
    }
}

// decide whether it is a terminal: 1 is terminal; 2 is not a terminal
int isTerminal(char* name){
    if (name == NULL){
        return 1;
    }

    if (name[0] == '-'){
        if (name[1] == 'p' || name[1] == 'q' || name[1] == 'r')
            return 1;
        else
            return 0;
    }else if(name[0] == 'p' || name[0] == 'q' || name[0] == 'r'){
        return 1;
    }else
        return 0;
}


// Complete function
void complete(struct tableau* t){
    struct tableau* table = t;
    struct set* set;
    while (table){
        set = table -> S;
        struct set* head = table -> S;
        while (set){
            if (!isTerminal(set -> item)){
                if (set -> item[0] == '-'){
                    set -> item = doNegation(set -> item);
                }
                if (isTerminal(set -> item)){
                    set = set -> tail;
                    continue;
                }
                char op = getOperator(set -> item);
                if (op == '^'){
                    struct set* newSet = (struct set*)malloc(sizeof(struct set));
                    newSet -> item = parttwo(set -> item);
                    newSet -> tail = set -> tail;
                    set -> tail = newSet;
                    set -> item = partone(set -> item);
                }else if(op == 'v'){
                    struct tableau* newT = (struct tableau*) malloc(sizeof(struct tableau));
                    struct set* newSet = (struct set*)malloc(sizeof(struct set));
                    newT -> S = newSet;
                    struct set* cS = head;
                    while (strcmp(cS -> item, set -> item) != 0){
                        newSet -> item = cS -> item;
                        newSet -> tail = (struct set*)malloc(sizeof(struct set));
                        newSet = newSet -> tail;
                        cS = cS -> tail;
                    }
                    newSet -> item = parttwo(set -> item);
                    cS = cS -> tail;
                    if (cS){
                        newSet -> tail = (struct set*)malloc(sizeof(struct set));
                        newSet = newSet -> tail;
                        while (cS){
                            newSet -> item = cS -> item;
                            if (cS -> tail) {
                                newSet->tail = (struct set *) malloc(sizeof(struct set));
                                newSet = newSet -> tail;
                            } else{
                                newSet -> tail = NULL;
                            }
                            cS = cS -> tail;
                        }
                    }else{
                        newSet -> tail = NULL;
                    }
                    set -> item = partone(set -> item);
                    newT -> rest = table -> rest;
                    table -> rest = newT;
                }else{
                    struct tableau* newT = (struct tableau*) malloc(sizeof(struct tableau));
                    struct set* newSet = (struct set*)malloc(sizeof(struct set));
                    newT -> S = newSet;
                    struct set* cS = head;
                    while (strcmp(cS -> item, set -> item) != 0){
                        newSet -> item = cS -> item;
                        newSet -> tail = (struct set*)malloc(sizeof(struct set));
                        newSet = newSet -> tail;
                        cS = cS -> tail;
                    }
                    newSet -> item = parttwo(set -> item);
                    cS = cS -> tail;
                    if (cS){
                        newSet -> tail = (struct set*)malloc(sizeof(struct set));
                        newSet = newSet -> tail;
                        while (cS){
                            newSet -> item = cS -> item;
                            if (cS -> tail) {
                                newSet->tail = (struct set *) malloc(sizeof(struct set));
                                newSet = newSet -> tail;
                            } else{
                                newSet -> tail = NULL;
                            }
                            cS = cS -> tail;
                        }
                    }else{
                        newSet -> tail = NULL;
                    }
                    char* nP1 = (char*)malloc(sizeof(char) * Fsize);
                    strcat(nP1, "-");
                    strcat(nP1, partone(set -> item));
                    set -> item = nP1;
                    newT -> rest = table -> rest;
                    table -> rest = newT;
                }
            }else{
                set = set -> tail;
            }
        }
        table = table -> rest;
    }

}

// Closed funtion
int closed(struct tableau* t) {
    struct set* set;
    /* what is read into the set
        0  1  2  3  4  5
        p -p  q -q  r -r
    */
    int isRead[6]; 

    int tmp = 0;
    int isClose = 1;
    while (t){
        set = t -> S;
        for (int i = 0; i < 6; ++i)
            isRead[i] = 0;
        while (set){
            if (strcmp(set -> item, "p") == 0){
                isRead[0] = 1;

            } else if (strcmp(set -> item, "-p") == 0){
                isRead[1] = 1;
            }else if (strcmp(set -> item, "q") == 0){
                isRead[2] = 1;
            }else if (strcmp(set -> item, "-q") == 0){
                isRead[3] = 1;
            }else if (strcmp(set -> item, "r") == 0){
                isRead[4] = 1;
            }else{
                isRead[5] = 1;
            }
            set = set -> tail;
        }
        for (int i = 1; i < 6; i+=2) {
            if (isRead[i] == 1 && isRead[i-1] == 1)
                tmp = 1;
        }
        if (!tmp){
            return 0;
        }
        tmp = 0;
        t = t -> rest;
    }
    return isClose;
}


/*----------------------------------------------------Main----------------------------------------------------------------*/
int main(){

    char* name = (char*)malloc(sizeof(char)*Fsize);
    /*You should not need to alter the program below.*/

    FILE *fp, *fpout;
    /* reads from input.txt, writes to output.txt*/
    if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
    if ((  fpout=fopen("output.txt","w"))==NULL){printf("Error opening file");exit(1);}
    int j;
    struct set* S=(struct set *)malloc(sizeof(struct set));
    struct tableau* t=(struct tableau *)malloc(sizeof(struct tableau));
    for(j=0;j<inputs;j++)
    {
        fscanf(fp, "%s",name);/*read formula*/
        switch (parse(name))
        {
            case(0): fprintf(fpout, "%s is not a formula.  \n", name);break;
            case(1): fprintf(fpout, "%s is a proposition. \n ", name);break;
            case(2): fprintf(fpout, "%s is a negation.  \n", name);break;
            case(3):fprintf(fpout, "%s is a binary. The first part is %s and the second part is %s \n", name, partone(name), parttwo(name));break;
            default:fprintf(fpout, "What the f***!  ");
        }

        if (parse(name)!=0)
        {
            S -> item = name;
            S -> tail = NULL;
            t -> S = S;
            t -> rest = NULL;
            complete(t);
            if (closed(t))  fprintf(fpout, "%s is not satisfiable.\n", name);
            else fprintf(fpout, "%s is satisfiable.\n", name);
        }
        else  fprintf(fpout, "I told you, %s is not a formula.\n", name);
    }


    fclose(fp);
    fclose(fpout);
    free(name);
    free(S);
    free(t);


    return(0);
}


