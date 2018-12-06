#include "mainwindow.h"
#include "widget.h"
#include <QApplication>
#include "gui_main.h"

int gui_main()
{
    char *az[2];int d=0;
    az[1]=(char *)"";
    QApplication a((int&)d, az);
    MainWindow w;
    w.show();
    Widget w1;
    w1.show();

    return a.exec();
}
void* gui_thread_func(void *arg)//(int argc, char *argv[])
{
    gui_main();
}
void gui_thread_create()
{
   pthread_t thread;
    //创建线程，并把NULL作为线程函数的参数 
    if(pthread_create(&thread, NULL, gui_thread_func, NULL) != 0) { 
        perror("pthread_create failed\n"); 
        exit(EXIT_FAILURE); 
    }
}

