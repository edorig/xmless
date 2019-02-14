/** xmless by Andy Colebourne and Andrew Brooks **/


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <locale.h> 
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/X.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/Form.h>
#include <Xm/SelectioB.h>


#include <stdio.h>
#include <sys/file.h>


/* Important to display UTF-8 characters: we need a font with ISO10646-1 encoding 
   defined in the fallback resources */ 
String fallback_resources[] = {
"*fontList: -misc-fixed-medium-r-normal--14-130-75-75-c-70-iso10646-1",
    "*textwindow.rows:                  24",
    "*textwindow.columns:               80",
    NULL,
};

/* Add some prototypes */ 
void dostdin (FILE*); 
void dofile(FILE*, char*, char*);

Widget toplevel;
int popups;


void killpop(w, c1, c2)
Widget w;
{
    XtUnmanageChild(w);
    XtDestroyWidget(w);
    popups--;
    if (popups == 0)
       exit(0);
}

static XtCallbackRec call_kill[] = { {(XtCallbackProc)killpop, (caddr_t)NULL},
                            { (XtCallbackProc)NULL, (caddr_t)NULL} };


int main(argc,argv)
int argc;
char* argv[];
{
char** arg = argv;
FILE* fp;
XtAppContext app_con;

 XtSetLanguageProc(NULL,NULL,NULL); 
    toplevel = XtAppInitialize(&app_con, "Xbrowser", NULL, 0,
                               &argc, argv, fallback_resources, NULL, 0);
    popups = 0;

    if (argc==1)
        dostdin(stdin);
    else
    {
        while (argc--!=1)
        {
            if (!strcmp(*++arg,"-"))
                dostdin(stdin);
            else
            {
                fp=fopen(*arg,"r");
                if (fp==NULL)
                {
                    fprintf(stderr,"%s: cannot open %s\n",argv[0],*arg);
                    continue;
                }
                dofile(fp, *arg, *arg);
                fclose(fp);
            }
        }
    }

    if (popups != 0 )
        XtAppMainLoop(app_con);
     
}

void dostdin(FILE *fp)
{
    char c;
    char *name=tmpnam(NULL);
    FILE *temp=fopen(name,"w");

    while ((c=getc(fp))!=EOF)
       putc(c, temp);
    fclose(temp);
    temp=fopen(name,"r");
    dofile(temp, name, "stdin");
    remove(name);
    
}

void view_window(char *str, char *title)
{
Widget pop, textwindow, outer;
Arg al[10];
int ac;

    ac = 0;
    XtSetArg(al[ac], XmNunmapCallback, call_kill);  ac++;

    XtSetArg(al[ac], XmNokLabelString, XmStringCreateLtoR("Close", XmFONTLIST_DEFAULT_TAG) ); ac++;
    pop = XmCreatePromptDialog(toplevel, "textpopup", al, ac);
    outer = XtParent(pop);

    XtUnmanageChild(XmSelectionBoxGetChild(pop, XmDIALOG_SELECTION_LABEL) );
    XtUnmanageChild(XmSelectionBoxGetChild(pop, XmDIALOG_TEXT ) );
    XtUnmanageChild(XmSelectionBoxGetChild(pop, XmDIALOG_SEPARATOR ) );
    XtUnmanageChild(XmSelectionBoxGetChild(pop, XmDIALOG_APPLY_BUTTON) );
    XtUnmanageChild(XmSelectionBoxGetChild(pop, XmDIALOG_CANCEL_BUTTON) );
    XtUnmanageChild(XmSelectionBoxGetChild(pop, XmDIALOG_HELP_BUTTON) );

    ac = 0;
    XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
    XtSetArg(al[ac], XmNeditMode, XmMULTI_LINE_EDIT); ac++;
    XtSetArg(al[ac], XmNscrollLeftSide, TRUE); ac++;
    /* This is actually useless: we don't need to define a render table or 
     a fontlist to be able to display UTF-8 characters, we simply need a fallback 
     ISO10646-1 font defined in fallback_resources. 
        XtSetArg(al[ac],XmNrenderTable,NULL); ac++;    
        XtSetArg(al[ac],XmNfontList,XmFONTLIST_DEFAULT_TAG); ac++; */ 
    textwindow = XmCreateScrolledText(pop, "textwindow", al, ac);
    XtManageChild(textwindow);

    ac = 0;

    XtSetArg (al[ac], XmNtitle, title); ac++;
    XtSetValues(outer, al, ac);


    ac = 0;
    XtSetArg(al[ac], XmNdefaultPosition, FALSE);  ac++;
    XtSetValues(XtParent(textwindow), al, ac);

    if (str != NULL)
        XmTextSetString(textwindow, str);

    XtManageChild(pop);
    popups++;
}


void dofile(FILE *fp, char *name, char *title)
{
struct stat etat_fichier;
 int zozo;   
char *string;
int len;
    zozo=stat(name, &etat_fichier);
    if (zozo==0) {
      len = (int) etat_fichier.st_size;
      string = (char *)XtMalloc(len+1);
      fread(string, sizeof(char), len, fp);
      string[len]='\0';
      view_window(string, title);
      XtFree(string);
    } else {
      perror ("xmless: "); 
    }
      
}

