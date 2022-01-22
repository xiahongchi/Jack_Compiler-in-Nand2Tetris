#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

void compile_class();
void compile_classVarDec();
void compile_subroutineDec();
void compile_parameterList();
void compile_subroutineBody();
void compile_varDec();
void compile_className();
void compile_subroutineName();
void compile_varName();

void compile_statements();
void compile_statement();
void compile_letStatement();
void compile_ifStatement();
void compile_whileStatement();
void compile_doStatement();
void compile_returnStatement();

void compile_expression();
void compile_term();
void compile_subroutineCall();
void compile_expressionList();
void compile_op();
void compile_unaryOp();
void compile_KeywordConstant();

void compile_integerConstant();
void compile_StringConstant();
void compile_identifier();

unsigned peek(const char*);
char* getname();
unsigned peek_kind();//0 kw;1 symbol;2 intC;3 StrC;4 ident
unsigned readline=1;
struct STelm{
    char name[30];
    char type[20];
    char kind[10];
    unsigned n;
};
STelm classtb[100];
STelm subrttb[100];
char classname[30];
unsigned classvar,subrtvar;
unsigned staticn,fieldn;
unsigned argn,localn;
unsigned ifn,whilen;
FILE*wfp;
char Txml[1000][100];

int main(){
    DIR* dirp;
    struct dirent *ptr;
    char dirpath[]="../ConvertToBin/";
    if((dirp=opendir(dirpath))==NULL){
        printf("invalid directory path.");
        exit(0);
    }
    while((ptr=readdir(dirp))!=NULL){
        char*p=strchr(ptr->d_name,'.');
        if(strcmp(p+1,"jack")==0){
            //tokenizer
            char filepath[100];
            strcpy(filepath,dirpath);
            strcat(filepath,ptr->d_name);
            FILE*fp;
            if((fp=fopen(filepath,"r"))==NULL){
                printf("invalid path.");
                exit(0);
            }
            else{
                printf("open file:%s\n",filepath);
            }
            char src[1000][200];
            unsigned k=0,l=0;
            while(!feof(fp)){
                while(!feof(fp)&&(src[k][l++]=fgetc(fp))!='\n');
                src[k++][l-1]='\0';
                l=0;
            }
            unsigned kmax=k;
            fclose(fp);
            k=0;
            l=0;
            unsigned m=0;
            for(unsigned i=0;i<1000;i++) Txml[i][0]='\0';
            strcpy(Txml[m++],"<tokens>");
            while(k<kmax){
                //skip null chars
                if(src[k][l]=='\n'||src[k][l]=='\0'){
                    k++;l=0;
                    continue;
                }
                if(src[k][l]=='\t'||src[k][l]==' '){
                    l++;
                    continue;
                }
                //skip comments
                if(src[k][l]=='/'&&src[k][l+1]=='/'){
                    k++;l=0;
                    continue;
                }
                if(src[k][l]=='/'&&src[k][l+1]=='*'&&src[k][l+2]=='*'){
                    l+=3;
                    while(true){
                        if(src[k][l]=='\n'||src[k][l]=='\0'){
                            k++;l=0;
                            continue;
                        }
                        if(src[k][l]=='*'&&src[k][l+1]=='/'){
                            l+=2;
                            break;
                        }
                        l++;
                    }
                    continue;
                }
                //symbol
                if(src[k][l]=='{'||src[k][l]=='}'||src[k][l]=='('||src[k][l]==')'||src[k][l]=='['||
                    src[k][l]==']'||src[k][l]=='.'||src[k][l]==','||src[k][l]==';'||src[k][l]=='+'||
                    src[k][l]=='-'||src[k][l]=='*'||src[k][l]=='/'||src[k][l]=='&'||src[k][l]=='|'||
                    src[k][l]=='<'||src[k][l]=='>'||src[k][l]=='='||src[k][l]=='~'){
                    //fprintf(fp,"<symbol> %c </symbol>\n",src[k][l]);
                    strcpy(Txml[m],"<symbol> ");
                    char sym[2];
                    sym[0]=src[k][l];
                    sym[1]='\0';
                    strcat(Txml[m],sym);
                    strcat(Txml[m++]," </symbol>");
                    l++;
                    continue;
                }
                //integer constant
                if(src[k][l]>='0'&&src[k][l]<='9'){
                    unsigned intcst=0;
                    do{
                        intcst=intcst*10+src[k][l]-'0';
                        l++;
                    }while(src[k][l]>='0'&&src[k][l]<='9');
                    char intstr[20];
                    itoa(intcst,intstr,10);
                    //fprintf(fp,"<integerConstant> %d </integerConstant>\n",intcst);
                    strcpy(Txml[m],"<integerConstant> ");
                    strcat(Txml[m],intstr);
                    strcat(Txml[m++]," </integerConstant>");
                    continue;
                }
                //string constant
                if(src[k][l]=='"'){
                    char str[100];
                    unsigned strk=0;
                    do{
                        str[strk++]=src[k][++l];
                    }while(src[k][l]!='"');
                    l++;
                    str[--strk]='\0';
                    //fprintf(fp,"<stringConstant> %s </stringConstant>\n",str);
                    strcpy(Txml[m],"<stringConstant> ");
                    strcat(Txml[m],str);
                    strcat(Txml[m++]," </stringConstant>");
                    continue;
                }
                //keyword or identifier
                if((src[k][l]>='a'&&src[k][l]<='z')||(src[k][l]>='A'&&src[k][l]<='Z')){
                    char id[100];
                    unsigned idk=0;
                    do{
                        id[idk++]=src[k][l++];
                    }while(src[k][l]=='_'||(src[k][l]>='a'&&src[k][l]<='z')||(src[k][l]>='A'&&src[k][l]<='Z'));
                    id[idk]='\0';
                    if(strcmp(id,"class")==0||strcmp(id,"constructor")==0||strcmp(id,"function")==0||strcmp(id,"method")==0||
                        strcmp(id,"field")==0||strcmp(id,"static")==0||strcmp(id,"var")==0||strcmp(id,"int")==0||
                        strcmp(id,"char")==0||strcmp(id,"boolean")==0||strcmp(id,"void")==0||strcmp(id,"true")==0||
                        strcmp(id,"false")==0||strcmp(id,"null")==0||strcmp(id,"this")==0||strcmp(id,"let")==0||
                        strcmp(id,"do")==0||strcmp(id,"if")==0||strcmp(id,"else")==0||strcmp(id,"while")==0||
                        strcmp(id,"return")==0){
                        //fprintf(fp,"<keyword> %s </keyword>\n",id);
                        strcpy(Txml[m],"<keyword> ");
                        strcat(Txml[m],id);
                        strcat(Txml[m++]," </keyword>");
                    }
                    else{
                        //fprintf(fp,"<identifier> %s </identifier>\n",id);
                        strcpy(Txml[m],"<identifier> ");
                        strcat(Txml[m],id);
                        strcat(Txml[m++]," </identifier>");
                    }
                    continue;
                }
                printf("impossible to reach here!");
            }
            strcpy(Txml[m++],"</tokens>");
            //compile
            char wfilepath[100];
            strcpy(wfilepath,dirpath);
            char*pwc=strchr(ptr->d_name,'.');
            *pwc='\0';
            strcat(wfilepath,ptr->d_name);
            strcat(wfilepath,"F.vm");
            if((wfp=fopen(wfilepath,"w"))==NULL){
                printf("invalid write path:%s",wfilepath);
                exit(-1);
            }
            k=0;l=0;
            readline=1;
            classvar=0;
            staticn=0;
            fieldn=0;
            compile_class();
            fclose(wfp);
            if((wfp=fopen(wfilepath,"r"))==NULL){
                printf("invalid read path:%s",wfilepath);
                exit(-1);
            }
            strcpy(filepath,dirpath);
            strcat(filepath,ptr->d_name);
            strcat(filepath,".vm");
            if((fp=fopen(filepath,"r"))==NULL){
                printf("invalid read path:%s",filepath);
                exit(-1);
            }
            char orifile[50000];
            char wfile[50000];
            k=0;
            while(!feof(wfp)) wfile[k++]=fgetc(wfp);
            wfile[k]='\0';
            k=0;
            while(!feof(fp)) orifile[k++]=fgetc(fp);
            orifile[k]='\0';
            fclose(wfp);
            fclose(fp);
            k=0;
            char*wp=strchr(wfile,'\n');
            char*op=strchr(orifile,'\n');
            *wp='\0';
            *op='\0';
            char*wpn=wp+1;
            char*opn=op+1;
            int cprs=strcmp(wfile,orifile);
            printf("write||real\n");
            if(cprs!=0) printf("line %d: %s || %s\n",k,wfile,orifile);
            while(*wpn!=-1){
                k++;
                wp=strchr(wpn,'\n');
                op=strchr(opn,'\n');
                *wp='\0';
                *op='\0';
                int cprs=strcmp(wpn,opn);
                if(cprs!=0) printf("line %d: %s || %s\n",k,wpn,opn);
                wpn=wp+1;opn=op+1;
            }
        }
    }
    system("pause");
    return 0;
}
void compile_class(){
    readline++;
    char* cn=getname();
    strcpy(classname,cn);
    free(cn);
    readline+=2;
    unsigned peekrs;
    while((peekrs=peek("static field"))==1) compile_classVarDec();
    while((peekrs=peek("constructor function method"))==1) compile_subroutineDec();
    readline++;
}
void compile_classVarDec(){
    unsigned peekrs;
    char* linename;
    char typen[20];
    if((peekrs=peek("static"))==1){
        strcpy((classtb[classvar]).kind,"static");
        readline++;
        linename=getname();
        strcpy(typen,linename);
        strcpy((classtb[classvar]).type,typen);
        free(linename);
        readline++;
        linename=getname();
        strcpy((classtb[classvar]).name,linename);
        free(linename);
        readline++;
        (classtb[classvar]).n=staticn++;
        classvar++;
        while((peekrs=peek(","))==1){
            readline++;
            strcpy((classtb[classvar]).kind,"static");
            strcpy((classtb[classvar]).type,typen);
            linename=getname();
            strcpy((classtb[classvar]).name,linename);
            free(linename);
            (classtb[classvar]).n=staticn++;
            classvar++;
            readline++;
        }
        readline++;
        return;
    }
    else{//field
        strcpy((classtb[classvar]).kind,"field");
        readline++;
        linename=getname();
        strcpy(typen,linename);
        strcpy((classtb[classvar]).type,typen);
        free(linename);
        readline++;
        linename=getname();
        strcpy((classtb[classvar]).name,linename);
        free(linename);
        readline++;
        (classtb[classvar]).n=fieldn++;
        classvar++;
        while((peekrs=peek(","))==1){
            readline++;
            strcpy((classtb[classvar]).kind,"field");
            strcpy((classtb[classvar]).type,typen);
            linename=getname();
            strcpy((classtb[classvar]).name,linename);
            free(linename);
            (classtb[classvar]).n=fieldn++;
            classvar++;
            readline++;
        }
        readline++;
        return;
    }
}
void compile_subroutineDec(){
    unsigned peekrs;
    char* linename;
    argn=0;
    localn=0;
    subrtvar=0;
    ifn=0;
    whilen=0;
    if((peekrs=peek("constructor"))==1){
        readline+=2;
        linename=getname();
        char funcname[20];
        strcpy(funcname,linename);
        free(linename);
        readline+=2;
        compile_parameterList();
        readline++;
        readline++;
        while((peekrs=peek("var"))==1){
            compile_varDec();
        }
        fprintf(wfp,"function %s.%s %d\n",classname,funcname,localn);
        fprintf(wfp,"push constant %d\n",fieldn);
        fprintf(wfp,"call Memory.alloc 1\n");
        fprintf(wfp,"pop pointer 0\n");
        compile_statements();
        readline++;
        return;
    }
    if((peekrs=peek("function"))==1){
        readline+=2;
        linename=getname();
        char funcname[20];
        strcpy(funcname,linename);
        free(linename);
        readline+=2;
        compile_parameterList();
        readline++;
        readline++;
        while((peekrs=peek("var"))==1){
            compile_varDec();
        }
        fprintf(wfp,"function %s.%s %d\n",classname,funcname,localn);
        compile_statements();
        readline++;
        return;
    }
    if((peekrs=peek("method"))==1){
        readline+=2;
        linename=getname();
        char funcname[20];
        strcpy(funcname,linename);
        free(linename);
        strcpy((subrttb[subrtvar]).type,classname);
        strcpy((subrttb[subrtvar]).name,"this");
        strcpy((subrttb[subrtvar]).kind,"argument");
        (subrttb[subrtvar++]).n=argn++;
        readline+=2;
        compile_parameterList();
        readline++;
        readline++;
        while((peekrs=peek("var"))==1){
            compile_varDec();
        }
        fprintf(wfp,"function %s.%s %d\n",classname,funcname,localn);
        fprintf(wfp,"push argument 0\n");
        fprintf(wfp,"pop pointer 0\n");
        compile_statements();
        readline++;
        return;
    }
    printf("control never reach here");
}
void compile_parameterList(){
    unsigned peekrs;
    if((peekrs=peek(")"))==1) return;
    char* linename;
    linename=getname();
    strcpy((subrttb[subrtvar]).type,linename);
    free(linename);
    readline++;
    linename=getname();
    strcpy((subrttb[subrtvar]).name,linename);
    free(linename);
    readline++;
    strcpy((subrttb[subrtvar]).kind,"argument");
    (subrttb[subrtvar++]).n=argn++;
    while((peekrs=peek(","))==1){
        readline++;
        linename=getname();
        strcpy((subrttb[subrtvar]).type,linename);
        free(linename);
        readline++;
        linename=getname();
        strcpy((subrttb[subrtvar]).name,linename);
        free(linename);
        readline++;
        strcpy((subrttb[subrtvar]).kind,"argument");
        (subrttb[subrtvar++]).n=argn++;
    }
}
void compile_subroutineBody(){
    readline++;
    unsigned peekrs;
    while((peekrs=peek("var"))==1){
        compile_varDec();
    }
    compile_statements();
    readline++;
}
void compile_varDec(){
    readline++;
    char*linename;
    unsigned peekrs;
    char typen[20];
    linename=getname();
    strcpy(typen,linename);
    strcpy((subrttb[subrtvar]).type,typen);
    free(linename);
    readline++;
    linename=getname();
    strcpy((subrttb[subrtvar]).name,linename);
    free(linename);
    readline++;
    strcpy((subrttb[subrtvar]).kind,"local");
    (subrttb[subrtvar++]).n=localn++;
    while((peekrs=peek(","))==1){
        readline++;
        strcpy((subrttb[subrtvar]).type,typen);
        linename=getname();
        strcpy((subrttb[subrtvar]).name,linename);
        free(linename);
        strcpy((subrttb[subrtvar]).kind,"local");
        (subrttb[subrtvar++]).n=localn++;
        readline++;
    }
    readline++;
    return;
}

void compile_statements(){
    unsigned peekrs;
    while((peekrs=peek("let if while do return"))==1){
        compile_statement();
    }
}
void compile_statement(){
    unsigned peekrs;
    if((peekrs=peek("let"))==1){
        compile_letStatement();
        return;
    }
    if((peekrs=peek("if"))==1){
        compile_ifStatement();
        return;
    }
    if((peekrs=peek("while"))==1){
        compile_whileStatement();
        return;
    }
    if((peekrs=peek("do"))==1){
        compile_doStatement();
        return;
    }
    if((peekrs=peek("return"))==1){
        compile_returnStatement();
        return;
    }
    printf("control never reach here!");
}
void compile_letStatement(){
    unsigned peekrs;
    readline++;
    readline++;
    peekrs=peek("[");
    readline--;
    if(peekrs==1){
        char varname[30];
        char kind[10];
        unsigned n;
        char*linename;
        linename=getname();
        strcpy(varname,linename);
        free(linename);
        unsigned i;
        for(i=0;i<subrtvar;i++){
            if(strcmp((subrttb[i]).name,varname)==0){
                strcpy(kind,(subrttb[i]).kind);
                n=(subrttb[i]).n;
                break;
            }
        }
        if(i==subrtvar){
            for(i=0;i<classvar;i++){
                if(strcmp((classtb[i]).name,varname)==0){
                    strcpy(kind,(classtb[i]).kind);
                    n=(classtb[i]).n;
                    break;
                }
            }
        }
        if(strcmp(kind,"field")==0){
            strcpy(kind,"this");
        }
        
        readline+=2;
        compile_expression();
        fprintf(wfp,"push %s %d\n",kind,n);
        fprintf(wfp,"add\n");
        readline+=2;
        compile_expression();
        fprintf(wfp,"pop temp 0\n");
        fprintf(wfp,"pop pointer 1\n");
        fprintf(wfp,"push temp 0\n");
        fprintf(wfp,"pop that 0\n");
        readline++;
        return;
    }
    else{
        char varname[30];
        char kind[10];
        unsigned n;
        char*linename;
        linename=getname();
        strcpy(varname,linename);
        free(linename);
        unsigned i;
        for(i=0;i<subrtvar;i++){
            if(strcmp((subrttb[i]).name,varname)==0){
                strcpy(kind,(subrttb[i]).kind);
                n=(subrttb[i]).n;
                break;
            }
        }
        if(i==subrtvar){
            for(i=0;i<classvar;i++){
                if(strcmp((classtb[i]).name,varname)==0){
                    strcpy(kind,(classtb[i]).kind);
                    n=(classtb[i]).n;
                    break;
                }
            }
        }
        readline++;
        readline++;
        compile_expression();
        if(strcmp(kind,"field")==0){
            strcpy(kind,"this");
        }
        fprintf(wfp,"pop %s %d\n",kind,n);
        readline++;
        return;
    }
}
void compile_ifStatement(){
    unsigned curifn=ifn++;
    readline+=2;
    compile_expression();
    fprintf(wfp,"if-goto IF_TRUE%d\n",curifn);
    fprintf(wfp,"goto IF_FALSE%d\n",curifn);
    fprintf(wfp,"label IF_TRUE%d\n",curifn);
    readline+=2;
    compile_statements();
    readline++;
    unsigned peekrs;
    if((peekrs=peek("else"))==1){
        readline+=2;
        fprintf(wfp,"goto IF_END%d\n",curifn);
        fprintf(wfp,"label IF_FALSE%d\n",curifn);
        compile_statements();
        fprintf(wfp,"label IF_END%d\n",curifn);
        readline++;
        return;
    }
    else{
        fprintf(wfp,"label IF_FALSE%d\n",curifn);
        return;
    }
}
void compile_whileStatement(){
    unsigned curwhilen=whilen++;
    readline+=2;
    fprintf(wfp,"label WHILE_EXP%d\n",curwhilen);
    compile_expression();
    fprintf(wfp,"not\n");
    fprintf(wfp,"if-goto WHILE_END%d\n",curwhilen);
    readline+=2;
    compile_statements();
    fprintf(wfp,"goto WHILE_EXP%d\n",curwhilen);
    fprintf(wfp,"label WHILE_END%d\n",curwhilen);
    readline++;
}
void compile_doStatement(){
    readline++;
    compile_subroutineCall();
    fprintf(wfp,"pop temp 0\n");
    readline++;
}
void compile_returnStatement(){
    readline++;
    unsigned peekrs=peek(";");
    if(peekrs==1){
        fprintf(wfp,"push constant 0\n");
        fprintf(wfp,"return\n");
        readline++;
        return;
    }
    else{
        compile_expression();
        fprintf(wfp,"return\n");
        readline++;
        return;
    }
}

void compile_expression(){
    compile_term();
    unsigned peekrs;
    while((peekrs=peek("+ - * / & | < > ="))==1){
        char*linename;
        linename=getname();
        char opstr[30];
        strcpy(opstr,linename);
        free(linename);
        readline++;
        compile_term();
        if(strcmp(opstr,"+")==0) strcpy(opstr,"add");
        else if(strcmp(opstr,"-")==0) strcpy(opstr,"sub");
        else if(strcmp(opstr,"*")==0) strcpy(opstr,"call Math.multiply 2");
        else if(strcmp(opstr,"/")==0) strcpy(opstr,"call Math.divide 2");
        else if(strcmp(opstr,"&")==0) strcpy(opstr,"and");
        else if(strcmp(opstr,"|")==0) strcpy(opstr,"or");
        else if(strcmp(opstr,"<")==0) strcpy(opstr,"lt");
        else if(strcmp(opstr,">")==0) strcpy(opstr,"gt");
        else if(strcmp(opstr,"=")==0) strcpy(opstr,"eq");
        else printf("control never reach here");
        fprintf(wfp,"%s\n",opstr);
    }
}
void compile_term(){
    unsigned peekrs,peek_k;
    char*linename;
    peek_k=peek_kind();
    if(peek_k==2){//intC
        linename=getname();
        fprintf(wfp,"push constant %s\n",linename);
        free(linename);
        readline++;
        return;
    }
    if(peek_k==3){//strC
        linename=getname();
        unsigned slen=strlen(linename);
        fprintf(wfp,"push constant %d\n",slen);
        fprintf(wfp,"call String.new 1\n");
        unsigned i;
        for(i=0;i<slen;i++){
            fprintf(wfp,"push constant %d\n",unsigned(linename[i]));
            fprintf(wfp,"call String.appendChar 2\n");
        }
        readline++;
        return;
    }
    //keyconst
    if((peekrs=peek("true"))==1){
        fprintf(wfp,"push constant 0\n");
        fprintf(wfp,"not\n");
        readline++;
        return;
    }
    if((peekrs=peek("false"))==1){
        fprintf(wfp,"push constant 0\n");
        readline++;
        return;
    }
    if((peekrs=peek("null"))==1){
        fprintf(wfp,"push constant 0\n");
        readline++;
        return;
    }
    if((peekrs=peek("this"))==1){
        fprintf(wfp,"push pointer 0\n");
        readline++;
        return;
    }
    //(expr)
    if((peekrs=peek("("))==1){
        readline++;
        compile_expression();
        readline++;
        return;
    }
    //unaryop term
    if((peekrs=peek("- ~"))==1){
        char unarystr[5];
        if((peekrs=peek("-"))==1){
            strcpy(unarystr,"neg");
        }
        else{
            strcpy(unarystr,"not");
        }
        readline++;
        compile_term();
        fprintf(wfp,"%s\n",unarystr);
        return;
    }
    readline++;
    peekrs=peek("[");
    readline--;
    if(peekrs==1){
        char vname[30];
        char kind[10];
        unsigned n;
        linename=getname();
        strcpy(vname,linename);
        free(linename);
        unsigned i;
        for(i=0;i<subrtvar;i++){
            if(strcmp((subrttb[i]).name,vname)==0){
                strcpy(kind,(subrttb[i]).kind);
                n=(subrttb[i]).n;
                break;
            }
        }
        if(i==subrtvar){
            for(i=0;i<classvar;i++){
                if(strcmp((classtb[i]).name,vname)==0){
                    strcpy(kind,(classtb[i]).kind);
                    n=(classtb[i]).n;
                    break;
                }
            }
        }
        if(strcmp(kind,"field")==0){
            strcpy(kind,"this");
        }
        readline+=2;
        compile_expression();
        fprintf(wfp,"push %s %d\n",kind,n);
        fprintf(wfp,"add\n");
        fprintf(wfp,"pop pointer 1\n");
        fprintf(wfp,"push that 0\n");
        readline++;
        return;
    }
    readline++;
    peekrs=peek(". (");
    readline--;
    if(peekrs==1){
        compile_subroutineCall();
        return;
    }
    char vname[30];
    char kind[10];
    unsigned n;
    linename=getname();
    strcpy(vname,linename);
    free(linename);
    unsigned i;
    for(i=0;i<subrtvar;i++){
        if(strcmp((subrttb[i]).name,vname)==0){
            strcpy(kind,(subrttb[i]).kind);
            n=(subrttb[i]).n;
            break;
        }
    }
    if(i==subrtvar){
        for(i=0;i<classvar;i++){
            if(strcmp((classtb[i]).name,vname)==0){
                strcpy(kind,(classtb[i]).kind);
                n=(classtb[i]).n;
                break;
            }
        }
    }
    if(strcmp(kind,"field")==0){
        strcpy(kind,"this");
    }
    fprintf(wfp,"push %s %d\n",kind,n);
    readline++;
    return;
}
void compile_subroutineCall(){
    unsigned peekrs;
    unsigned srtargn=0;
    readline++;
    peekrs=peek("(");
    readline--;
    if(peekrs==1){
        char srtname[30];
        char* linename;
        linename=getname();
        strcpy(srtname,linename);
        free(linename);
        readline+=2;
        fprintf(wfp,"push pointer 0\n");
        srtargn++;
        if((peekrs=peek(")"))!=1){
            compile_expression();
            srtargn++;
            while((peekrs=peek(","))==1){
                readline++;
                compile_expression();
                srtargn++;
            }
        }
        readline++;
        fprintf(wfp,"call %s.%s %d\n",classname,srtname,srtargn);
        return;
    }
    else{
        char srtname[30];
        char firstname[30];
        char typen[30];
        char kindname[10];
        unsigned varn;
        char*linename;
        linename=getname();
        strcpy(firstname,linename);
        free(linename);
        readline+=2;
        linename=getname();
        strcpy(srtname,linename);
        free(linename);
        readline+=2;
        unsigned i;
        bool var=false;
        for(i=0;i<subrtvar;i++){
            if(strcmp((subrttb[i]).name,firstname)==0){
                var=true;
                strcpy(typen,(subrttb[i]).type);
                strcpy(kindname,(subrttb[i]).kind);
                varn=(subrttb[i]).n;
                break;
            }
        }
        if(i==subrtvar){
            for(i=0;i<classvar;i++){
                if(strcmp((classtb[i]).name,firstname)==0){
                    var=true;
                    strcpy(typen,(classtb[i]).type);
                    strcpy(kindname,(classtb[i]).kind);
                    varn=(classtb[i]).n;
                    break;
                }
            }
        }
        if(strcmp(kindname,"field")==0){
            strcpy(kindname,"this");
        }
        if(var==true){
            fprintf(wfp,"push %s %d\n",kindname,varn);
            srtargn++;
            if((peekrs=peek(")"))!=1){
                compile_expression();
                srtargn++;
                while((peekrs=peek(","))==1){
                    readline++;
                    compile_expression();
                    srtargn++;
                }
            }
            readline++;
            fprintf(wfp,"call %s.%s %d\n",typen,srtname,srtargn);
            return;
        }
        else{
            if((peekrs=peek(")"))!=1){
                compile_expression();
                srtargn++;
                while((peekrs=peek(","))==1){
                    readline++;
                    compile_expression();
                    srtargn++;
                }
            }
            readline++;
            fprintf(wfp,"call %s.%s %d\n",firstname,srtname,srtargn);
            return;
        }
    }
}

unsigned peek(const char*str){
    char cur[100];
    unsigned k=0,l=0;
    while(Txml[readline][k]!='>') k++;
    k+=2;
    while(Txml[readline][k]!=' ') cur[l++]=Txml[readline][k++];
    cur[l]='\0';

    char strc[100];
    strcpy(strc,str);
    char*pc,*pstr;
    pstr=strc;
    while((pc=strchr(pstr,' '))!=NULL){
        *pc='\0';
        if(strcmp(cur,pstr)==0){
            return 1;
        }
        pstr=pc+1;
    }
    if(strcmp(cur,pstr)==0){
        return 1;
    }
    return 0;
}
char* getname(){
    char* cur=(char*)malloc(200);
    unsigned k=0,l=0;
    while(Txml[readline][k]!='>') k++;
    k+=2;
    while(Txml[readline][k]!='<'||(Txml[readline][k]=='<'&&Txml[readline][k+1]==' ')) cur[l++]=Txml[readline][k++];
    cur[l-1]='\0';
    return cur;
}
unsigned peek_kind(){//0 kw;1 symbol;2 intC;3 StrC;4 ident
    char str[100];
    unsigned k=1,l=0;
    while(Txml[readline][k]!='>') str[l++]=Txml[readline][k++];
    str[l]='\0';
    if(strcmp(str,"keyword")==0) return 0;
    else if(strcmp(str,"symbol")==0) return 1;
    else if(strcmp(str,"integerConstant")==0) return 2;
    else if(strcmp(str,"stringConstant")==0) return 3;
    else return 4;
}