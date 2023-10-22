#include <stdlib.h>
#include <string.h>
#include <stdio.h>



typedef struct NAMES {
    unsigned char n;
    char **names;
} Names;

int addName(Names *names, char *name)
{
    char **ptr;
    
    if(names == NULL)
        return 0;
    
    // memory
    if(names->n == 0)
        ptr = (char **)malloc(sizeof(char *));
    else ptr = (char **)realloc(names->names, (names->n + 1) * sizeof(char *));
    
    if(ptr == NULL)
        return -1;
    
    // insert
    *(ptr + names->n) = name;
    names->names = ptr;
    names->n++;
    
    return 1;
}

// middle got priority over mean,
// because mean is set to 1 per default
int main (int argc, const char *argv[])
{
    // flags:
    unsigned char flag_break = 0, flag_mean = 1, flag_merge = 0, flag_middle = 0, flag_verify = 0;
    // file and variable names
    Names names_include = {0, NULL}, names_pdata = {0, NULL}, names_pdataA = {0, NULL}, names_xpdata = {0, NULL}, names_xpdataA = {0, NULL}, names_collision = {0, NULL};
    // output filename
    char outDefault[] = "default.mdl";
    char *name_out = outDefault;
    int i;
    
    // parse command line
    for(i = 1; i < argc; i++) {
        // flags
        if(strcmp(argv[i], "break") == 0)
            flag_break = 1;
        else if(strcmp(argv[i], "mean") == 0)
            flag_mean = 1;
        else if(strcmp(argv[i], "merge") == 0)
            flag_merge = 1;
        else if(strcmp(argv[i], "middle") == 0)
            flag_middle = 1;
        else if(strcmp(argv[i], "verify") == 0)
            flag_verify = 1;
        else {
            // names
            if(strcmp(argv[i], "include") == 0) {
                if((i + 1) < argc) {
                    if(addName(&names_include, (char *)argv[i+1]) != 1)
                        printf("Error adding name %s!\n", argv[i+1]);
                    i++;
                }
            } else if(strcmp(argv[i], "pdata") == 0) {
                if((i + 1) < argc) {
                    if(addName(&names_pdata, (char *)argv[i+1]) != 1)
                        printf("Error adding name %s!\n", argv[i+1]);
                    i++;
                }
            } else if(strcmp(argv[i], "pdata[]") == 0) {
                if((i + 1) < argc) {
                    if(addName(&names_pdataA, (char *)argv[i+1]) != 1)
                        printf("Error adding name %s!\n", argv[i+1]);
                    i++;
                }
            } else if(strcmp(argv[i], "xpdata") == 0) {
                if((i + 1) < argc) {
                    if(addName(&names_xpdata, (char *)argv[i+1]) != 1)
                        printf("Error adding name %s!\n", argv[i+1]);
                    i++;
                }
            } else if(strcmp(argv[i], "xpdata[]") == 0) {
                if((i + 1) < argc) {
                    if(addName(&names_xpdataA, (char *)argv[i+1]) != 1)
                        printf("Error adding name %s!\n", argv[i+1]);
                    i++;
                }
            } else if(strcmp(argv[i], "collision") == 0) {
                if((i + 1) < argc) {
                    if(addName(&names_collision, (char *)argv[i+1]) != 1)
                        printf("Error adding name %s!\n", argv[i+1]);
                    i++;
                }
            } else if(strcmp(argv[i], "out") == 0) {
                if((i + 1) < argc) {
                    name_out = (char *)argv[i+1];
                    i++;
                }
            }
        }
    }
    
    printf("\n\n");
    
    // print registered commandline inputs
    printf("Using options: %sbreak, %smerge, %s and %sverify,\n", (flag_break == 1 ? "" : "no_"), (flag_merge == 1 ? "" : "no_"), (flag_middle == 1 ? "middle" : "mean"), (flag_verify == 1 ? "" : "no_"));
    printf("I will use polygon data variables named:\n");
    for(i = 0; i < names_pdata.n; i++)
        printf("\t%s\n", *(names_pdata.names + i));
    for(i = 0; i < names_pdataA.n; i++)
        printf("\t%s\n", *(names_pdataA.names + i));
    for(i = 0; i < names_xpdata.n; i++)
        printf("\t%s\n", *(names_xpdata.names + i));
    for(i = 0; i < names_xpdataA.n; i++)
        printf("\t%s\n", *(names_xpdataA.names + i));
    printf("and a ground collision types variables named:\n");
    for(i = 0; i < names_collision.n; i++)
        printf("\t%s\n", *(names_collision.names + i));
    printf("found in files:\n");
    for(i = 0; i < names_include.n; i++)
        printf("\t%s\n", *(names_include.names + i));
    printf("To create a polygon map file named\n\t%s\n.........\n", name_out);
    
    // running other programs
    if(system(NULL) == 0)
        printf("No command processor found!\n");
    else printf("Command processor found!\n");

    printf("Trying to run command...\n");
    i = system("gcc");
    if(i == -1)
        printf("Shell process could not be created!\n");
    else printf("Status of the shell process is %i.\n", i);
    
    return 1;
}