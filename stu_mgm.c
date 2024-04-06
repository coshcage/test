/* cosh.cage#hotmail.com
 * Public Domain. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "StoneValley/src/svstring.h"

static const char * db_filename = "stu_database.dat";

typedef struct tag_StudentInfo
{
    int id;
    char name[16];
    float chinese;
    float math;
    float english;
} StudentInfo, * PStudentInfo;

ARRAY_Z g_db = { 0 }; // Data base.

// Input students' credits.
void InputDegrees(PStudentInfo pinfo)
{
    int i;
    do
    {
        printf("Input student ID:%d. %s's credits by class.\n\t[1].Chinese.\n\t[2].Math\n\t[3].English\n\t[0].Return to upper menu.\n? ", pinfo->id, pinfo->name);
        scanf_s("%d", &i);
        switch (i)
        {
        case 1:
            printf("Chinese:? ");
            scanf_s("%f", &pinfo->chinese);
            break;
        case 2:
            printf("Math:? ");
            scanf_s("%f", &pinfo->math);
            break;
        case 3:
            printf("English:? ");
            scanf_s("%f", &pinfo->english);
            break;
        }
    } while (i);
}

int LinearSearchDBByID(void * pitem, size_t param)
{
    if (((PStudentInfo)pitem)->id == (int)0[((size_t *)param)])
        return CBF_TERMINATE;
    ++1[((size_t *)param)];
    return CBF_CONTINUE;
}

void InputStudentIDAndName(PStudentInfo pinfo)
{
    size_t a[2];
    StudentInfo infot = { 0 };
    printf("Input student ID and name.\n");
Lbl_Input_ID:
    printf("Student ID:? ");
    scanf_s("%d", &infot.id);
    a[0] = infot.id;
    a[1] = 0;
    if (CBF_TERMINATE == strTraverseArrayZ(&g_db, sizeof infot, LinearSearchDBByID, (size_t)a, FALSE))
    {
        printf("Student ID should be unique.\n");
        goto Lbl_Input_ID;
    }
    printf("Student name:? ");
    scanf_s("%s", infot.name, 15);
    memcpy(pinfo, &infot, sizeof infot);
}

void PrintStudentInfo(PStudentInfo pinfo)
{
    printf("%d\t%s\t%.2f\t%.2f\t%.2f\n", pinfo->id, pinfo->name, pinfo->chinese, pinfo->math, pinfo->english);
}

PStudentInfo FindStudentByID(int id)
{
    size_t a[2];
    a[0] = id;
    a[1] = 0;
    if (CBF_TERMINATE == strTraverseArrayZ(&g_db, sizeof(StudentInfo), LinearSearchDBByID, (size_t)a, FALSE))
        return strLocateItemArrayZ(&g_db, sizeof(StudentInfo), a[1]);
    return NULL;
}

int LinearSearchDBByName(void * pitem, size_t param)
{
    if (strcmp(((PStudentInfo)pitem)->name, (const char *)0[((size_t *)param)]) == 0)
    {
        PrintStudentInfo(pitem);
        ++1[((size_t *)param)];
    }
    return CBF_CONTINUE;
}

int LinearSearchDBByScore(void * pitem, size_t param)
{
    float x, y, z;
    x = *(float *)0[((size_t *)param)];
    y = *(float *)1[((size_t *)param)];

    switch (3[((size_t *)param)])
    {
    case 3: // Chinese.
        z = ((PStudentInfo)pitem)->chinese;
        if (z >= x && z <= y)
        {
            PrintStudentInfo(pitem);
            ++2[((size_t *)param)];
        }
        break;
    case 4: // Math.
        z = ((PStudentInfo)pitem)->math;
        if (z >= x && z <= y)
        {
            PrintStudentInfo(pitem);
            ++2[((size_t *)param)];
        }
        break;
    case 5: // English.
        z = ((PStudentInfo)pitem)->english;
        if (z >= x && z <= y)
        {
            PrintStudentInfo(pitem);
            ++2[((size_t *)param)];
        }
        break;
    }
    return CBF_CONTINUE;
}

void FindStudent(void)
{
    size_t a[4];
    int i, id;
    char buffer[16] = { 0 };
    PStudentInfo pinfo;
    float x, y;
    do
    {
        printf("Find student.\n\t[1].By ID.\n\t[2].By name.\n\t[3].By Chinese score.\n\t[4].By Math score.\n\t[5].By English score.\n\t[0].Return to upper menu.\n? ");
        scanf_s("%d", &i);
        switch (i)
        {
        case 1:
            printf("Input student ID:? ");
            scanf_s("%d", &id);
            pinfo = FindStudentByID(id);
            if (pinfo)
            {
                printf("ID\tName\tChinese\tMath\tEnglish\n");
                PrintStudentInfo(pinfo);
            }
            else
                printf("Can not find student ID:%d.\n", id);
            break;
        case 2:
            printf("Input student name:? ");
            scanf_s("%s", buffer, 15);
            a[0] = (size_t)buffer;
            a[1] = 0;
            printf("ID\tName\tChinese\tMath\tEnglish\n");
            strTraverseArrayZ(&g_db, sizeof(StudentInfo), LinearSearchDBByName, (size_t)a, FALSE);
            printf("%zd student(s) found.\n", a[1]);
            break;
        case 3:
        case 4:
        case 5:
            printf("Input lower bound for score:[x,y] x? ");
            scanf_s("%f", &x);
            printf("Input upper bound for score:[x,y] y? ");
            scanf_s("%f", &y);
            a[0] = (size_t)&x;
            a[1] = (size_t)&y;
            a[2] = 0;
            a[3] = i;
            printf("ID\tName\tChinese\tMath\tEnglish\n");
            strTraverseArrayZ(&g_db, sizeof(StudentInfo), LinearSearchDBByScore, (size_t)a, FALSE);
            printf("%zd student(s) found.\n", a[2]);
            break;
        }
    } while (i);
}

int ListStudentsInfo(void * pitem, size_t param)
{
    PrintStudentInfo(pitem);
    ++0[((size_t *)param)];
    return CBF_CONTINUE;
}

int cmp_student_id(const void * px, const void * py)
{
    return ((PStudentInfo)px)->id - ((PStudentInfo)py)->id;
}

int cmp_student_name(const void * px, const void * py)
{
    return strcmp(((PStudentInfo)px)->name, ((PStudentInfo)py)->name);
}

int cmp_student_chinese(const void * px, const void * py)
{
#define CLASS chinese
    if (((PStudentInfo)px)->CLASS > ((PStudentInfo)py)->CLASS)
        return 1;
    else if (((PStudentInfo)px)->CLASS < ((PStudentInfo)py)->CLASS)
        return -1;
    return 0;
#undef CLASS
}

int cmp_student_math(const void * px, const void * py)
{
#define CLASS math
    if (((PStudentInfo)px)->CLASS > ((PStudentInfo)py)->CLASS)
        return 1;
    else if (((PStudentInfo)px)->CLASS < ((PStudentInfo)py)->CLASS)
        return -1;
    return 0;
#undef CLASS
}

int cmp_student_english(const void * px, const void * py)
{
#define CLASS english
    if (((PStudentInfo)px)->CLASS > ((PStudentInfo)py)->CLASS)
        return 1;
    else if (((PStudentInfo)px)->CLASS < ((PStudentInfo)py)->CLASS)
        return -1;
    return 0;
#undef CLASS
}

int cmp_student_total(const void * px, const void * py)
{
    float x, y;
    x = ((PStudentInfo)px)->chinese + ((PStudentInfo)px)->math + ((PStudentInfo)px)->english;
    y = ((PStudentInfo)py)->chinese + ((PStudentInfo)py)->math + ((PStudentInfo)py)->english;
    if (x > y)
        return 1;
    else if (x < y)
        return -1;
    return 0;
}

void SortStudentInfo(void)
{
    int i;
    size_t a[2] = { 0 };
    do
    {
        printf("Sort student information.\n\t[1].Sort student by ID.\n\t[2].Sort students by name.\n\t[3].Sort students Chinese score.\n\t[4].Sort students by Math score.\n\t[5].Sort students by English score.\n\t[6].Sort students by total score.\n\t[0].Return to upper menu.\n? ");
        scanf_s("%d", &i);
        switch (i)
        {
        case 1: // By ID.
            strSortArrayZ(&g_db, sizeof(StudentInfo), cmp_student_id);
            break;
        case 2: // By name.
            strSortArrayZ(&g_db, sizeof(StudentInfo), cmp_student_name);
            break;
        case 3: // By Chinese.
            strSortArrayZ(&g_db, sizeof(StudentInfo), cmp_student_chinese);
            break;
        case 4: // By Math.
            strSortArrayZ(&g_db, sizeof(StudentInfo), cmp_student_math);
            break;
        case 5: // By English.
            strSortArrayZ(&g_db, sizeof(StudentInfo), cmp_student_english);
            break;
        case 6: // By total score.
            strSortArrayZ(&g_db, sizeof(StudentInfo), cmp_student_total);
            break;
        }
        a[0] = 0;
        printf("ID\tName\tChinese\tMath\tEnglish\n");
        strTraverseArrayZ(&g_db, sizeof(StudentInfo), ListStudentsInfo, (size_t)a, FALSE);
        printf("%zd student(s) enlisted.\n", a[0]);
    } while (i);
}

int ComputeStatistics(void * pitem, size_t param)
{
    PStudentInfo pinfo = (PStudentInfo)pitem;
    float * chinese_total = (float *)0[(size_t *)param];
    float * chinese_max = (float *)1[(size_t *)param];
    float * chinese_min = (float *)2[(size_t *)param];
    float * math_total = (float *)3[(size_t *)param];
    float * math_max = (float *)4[(size_t *)param];
    float * math_min = (float *)5[(size_t *)param];
    float * english_total = (float *)6[(size_t *)param];
    float * english_max = (float *)7[(size_t *)param];
    float * english_min = (float *)8[(size_t *)param];

    (*chinese_total) += pinfo->chinese;
    (*math_total) += pinfo->math;
    (*english_total) += pinfo->english;

    if (*chinese_max < pinfo->chinese)
        *chinese_max = pinfo->chinese;
    if (*math_max < pinfo->math)
        *math_max = pinfo->math;
    if (*english_max < pinfo->english)
        *english_max = pinfo->english;

    if (*chinese_min > pinfo->chinese)
        *chinese_min = pinfo->chinese;
    if (*math_min > pinfo->math)
        *math_min = pinfo->math;
    if (*english_min > pinfo->english)
        *english_min = pinfo->english;

    ++9[(size_t *)param];
    return CBF_CONTINUE;
}

void Statistics(void)
{
    int i;
    float f[10] = { 0.0 };
    size_t a[10];
    f[2] = f[5] = f[8] = FLT_MAX;
    f[1] = f[4] = f[7] = FLT_MIN;
    for (i = 0; i < 9; ++i)
        a[i] = (size_t)(f + i);
    a[9] = 0;
    printf("Statistics for average, maximum and minimum score of three classes:\n");
    strTraverseArrayZ(&g_db, sizeof(StudentInfo), ComputeStatistics, (size_t)a, FALSE);
    printf("Chinese:\n\tAvg\tMax\tMin\n\t%0.2f\t%0.2f\t%0.2f\n", f[0] / (float)a[9], f[1], f[2]);
    printf("Math:\n\tAvg\tMax\tMin\n\t%0.2f\t%0.2f\t%0.2f\n", f[3] / (float)a[9], f[4], f[5]);
    printf("English:\n\tAvg\tMax\tMin\n\t%0.2f\t%0.2f\t%0.2f\n", f[6] / (float)a[9], f[7], f[8]);
}

void ReadDataBaseFile(void)
{
    StudentInfo si;
    FILE * fp;
    fopen_s(&fp, db_filename, "rb");
    if (fp)
    {
        while (!feof(fp))
        {
            fread(&si, sizeof si, 1, fp);
            if (!feof(fp))
                if (strResizeArrayZ(&g_db, strLevelArrayZ(&g_db) + 1, sizeof si))
                    strInsertItemArrayZ(&g_db, &si, sizeof si, strLevelArrayZ(&g_db) - 1);
        }
        fclose(fp);
    }
}

int CBFWriteDataBase(void * pitem, size_t param)
{
    fwrite(pitem, sizeof(StudentInfo), 1, (FILE *)param);
    return CBF_CONTINUE;
}

void WriteDataBaseFile(void)
{
    FILE * fp;
    fopen_s(&fp, db_filename, "wb");
    if (fp)
    {
        strTraverseArrayZ(&g_db, sizeof(StudentInfo), CBFWriteDataBase, (size_t)fp, FALSE);
        fclose(fp);
    }
}

int main(void)
{
    int i, id;
    size_t a[2];
    StudentInfo si = { 0 }, * pinfo;
    ReadDataBaseFile();
    do
    {
        printf("Student Information Management System v%s.\n", LIB_VER);
        printf("[1].Insert student.\n");
        printf("[2].Remove student.\n");
        printf("[3].List all students.\n");
        printf("[4].Find student.\n");
        printf("[5].Alter student's information.\n");
        printf("[6].Sort student information.\n");
        printf("[7].Statistics.\n");
        printf("[0].Exit.\n? ");
        scanf_s("%d", &i);
        switch (i)
        {
        case 1:
            InputStudentIDAndName(&si);
            InputDegrees(&si);
            if (strResizeArrayZ(&g_db, strLevelArrayZ(&g_db) + 1, sizeof si) && strInsertItemArrayZ(&g_db, &si, sizeof si, strLevelArrayZ(&g_db) - 1))
            {
                printf("Insertion succeeded!\n");
                PrintStudentInfo(&si);
            }
            else
                printf("Insertion failed!\n");
            break;
        case 2:
            printf("Input student ID:? ");
            scanf_s("%d", &id);
            a[0] = id;
            a[1] = 0;
            if (CBF_TERMINATE == strTraverseArrayZ(&g_db, sizeof(StudentInfo), LinearSearchDBByID, (size_t)a, FALSE))
            {
                strRemoveItemArrayZ(&g_db, sizeof si, a[1], TRUE);
                printf("Student information has been deleted!\n");
            }
            else
                printf("Can not find student ID:%d.\n", id);
            break;
        case 3:
            a[0] = 0;
            printf("ID\tName\tChinese\tMath\tEnglish\n");
            strTraverseArrayZ(&g_db, sizeof(StudentInfo), ListStudentsInfo, (size_t)a, FALSE);
            printf("%zd student(s) enlisted.\n", a[0]);
            break;
        case 4:
            FindStudent();
            break;
        case 5:
            printf("Input student ID:? ");
            scanf_s("%d", &id);
            a[0] = id;
            a[1] = 0;
            if (CBF_TERMINATE == strTraverseArrayZ(&g_db, sizeof(StudentInfo), LinearSearchDBByID, (size_t)a, FALSE))
            {
                pinfo = (PStudentInfo)strLocateItemArrayZ(&g_db, sizeof si, a[1]);
                InputStudentIDAndName(pinfo);
                InputDegrees(pinfo);
                printf("Student information has been successfully altered!\n");
            }
            else
                printf("Can not find student ID:%d.\n", id);
            break;
        case 6:
            SortStudentInfo();
            break;
        case 7:
            Statistics();
            break;
        }
    } while (i);
    WriteDataBaseFile();
    PrintStudentInfo(&si);
    strFreeArrayZ(&g_db);
    return 0;
}
