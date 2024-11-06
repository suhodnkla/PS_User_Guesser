#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include <pthread.h>

#define dis_line    puts("________________________________________________________________________________________________\n")

const char master_dir[] = "<Directory>";

const int long_len = 34;

const int substr_type = 11;
const int etc_type = 2;
const int type_cnt = substr_type + etc_type;

const char find_str[16][16][16]={
        {"struct",  "class"},
        {"){",      ") {"},
        {"vector<", "queue<",   "map<",     "stack<",   "set<",     "pair<",    "string",   "tuple<"},
        {"template","typedef",  "using"},
        {"union",   "register", "static",   "alloc",    "goto"},
        {"auto"},
        {"int*",    "double*",  "float*",   "long*",    "char*",
         "int *",   "double *", "float *",  "long *",   "char *"},
        {"{"},
        {"cin",     "cout"},
        {"#"},
        {"?"}};

const char type_name[16][16]={"str","Ast","STL","tmp","wrd","aut","poi","CB","C++","pre","thr","lng","emt"};

double W[16]={4, 1, 3.5, 1, 10, 2, 1, 0.5, 0.5, 0.5, 10, 0.1, 2};

struct code_type{
    char name[32];
    int len;

    struct Group{
        int cnt;
        double type[16];
        double av_len;
    }G[16];
}devs[32];

int length;

double get_point(int*type,double*ans,double*grp){
    double ret = 0;
    for(register int i = 0;i ^ type_cnt;i++)
        ans[i]=0;

    for(register int i = 0;i ^ type_cnt;i++)
        ans[i]=pow(type[i]-grp[i],2)*W[i],
        ret += ans[i];

    return ret;
}

void get_line(char*line,int*type){
    for(int i = 0;i < substr_type;i++)
        for(int j = 0;find_str[i][j][0];j++)
            if(strstr(line,find_str[i][j]))
                type[i]++;

    if(strlen(line)>=long_len)
        type[substr_type]++;

    for(register char* i = line;*i;i++)
        if(*i != '\t' && *i != ' ')
            goto OUT;
    type[substr_type+1]++;
    OUT:
    return;
}

int init(void){

#define Dtop    devs[length]
#define Gi      Dtop.G[i]

    double buf[16];

    char name[128];
    char dir[128];
    char line[256];

    int type[16];

    sprintf(dir, "%s\\names",master_dir);

    FILE* namefile=fopen(dir,"r+");
    if(namefile == NULL)
        return -1;

    for(length = 0;~fscanf(namefile,"%s",name);length++){
        strcpy(Dtop.name,name);
        for(int T = 0;;T++){

            int idx = -1;
            double Cl = 200.0;

            int code_len = 0;

            sprintf(dir,"%s\\%s\\%d",master_dir,name,T);
            FILE*code=fopen(dir,"r");

            memset(type,0,sizeof(int)*type_cnt);

            if(code==NULL)
                break;

            for(;fgets(line,128,code);code_len++)
                get_line(line,type);

            for(int i = 0;i < Dtop.len;i++){
                for(int j =0;j < type_cnt;j++)
                    Gi.type[j]/=Gi.cnt;
                register double tmp = get_point(type,buf,Gi.type);
                if(tmp <= 120.0/Gi.cnt&&tmp<Cl)
                    idx = i,
                    Cl=tmp;
                for(int j =0;j < type_cnt;j++)
                    Gi.type[j]*=Gi.cnt;
            }
            if(!~idx) {
                for (int i = 0; i < type_cnt; i++)
                    Dtop.G[Dtop.len].type[i] += type[i];
                Dtop.G[Dtop.len].cnt++;
                Dtop.G[Dtop.len].av_len += code_len;
                Dtop.len++;
            }
            else{
                for(int i = 0;i < type_cnt;i++)
                    Dtop.G[idx].type[i] += type[i];
                Dtop.G[idx].cnt++;
                Dtop.G[idx].av_len += code_len;
            }
            fclose(code);
        }
        for(int i = 0;i < Dtop.len;i++)
            for(int j = 0;j < type_cnt;j++)
                Dtop.G[i].type[j] /= Dtop.G[i].cnt;
    }
    fclose(namefile);
    return 0;
}

int guess_who(char* dir, bool log){

    double min_score=-1,tmp;
    int closest = 0;

    double WW[64]={0,};

    int type[16]={0,};
    char line[256];

    FILE*code=fopen(dir,"r");

    if(code==NULL)
        return -1;

    for(;fgets(line,256,code);)
        get_line(line,type);

    fclose(code);

    for(int i = 0;i < length;i++){
        double usr_min=-1;
        int Gn;
        for(int j = 0;j < devs[i].len;j++){
            if(devs[i].G[j].cnt<3)
                continue;

            tmp=get_point(type,WW,devs[i].G[j].type);

            if(tmp<min_score||min_score==-1)
                min_score=tmp,
                closest=i;

            if(log)
                if(usr_min>tmp||usr_min==-1)
                    usr_min=tmp,
                    Gn=j;
        }
        if(log)
            printf("%16s\t %c : %.1lf\n",devs[i].name,Gn+'A',usr_min);
    }

    printf("this code is written by %s\n",devs[closest].name);
    dis_line;
    return 0;
}

void print_dev_grp(int n){
    printf("%s : \n",devs[n].name);
    for(int i = 0;i < devs[n].len;i++){
        printf("%c : %d\n",'A'+i,devs[n].G[i].cnt);
        for(int j = 0;j < type_cnt;j++)
            printf("%0.1f\t",devs[n].G[i].type[j]);
        puts("");
    }

    dis_line;
}

int main(){
    char dir[256];

    if(!~init())
        return 1;
    for(int i = 0;i < length;i++)
        print_dev_grp(i);
    do{
        static int i = 0;

        sprintf(dir,"%s\\%s\\%d",master_dir,"<Username>",i);

        i++;
    }while(~guess_who(dir,1));
    return 0;
}
