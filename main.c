
#include <dlfcn.h>  // обязательно подключать для плагинов

#include <malloc.h>
#include <dirent.h>

#include "main.h"

#define MAX_NUMBER_DESCRIPTOR 255

struct descriptor_plugin descriptor_number[MAX_NUMBER_DESCRIPTOR];
void *handle[MAX_NUMBER_DESCRIPTOR];
//void (*plug_make_func[MAX_NUMBER_DESCRIPTOR])(void, void);
void (*plug_make_func[MAX_NUMBER_DESCRIPTOR])(void*, void*, void*);
void (*plug_printf_func[MAX_NUMBER_DESCRIPTOR])(void);

void print_invalid_operation(void*, void*, void*);
void init_plugin(void);

int main()
{
    int a,b,c, n;
    char *error;
    
    for(unsigned int i = 0; i < MAX_NUMBER_DESCRIPTOR; ++i)
    {
        descriptor_number[i].number_operation = 0;
        plug_make_func[i] = print_invalid_operation;
    }
    init_plugin();
    
 /*   */
    printf("Enter two numbers:");
    scanf("%d %d",&a,&b);
    printf("Enter your choice:\n");
    
    for(unsigned int i = 0; i < MAX_NUMBER_DESCRIPTOR; ++i)
    {
        if(0 != descriptor_number[i].number_operation)
        {
            plug_printf_func[i]();
        }
    }
    
    
    scanf("%d",&n);
    plug_make_func[n](&c, &a, &b);
    
    
    for(unsigned int i = 0; i < MAX_NUMBER_DESCRIPTOR; ++i)
    {
        if(0 != descriptor_number[i].number_operation)
        {
            dlclose(handle[i]);
        }
    }
    
    return 0;
}

void print_invalid_operation(void* aa, void* bb, void* cc)
{
    printf("Invalid operation \n");
}




void init_plugin(void)
{
    struct dirent **namelist;
    int n;
    
    n = scandir("./PlaginLib", &namelist, NULL, alphasort);
    if (n == -1) {
        perror("scandir");
//        exit(EXIT_FAILURE);
    }

    while (n--) {
//        printf("%s\n", namelist[n]->d_name);
        int i = 0;
        while(0 != namelist[n]->d_name[i]){++i;}
        if(3 < i)
        {
            if(('o' == namelist[n]->d_name[i - 1]) && ('s' == namelist[n]->d_name[i - 2]) && ('.' == namelist[n]->d_name[i - 3]))
            {
                void *handle_temp;
                struct descriptor_plugin *descriptor_temp;
                
                handle_temp = dlopen(namelist[n]->d_name,  RTLD_LAZY);
                if (!handle_temp) {
                    fprintf(stderr, "%s\n", dlerror());
//                    return 1;
                }
                dlerror();    // Clear any existing error 
                
                descriptor_temp = dlsym(handle_temp, "descriptor_lib");
                descriptor_number[descriptor_temp->number_operation] = (*descriptor_temp);
                handle[descriptor_temp->number_operation] = handle_temp;
                plug_printf_func[descriptor_temp->number_operation] = dlsym(handle_temp, descriptor_temp->name_printf_func);
                plug_make_func[descriptor_temp->number_operation] = dlsym(handle_temp, descriptor_temp->name_make_func);
//                printf("%s\n", namelist[n]->d_name);
            }
        }
        free(namelist[n]);
    }
    free(namelist);
}



