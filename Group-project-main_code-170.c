
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 20
#define DAYS 5
#define SLOTS 9   // 8�5

// ---------------- STRUCTS ----------------

typedef struct {
    char name[50];
    char code[10];
} Teacher;

typedef struct {
    char name[50];
    char code[10];
    int capacity;
    char type[10];   // theory/lab
} Classroom;

typedef struct {
    char title[50];
    char code[10];
    int duration;
    char roomType[10];
    char teacherCode[10];   // FIXED teacher
} Course;

typedef struct {
    char name[10];
    int students;
    char homeRoom[10];
    char courseCodes[10][10];
    int courseCount;
} Section;

typedef struct {
    int day;
    int slot;
    int duration;
    char section[10];
    char course[10];
    char teacher[10];
    char room[10];
} Routine;

// ---------------- GLOBAL DATA ----------------

Teacher teachers[MAX];
Classroom rooms[MAX];
Course courses[MAX];
Section sections[MAX];
Routine routine[200];

int tCount=0, rCount=0, cCount=0, sCount=0, rc=0;

int teacherBusy[MAX][DAYS][SLOTS];
int roomBusy[MAX][DAYS][SLOTS];
int sectionBusy[MAX][DAYS][SLOTS];

// ---------------- HELPERS ----------------

int findTeacher(char *code){
    for(int i=0;i<tCount;i++)
        if(strcmp(teachers[i].code,code)==0) return i;
    return -1;
}

int findRoom(char *code){
    for(int i=0;i<rCount;i++)
        if(strcmp(rooms[i].code,code)==0) return i;
    return -1;
}

int findCourse(char *code){
    for(int i=0;i<cCount;i++)
        if(strcmp(courses[i].code,code)==0) return i;
    return -1;
}

// ---------------- INPUT ----------------

void addTeacher(){
    printf("Teacher Name: ");
    scanf(" %[^\n]", teachers[tCount].name);
    printf("Teacher Code: ");
    scanf("%s", teachers[tCount].code);
    tCount++;
}

void addRoom(){
    printf("Room Name: ");
    scanf(" %[^\n]", rooms[rCount].name);
    printf("Room Code: ");
    scanf("%s", rooms[rCount].code);
    printf("Capacity: ");
    scanf("%d",&rooms[rCount].capacity);
    printf("Type (theory/lab): ");
    scanf("%s",rooms[rCount].type);
    rCount++;
}

void addCourse(){
    printf("Course Title: ");
    scanf(" %[^\n]", courses[cCount].title);
    printf("Course Code: ");
    scanf("%s", courses[cCount].code);
    printf("Duration (1-3): ");
    scanf("%d",&courses[cCount].duration);
    printf("Room Type: ");
    scanf("%s", courses[cCount].roomType);
    printf("Assigned Teacher Code: ");
    scanf("%s", courses[cCount].teacherCode);
    cCount++;
}

void addSection(){
    printf("Section Name: ");
    scanf("%s", sections[sCount].name);
    printf("Students: ");
    scanf("%d",&sections[sCount].students);
    printf("Home Room Code: ");
    scanf("%s", sections[sCount].homeRoom);

    printf("Number of Courses: ");
    scanf("%d",&sections[sCount].courseCount);

    for(int i=0;i<sections[sCount].courseCount;i++){
        printf("Course %d Code: ",i+1);
        scanf("%s",sections[sCount].courseCodes[i]);
    }
    sCount++;
}

// ---------------- CORE SCHEDULER ----------------

int isFree(int t,int r,int s,int d,int sl,int dur){
    for(int i=0;i<dur;i++)
        if(teacherBusy[t][d][sl+i]||roomBusy[r][d][sl+i]||sectionBusy[s][d][sl+i])
            return 0;
    return 1;
}

void markBusy(int t,int r,int s,int d,int sl,int dur){
    for(int i=0;i<dur;i++){
        teacherBusy[t][d][sl+i]=1;
        roomBusy[r][d][sl+i]=1;
        sectionBusy[s][d][sl+i]=1;
    }
}

void generateRoutine(){
    memset(teacherBusy,0,sizeof(teacherBusy));
    memset(roomBusy,0,sizeof(roomBusy));
    memset(sectionBusy,0,sizeof(sectionBusy));
    rc=0;

    for(int s=0;s<sCount;s++){
        for(int i=0;i<sections[s].courseCount;i++){
            int c=findCourse(sections[s].courseCodes[i]);
            int t=findTeacher(courses[c].teacherCode);
            int r=findRoom(sections[s].homeRoom);

            for(int d=0;d<DAYS;d++){
                for(int sl=0;sl<=SLOTS-courses[c].duration;sl++){
                    if(isFree(t,r,s,d,sl,courses[c].duration)){
                        routine[rc++] = (Routine){d,sl,courses[c].duration,
                            "", "", "", ""};
                        strcpy(routine[rc-1].section,sections[s].name);
                        strcpy(routine[rc-1].course,courses[c].code);
                        strcpy(routine[rc-1].teacher,courses[c].teacherCode);
                        strcpy(routine[rc-1].room,sections[s].homeRoom);
                        markBusy(t,r,s,d,sl,courses[c].duration);
                        goto nextCourse;
                    }
                }
            }
            nextCourse:;
        }
    }
}

// ---------------- TABLE OUTPUT ----------------

void printSectionPOV(){
    char sec[10];
    printf("Section: ");
    scanf("%s",sec);

    printf("\nTIME  Mon Tue Wed Thu Fri\n");
    for(int sl=0;sl<SLOTS;sl++){
        printf("%02d-%02d ",sl+8,sl+9);
        for(int d=0;d<DAYS;d++){
            int found=0;
            for(int i=0;i<rc;i++){
                if(strcmp(routine[i].section,sec)==0 &&
                   routine[i].day==d &&
                   routine[i].slot==sl){
                    printf("%-4s ",routine[i].course);
                    found=1;
                }
            }
            if(!found) printf("---- ");
        }
        printf("\n");
    }
}

void printTeacherPOV(){
    char t[10];
    printf("Teacher Code: ");
    scanf("%s",t);

    printf("\nTIME  Mon Tue Wed Thu Fri\n");
    for(int sl=0;sl<SLOTS;sl++){
        printf("%02d-%02d ",sl+8,sl+9);
        for(int d=0;d<DAYS;d++){
            int found=0;
            for(int i=0;i<rc;i++){
                if(strcmp(routine[i].teacher,t)==0 &&
                   routine[i].day==d &&
                   routine[i].slot==sl){
                    printf("%-4s ",routine[i].section);
                    found=1;
                }
            }
            if(!found) printf("---- ");
        }
        printf("\n");
    }
}

// ---------------- MENU ----------------

int main(){
    int ch;
    while(1){
        printf("\n1.Add Teacher\n2.Add Room\n3.Add Course\n4.Add Section\n");
        printf("5.Generate Routine\n6.Section POV\n7.Teacher POV\n8.Exit\n");
        scanf("%d",&ch);

        if(ch==1) addTeacher();
        else if(ch==2) addRoom();
        else if(ch==3) addCourse();
        else if(ch==4) addSection();
        else if(ch==5) generateRoutine();
        else if(ch==6) printSectionPOV();
        else if(ch==7) printTeacherPOV();
        else break;
    }
}
