#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"

#define MAXCHAR                (1000)
#define FILELEN                ( 64 )
#define CHARLEN                ( 64 )
#define ERR                    ( 1  )
#define NUM_OF_DEFAULT_SYMBOLS ( 28 )


typedef struct C_dest_type{
    char instruction[CHARLEN];
    char binCode[3];
}C_dest_t;

typedef struct C_jmp_type{
    char instruction[CHARLEN];
    char binCode[3];
}C_jmp_t;

typedef struct C_comp_type{
    char instruction[CHARLEN];
    char binCode[8];
}C_comp_t;


//global symbol table
struct linked_list *symbol_table;
//global line count
static int lineCnt = -1;

const C_dest_t C_dest_lookup[] = {

    {"","000"},
    {"M","001"},
    {"D","010"},
    {"MD","011"},
    {"A","100"},
    {"AM","101"},
    {"AD","110"},
    {"AMD","111"},
    {NULL,NULL}
};

const C_jmp_t C_jmp_lookup[] = {

    {"","000"},
    {"JGT","001"},
    {"JEQ","010"},
    {"JGE","011"},
    {"JLT","100"},
    {"JNE","101"},
    {"JLE","110"},
    {"JMP","111"},
    {NULL,NULL}
};

const C_comp_t C_comp_lookup[] = {

    //computations associated with D&A registers
    {"0","0101010"},
    {"1","0111111"},
    {"-1","0111010"},
    {"D","0001100"},
    {"A","0110000"},
    {"!D","0001101"},
    {"!A","0110001"},
    {"-D","0001111"},
    {"-A","0110011"},
    {"D+1","0011111"},
    {"A+1","0110111"},
    {"D-1","0001110"},
    {"A-1","0110010"},
    {"D+A","0000010"},
    {"D-A","0010011"},
    {"A-D","0000111"},
    {"D&A","0000000"},
    {"D|A","0010101"},

    //computations associated with memory registers
    {"M","1110000"},
    {"!M","1110001"},
    {"M+1","1110111"},
    {"M-1","1110010"},
    {"D+M","1000010"},
    {"D-M","1010011"},
    {"M-D","1000111"},
    {"D&M","1000000"},
    {"D|M","1010101"},
    {NULL,NULL}
};

static int getCompField(char* str, char binCode[]){

    DPRINTF("getCompField: str len = %d\n",strlen(str));
    C_comp_t* iter = C_comp_lookup;
    if( strlen(str) == 0 ){
        strncpy(binCode,(const char*)iter->binCode,7);
            return 0;
    }
    while(strlen(iter->instruction)){

        DPRINTF("getCompField():iter->instruction = %s, length = %d\n",iter->instruction,strlen(iter->instruction));

        if(strcmp(str,iter->instruction) == 0){
            strncpy(binCode,(const char*)iter->binCode,7);
            return 0;
        }
        iter++;
    }
    return ERR;
}

static int getDestField(char* str, char binCode[]){

    DPRINTF("getDestField: str len = %d\n",strlen(str));
    C_dest_t* iter = C_dest_lookup;

    if( strlen(str) == 0 ){
        strncpy(binCode,(const char*)iter->binCode,7);
            return 0;
    }

    while( iter->instruction != NULL ){

        DPRINTF("getDestField():iter->instruction = %s, length = %d\n",iter->instruction,strlen(iter->instruction));

        if(strcmp(str,iter->instruction) == 0){
            strncpy(binCode,(const char*)iter->binCode,3);
            return 0;
        }
        iter++;
    }
    return ERR;
}

static int getJumpField(char* str, char binCode[]){

    DPRINTF("getJumpField: str len = %d\n",strlen(str));
    C_jmp_t* iter = C_jmp_lookup;
    while( iter->instruction != NULL ){

        DPRINTF("getJumpField():iter->instruction = %s, length = %d\n",iter->instruction,strlen(iter->instruction));

        if(strcmp(str,iter->instruction) == 0){
            strncpy(binCode,(const char*)iter->binCode,3);
            return 0;
        }
        iter++;
    }
    return ERR;
}

static void convIntegerToBinary(int integer, char bin[]){

    int c,k;

    char* iter = bin;

    for (c = 14; c >= 0; c--)
      {
        k = integer >> c;
     
        if (k & 1){
            *iter = '1';
          // printf("1");
        }
        else{
          *iter = '0';
            // printf("0");
        }
        iter++;
      }
}


static int isAllSpace(const char *str) {
    while (*str) {
        if (!isspace(*str++)) {
            return 0;
        }
    }
    return 1;
}

static void getLenForFields(const char* str, int* destLen, int* compLen, int* jumpLen, int* caseNum){

    int i;
    int len = strlen(str);

    *destLen = 0;
    *jumpLen = 0;
    *compLen = 0;
    *caseNum = 1;

    DPRINTF("getLenForFields: len = %d\n",len);
    for (int i = 0; i < len; i++){

        printf("str[%d] = %c\n",i,str[i]);

        if(str[i] == '='){
            *destLen = i;
        }
        else if(str[i] == ';'){

            if( i == 1 ){ //corresponds to the case where D;JMP
                *caseNum = 2;
                return;
            }
            else if(i == 3 ){ //corresponds to the case where @R0;JMP
                *caseNum = 3;
                return;
            }
            else if(*destLen == 0){ //corresponds to the case where A+1;JMP
                *compLen = i;
                *jumpLen = len - i - 2;
                return; 
            }
            else{  //corresponds to the case where D=M+1;JMP
                *compLen = i - *destLen - 1;
                *jumpLen = len - (i + 1) - 2;//the '2' is the ending character '\0' + '\n', (i+1) represents the length from index 0 to index representing ';', including index 0.
                return;
            }
        }
    }
   // *compLen = len - *destLen - 1 - 2; //the '1' is "=", the '2' is the ending character '\0' + '\n'
    *compLen = len - *destLen - 1;
    *jumpLen = 0;
 
    return;
}

static int initSymbolTable(void){

    symbol_table= linked_list_new();

    if(!symbol_table){
        printf("Error: failed to allocate space for symbol table\n");
        return ERR;
    }

    linked_list_insert(symbol_table, linked_list_new_string_node("R0",0));
    linked_list_insert(symbol_table, linked_list_new_string_node("R1",1));
    linked_list_insert(symbol_table, linked_list_new_string_node("R2",2));
    linked_list_insert(symbol_table, linked_list_new_string_node("R3",3));
    linked_list_insert(symbol_table, linked_list_new_string_node("R4",4));
    linked_list_insert(symbol_table, linked_list_new_string_node("R5",5));
    linked_list_insert(symbol_table, linked_list_new_string_node("R6",6));
    linked_list_insert(symbol_table, linked_list_new_string_node("R7",7));
    linked_list_insert(symbol_table, linked_list_new_string_node("R8",8));
    linked_list_insert(symbol_table, linked_list_new_string_node("R9",9));
    linked_list_insert(symbol_table, linked_list_new_string_node("R10",10));
    linked_list_insert(symbol_table, linked_list_new_string_node("R11",11));
    linked_list_insert(symbol_table, linked_list_new_string_node("R12",12));
    linked_list_insert(symbol_table, linked_list_new_string_node("R13",13));
    linked_list_insert(symbol_table, linked_list_new_string_node("R14",14));
    linked_list_insert(symbol_table, linked_list_new_string_node("R15",15));

    linked_list_insert(symbol_table, linked_list_new_string_node("SCREEN",16384));
    linked_list_insert(symbol_table, linked_list_new_string_node("KBD",24576));
    linked_list_insert(symbol_table, linked_list_new_string_node("SP",0));
    linked_list_insert(symbol_table, linked_list_new_string_node("LCL",1));
    linked_list_insert(symbol_table, linked_list_new_string_node("ARG",2));
    linked_list_insert(symbol_table, linked_list_new_string_node("THIS",3));
    linked_list_insert(symbol_table, linked_list_new_string_node("THAT",4));

    linked_list_print(symbol_table);

    return 0;
}

static void addSymbol(const char* symbol){

    DPRINTF("new symbol is being added to the symbol table: %s\n",symbol);

    linked_list_insert(symbol_table, linked_list_new_string_node(symbol,-1));
}

static int isNumeric (const char *s){

    if (s == NULL || *s == '\0' || isspace(*s))
      return 0;
    char *p;
    strtod(s, &p);
    return *p == '\0';
}

static void removeSpacesAndCarrigeReturn(char* source){

  char* i = source;
  char* j = source;
  while(*j != 0)
  {
    *i = *j++;
    if(*i != ' ' && (*i != '\n') && (*i != '\r'))
      i++;
  }
  *i = 0;
}

static void truncateComments(char text[]){

    char* iter = text;
    char* ref = text;
    int len = strlen(text);

    while(*iter){

        if(*iter == '/'){
            memset(iter,0,len-(iter - ref));
            return;
        }
        iter++;
    }
}

int main(int argc, char const *argv[]){

    FILE *inFp;
    FILE *outFP;
    char str[MAXCHAR];
    char* iter;
    int currLineNum = 0;
    int i;

    if(argc < 2 ){
        printf("Error: arguments not specified\n");
        printf("Usage: ./HackAssembler file.asm\n");
        exit(-1);
    }

    inFp = fopen(argv[1], "r");
    if (inFp == NULL){
        printf("Could not open file %s",argv[1]);
        exit(-1);
    }

    int pureFileNameLen = strlen(argv[1]) - 4;
    char pureFilename[32];
    strncpy(pureFilename,argv[1],pureFileNameLen);

    char* outputFileName = (char*)calloc(FILELEN,sizeof(char));
    strcat(outputFileName,pureFilename);
    strcat(outputFileName,".hack");

    outFP = fopen(outputFileName,"w");
    if(outFP == NULL){
        printf("Could not open file %s",outputFileName);
        exit(-1);
    }

    if(initSymbolTable() == ERR){
        exit(-1);
    }

    for(i = 0; i < 2; i++){

        lineCnt = 0;//reset
        rewind(inFp);
        while (fgets(str, MAXCHAR, inFp) != NULL){

            DPRINTF("str = %s\n",str);
          
            char command[17];
            memset(command,0,17);

            if(isAllSpace(str)){ //empty line
                continue;
            }

            if(*str == '/' && *(str+1) == '/'){ //comment
                continue;
            }

            truncateComments(str);
            removeSpacesAndCarrigeReturn(str);
            
            printf("line: %d, i = %d, post-processed str = %s\n",lineCnt, i,str);

            iter = str;

            if(*iter == '@'){ //A instruction
               
                iter++;

                if( i == 1 ){ //for pass 2, replace symbols (here can be either variable name or label name) with allocated memory address
                    
                    //check if the following string could form an integer, if not, it is a symbol
                    int isNum = isNumeric(iter);
                    memset(command,0,17);

                    if(isNum){

                        int address = atoi(iter);
                        command[0] = '0';
                        convIntegerToBinary(address, &command[1]);
                    }
                    else{

                        if(!linked_list_check_node_avail((const char*)iter, symbol_table)){

                            DPRINTF("length of post-processed text = %d\n",strlen(iter));
                            addSymbol(iter); //for pass 1, simply add the symbol to the symbol table

                            DPRINTF("in loop 1: add symbol %s",iter);
                            #ifdef DEBUG
                            linked_list_print(symbol_table);
                            #endif
                        }
                        
                        int address = linked_list_get_node_addr((const char*) iter, symbol_table);
                        DPRINTF("obtained the address of %s = %d\n",iter,address);

                        command[0] = '0';
                        convIntegerToBinary(address, &command[1]);
                    }

                    // printf("for A instruction: %s\n",command);
                    fprintf(outFP,"%s\n",command);
                }
            }
             else if(*iter == '('){ //label

                lineCnt--;

                if(i == 0){

                    DPRINTF("str = %s\n",str);
                    DPRINTF("reached label section here in loop 1\n");
    
                    char* ref = iter;

                    while(*iter){
                        if(*iter == ')'){
                            ref++;
                            char symbolName[CHARLEN];
                            memset(symbolName,0,CHARLEN);
                            strncpy(symbolName,ref,(iter - ref));
                            DPRINTF("in loop 1, under label branch: iter = %s\n",iter);
                            DPRINTF("in loop 1, under label branch: ref = %s\n",ref);
                            DPRINTF("in loop 1, under label branch: symbolName = %s\n",symbolName);
                            DPRINTF("in loop 1, under label branch: len(symbolName) = %d\n",strlen(symbolName));

                            if(!linked_list_check_node_avail((const char*)symbolName, symbol_table)){
                                addSymbol(symbolName);
                                linked_list_change_node_address(symbol_table, (const char*)symbolName, lineCnt+1);
                                // printf("in loop 1, under label branch: symbolName = %s, address = %d\n",symbolName,lineCnt);
                                #ifdef DEBUG
                                linked_list_print(symbol_table);
                                #endif
                            }
                        }
                        iter++;
                    }
                }
            }
            else{ 

                if(i == 1){

                  //  iter = text;
        
                    //C instruction
                    command[0] = '1';
                    command[1] = '1';
                    command[2] = '1';

                    char compField[8];
                    char jmpField[4];
                    char destField[4];

                    int destLen, compLen,jumpLen,caseNum;

                    char dest[8];
                    char jmp[8];
                    char comp[8];

                    //find the length for each command field
                    getLenForFields((const char*) iter, &destLen, &compLen, &jumpLen, &caseNum);

                    DPRINTF("compLen = %d\n", compLen);
                    DPRINTF("jumpLen = %d\n", jumpLen);
                    DPRINTF("destLen = %d\n", destLen);

                    memset(dest,0,8);
                    memset(jmp,0,8);
                    memset(comp,0,8);

                    DPRINTF("caseNum = %d\n",caseNum);

                    if(caseNum == 1){

                        strncpy(dest,(const char*)iter,destLen);
                        strncpy(comp,(const char*)&iter[destLen+1],compLen);
                        if(jumpLen){
                            strncpy(jmp,(const char*)&iter[destLen + compLen+2],jumpLen);
                        } 
                    }
                    else if(caseNum == 3){
                        strncpy(comp,(const char*)&iter[0],3);
                        strncpy(jmp,(const char*)&iter[4],3);
                    }
                    else{
                        strncpy(comp,(const char*)&iter[0],1);
                        strncpy(jmp,(const char*)&iter[2],3);
                    }

                    DPRINTF("dest = %s\n",dest);
                    DPRINTF("comp = %s\n",comp);
                    DPRINTF("jmp = %s\n",jmp);
                   
                    if(getCompField(comp,compField) == ERR){ //7
                        printf("Error: failed to parse comp field\n");
                        exit(-1);
                    }

                    if(getDestField(dest,destField) == ERR){ //3
                        printf("Error: failed to parse dest field\n");
                        exit(-1);
                    }

                    if(getJumpField(jmp,jmpField) == ERR){ //3
                        printf("Error: failed to parse jmp field\n");
                        exit(-1);
                    }

                    DPRINTF("compField = %s\n", compField);
                    DPRINTF("destField = %s\n", destField);
                    DPRINTF("jmpField = %s\n", jmpField);

                    strncpy(&command[3],compField,7);
                    strncpy(&command[10],destField,3);
                    strncpy(&command[13],jmpField,3);

                    //write command to the output file
                    fprintf(outFP,"%s\n",command);
                }
            }
             lineCnt++;
        }
    }

    del_linked_list(symbol_table);
    free(outputFileName);
    outputFileName = NULL;
    fclose(inFp);
    fclose(outFP);
    return 0;
}