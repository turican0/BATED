#include <iostream>
#include <vector>
//#include <sstream>
#include <string>
#include <functional>
#include <dirent.h>

#include <direct.h> // _getcwd
#include <stdlib.h> // free, perror
#include <stdio.h>  // printf
#include <string.h> // strlen


using namespace std;
//string testcode;

//my code /*
/*
aa //command
//
*/

typedef struct {
    int type;
    string entity;
    string comment;
    long beginEnt;
    long beginLine;
    long beginWord;
    long endEnt;
    long endLine;
    long endWord;
} TypeWord;

typedef struct {
    vector<TypeWord> words;
    long lineIndex;
    string entity;
    string comment;
} TypeLine;

typedef struct TypeTree TypeTree;

struct TypeTree {
    int pre;
    int type;
    long beginLine;
    long beginChar;
    long endLine;
    long endChar;
    vector<struct TypeTree> subTree;
};

typedef struct {
    vector<TypeLine> lines;
    TypeTree subTree;
    string filename;
    string source;
    string tree;
    string comment;
} TypeFile;

//void test    () {};

/*typedef struct {
    vector<TypeLine> lines;
    string entity;
    bool comment;
} TypeCommentEnt;*/

void SetComment(string* testcomment, long begin, long size, int comment) {
    for (int i = begin; i < begin + size; i++)
    {

        if (comment == 1)
            (*testcomment)[i] = 'y';
        else if (comment == 3)
            (*testcomment)[i] = 't';
        else
            (*testcomment)[i] = 'n';
    }
};

void AnalyzeComments(string* testcode, string* testcomment) {
    *testcomment = *testcode;
    int state = 0;
    long index = 0;
    bool runAgain = true;
    while (runAgain)
    {
        if (index == -1)
            break;
        long oldindex = index;
        if (state == 0)
        {
            long testIndex1 = index;
            long testIndex2 = index;
            testIndex1 = testcode->find("//", index);
            testIndex2 = testcode->find("/*", index);
            if (testIndex1 < 0) testIndex1 = 0xfffffff;
            if (testIndex2 < 0) testIndex2 = 0xfffffff;
            if ((testIndex1 == 0xfffffff) && (testIndex2 == 0xfffffff))
            {
                SetComment(testcomment, oldindex, testcode->length() - oldindex, 0);
                break;
            }
            else
            {
                if (testIndex1 < testIndex2)
                {
                    index = testIndex1;
                    SetComment(testcomment, oldindex, index - oldindex, 0);
                    state = 1;
                }
                else
                {
                    index = testIndex2;
                    SetComment(testcomment, oldindex, index - oldindex, 0);
                    state = 2;
                }
            }
        }
        else if (state == 1)
        {
            index = testcode->find("\n", index);
            if (index == -1)
            {
                index = testcode->size() - 1;
                runAgain = false;
            }
            index++;
            SetComment(testcomment, oldindex, index - oldindex, 1);
            state = 0;
        }
        else
        {
            index = testcode->find("*/", index);
            index += 2;
            SetComment(testcomment, oldindex, index - oldindex, 1);
            state = 0;
        }
    }
};

void AnalyzeText(string* testcode, string* testcomment) {
    int state = 0;
    long index = 0;
    bool runAgain = true;
    while (runAgain)
    {
        if (index == -1)
            break;
        long oldindex = index;
        if (state == 0)
        {
            long testIndex2 = index;
            
            bool runAgain2 = true;
            while (runAgain2)
            {
                runAgain2 = false;
                testIndex2 = testcode->find("\"", testIndex2);
                if (testIndex2 > -1)
                {
                    if ((*testcomment)[testIndex2] == 'y')
                    {
                        testIndex2++;
                        runAgain2 = true;
                    }
                    else if ((testIndex2 > 0) && ((*testcode)[testIndex2 - 1] == '\\'))
                    {
                        testIndex2++;
                        runAgain2 = true;
                    }
                }
            }

            if (testIndex2 < 0) testIndex2 = 0xfffffff;
            if (testIndex2 == 0xfffffff)
            {
                //SetComment(testcomment, oldindex, testcode->length() - oldindex, 4);
                break;
            }
            else
            {
                    index = testIndex2;
                    //SetComment(testcomment, oldindex, index - oldindex, 4);
                    index++;
                    state = 2;
            }
        }
        else
        {            
            bool runAgain2 = true;
            while (runAgain2)
            {
                runAgain2 = false;
                index = testcode->find("\"", index);
                if (index > -1)
                {
                    if ((*testcomment)[index] == 'y')
                    {
                        index++;
                        runAgain2 = true;
                    } else if ((index > 0) && ((*testcode)[index - 1] == '\\'))
                    {
                        index++;
                        runAgain2 = true;
                    }
                }
            }
            if (index == -1)
                runAgain = false;
            index++;
            SetComment(testcomment, oldindex - 1, index - oldindex + 1, 3);
            state = 0;
        }
    }
};


void AnalyzeLines(vector<TypeLine>* ent, string* testcode, string* testcomment) {
    long lineIndex = 0;
    bool runAgain = true;
    long index = 0;
    while (runAgain) {
        long oldindex = index;
        index = (*testcode).find("\n", index);
        if (index > -1)
        {
            int cutR = 1;
            if ((*testcode)[index - 1] == '\r')
                cutR = 2;
            TypeLine locLine;
            index++;
            locLine.entity = (*testcode).substr(oldindex, index - oldindex - cutR);
            locLine.comment = (*testcomment).substr(oldindex, index - oldindex - cutR);
            locLine.lineIndex = lineIndex++;
            //if (locLine.entity.length() > 0)
            ent->push_back(locLine);
        }
        else
        {
            TypeLine locLine;
            index = ent->size();
            locLine.entity = (*testcode).substr(oldindex, index - oldindex);
            locLine.comment = (*testcomment).substr(oldindex, index - oldindex);
            locLine.lineIndex = lineIndex;
            //if (locLine.entity.length() > 0)
            ent->push_back(locLine);
            runAgain = false;
        }
    }
};

string prepComm[] = { "#undef","#ifdef","#ifndef","#if","#else","#elif","#endif","#include","#error","#pragma" };
string stdComm[] = { "asm", "auto", "bool", "break", "case", "catch", "char", "class", "const", "const_cast", "continue", "default", "delete", "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", "long", "mutable", "namespace", "new", "operator", "private", "protected", "public", "register", "reinterpret_cast", "return", "short", "signed", "sizeof", "static", "static_cast", "struct", "switch", "template", "this", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while" };
string stdOp[] = { "and", "and_eq", "bitand", "bitor ", "compl", "not", "not_eq ", "or ", "or_eq", "xor", "xor_eq" };

int GetStdType(string word)
{
    size_t i = 0;
    while (!prepComm[i].empty())
    {
        if (!word.compare(prepComm[i]))
            return 5;
        i++;
    }
    i = 0;
    while (!stdComm[i].empty())
    {
        if (!word.compare(stdComm[i]))
            return 4;
        i++;
    }
    i = 0;
    while (!stdOp[i].empty())
    {
        if (!word.compare(stdOp[i]))
            return 3;
        i++;
    }
    if ((word[0] >= '0') && (word[0] <= '9'))
        return 2;

    if (((word[0] >= 'a') && (word[0] <= 'z')) || ((word[0] >= 'A') && (word[0] <= 'Z')) || ((word[0] == '_')))
        return 1;

    return 0;
}

long FindNextWord(string* entity, string* comment, long index, int* state, char* commentSt, bool config) {    
    if (config)
    {
        long actIndex = index;
        while (actIndex < entity->size())
        {
            if ((*comment)[actIndex] == 'n')
                if (((*entity)[actIndex] == '=') && (*state != '=')) {
                    *state = '=';
                    return actIndex;
                }
            //if (!isComment)
            if (((*entity)[actIndex] == '\n') && (*state != '\n')) {
                *state = '\n';
                return actIndex;
            }
            if (((((*entity)[actIndex] >= '0') && ((*entity)[actIndex] <= '9')) ||
                (((*entity)[actIndex] >= 'a') && ((*entity)[actIndex] <= 'z')) ||
                (((*entity)[actIndex] >= 'A') && ((*entity)[actIndex] <= 'Z')) ||
                ((*entity)[actIndex] == '_') || ((*entity)[actIndex] == '#')
                || ((*entity)[actIndex] == '/') || ((*entity)[actIndex] == '\\')) && (*state != 1))
            {
                *state = 1;
                return actIndex;
            }
            if (*commentSt != (*comment)[actIndex])
            {
                *commentSt = (*comment)[actIndex];
                *state = 0;
                return actIndex;
            }
            if (((((*entity)[actIndex] < '0') || ((*entity)[actIndex] > '9')) &&
                (((*entity)[actIndex] < 'a') || ((*entity)[actIndex] > 'z')) &&
                (((*entity)[actIndex] < 'A') || ((*entity)[actIndex] > 'Z')) &&
                ((*entity)[actIndex] != '_') && ((*entity)[actIndex] != '#') &&
                ((*entity)[actIndex] == '/') && ((*entity)[actIndex] == '\\')) &&
                (*state != 0) &&
                ((*entity)[actIndex] != '=') && ((*entity)[actIndex] != '\n'))
            {
                *state = 0;
                return actIndex;
            }
            else
                actIndex++;
        }
    }
    else
    {
        long actIndex = index;
        while (actIndex < entity->size())
        {
            if ((*comment)[actIndex] == 'n')
                if (((*entity)[actIndex] == '(') && (*state != '(')) {
                    *state = '(';
                    return actIndex;
                }
            if ((*comment)[actIndex] == 'n')
                if (((*entity)[actIndex] == ')') && (*state != ')')) {
                    *state = ')';
                    return actIndex;
                }
            if ((*comment)[actIndex] == 'n')
                if (((*entity)[actIndex] == '{') && (*state != '{')) {
                    *state = '{';
                    return actIndex;
                }
            if ((*comment)[actIndex] == 'n')
                if (((*entity)[actIndex] == '}') && (*state != '}')) {
                    *state = '}';
                    return actIndex;
                }
            if ((*comment)[actIndex] == 'n')
                if (((*entity)[actIndex] == ';') && (*state != ';')) {
                    *state = ';';
                    return actIndex;
                }
            if ((*comment)[actIndex] == 'n')
                if (((*entity)[actIndex] == '=') && (*state != '=')) {
                    *state = '=';
                    return actIndex;
                }
            //if (!isComment)
            if (((*entity)[actIndex] == '\n') && (*state != '\n')) {
                *state = '\n';
                return actIndex;
            }
            if (((((*entity)[actIndex] >= '0') && ((*entity)[actIndex] <= '9')) ||
                (((*entity)[actIndex] >= 'a') && ((*entity)[actIndex] <= 'z')) ||
                (((*entity)[actIndex] >= 'A') && ((*entity)[actIndex] <= 'Z')) ||
                ((*entity)[actIndex] == '_') || ((*entity)[actIndex] == '#')) && (*state != 1))
            {
                *state = 1;
                return actIndex;
            }
            if (*commentSt != (*comment)[actIndex])
            {
                *commentSt = (*comment)[actIndex];
                *state = 0;
                return actIndex;
            }
            if (((((*entity)[actIndex] < '0') || ((*entity)[actIndex] > '9')) &&
                (((*entity)[actIndex] < 'a') || ((*entity)[actIndex] > 'z')) &&
                (((*entity)[actIndex] < 'A') || ((*entity)[actIndex] > 'Z')) &&
                ((*entity)[actIndex] != '_') && ((*entity)[actIndex] != '#')) && (*state != 0) &&
                ((*entity)[actIndex] != '(') && ((*entity)[actIndex] != ')') &&
                ((*entity)[actIndex] != '{') && ((*entity)[actIndex] != '}') &&
                ((*entity)[actIndex] != '=') && ((*entity)[actIndex] != ';') &&
                ((*entity)[actIndex] != '\n'))
            {
                *state = 0;
                return actIndex;
            }
            else
                actIndex++;
        }
    }
    return -1;
}

void AnalyzeWords(vector<TypeLine>* ent, bool config) {
    for (int j = 0; j < ent->size(); j++)
    {
        TypeLine* actLine = &(*ent)[j];
        bool runAgain = true;
        long index = 0;
        int state = -1;
        char commentSt = actLine->comment[0];
        while (runAgain) {
            long oldindex = index;
            index = FindNextWord(&actLine->entity, &actLine->comment, index, &state, &commentSt, config);
            if (index == -1)
            {
                runAgain = false;
                index = actLine->entity.size();
            }                
            TypeWord locWord;
            locWord.entity = actLine->entity.substr(oldindex, index - oldindex);
            locWord.comment = actLine->comment.substr(oldindex, index - oldindex);
            locWord.type = GetStdType(locWord.entity);
            if (locWord.entity.length() > 0)
                actLine->words.push_back(locWord);
        }
    }
}

bool ExistWord(vector<TypeLine>* ent, long j, long k)
{
    if ((j < 0) || (j >= (*ent).size()))
        return false;
    if ((k < 0) || (k >= (*ent)[j].words.size()))
        return false;
    return true;
}

void GetPreIndex(vector<TypeLine>* ent, long* j, long* k) {
    (*k)--;
    if (ExistWord(ent, (*j), (*k)))
        return;
    else
    {
        (*j)--;
        (*k) = 0;
        if (ExistWord(ent, (*j), (*k)))
        {
            (*k) = (*ent)[(*j)].words.size() - 1;
            if (ExistWord(ent, (*j), (*k)))
                return;
        }
    }
    (*j) = -1;
    (*k) = -1;
}

void GetPostIndex(vector<TypeLine>* ent, long* j, long* k) {
    (*k)++;
    if (ExistWord(ent, (*j), (*k)))
        return;
    else
    {
        (*j)++;
        (*k) = 0;
        if (ExistWord(ent, (*j), (*k)))
            return;
    }
    (*j) = -1;
    (*k) = -1;
}

void PrintEntity(vector<TypeLine>* ent, TypeWord word, string preStr, string postStr) {
    long j = word.beginLine;
    long k = word.beginWord;
    cout << preStr;
    cout << (*ent)[j].words[k].entity;
    do
    {
        GetPostIndex(ent, &j, &k);
        cout << (*ent)[j].words[k].entity;
    } while ((j > -1) && ((j != word.endLine) || (k != word.endWord)));
    cout << postStr;
};

bool WordComment(string word) {
    for (int i = 0; i < word.size(); i++)
        if (word[i] == 'y')
            return true;
    return false;
}

bool PrevLineComment(vector<TypeLine>* ent, long* j, long* k)
{
    long endTempJ = *j;
    long endTempK = *k;
    long beginTempJ = *j;
    long beginTempK = *k;
    long tempJ = *j;
    long tempK = *k;
    bool runAgain = true;
    bool isComment = false;
    do
    {
        tempJ = beginTempJ;
        tempK = beginTempK;
        GetPreIndex(ent, &beginTempJ, &beginTempK);
        if (beginTempJ > -1)
            if ((*ent)[endTempJ].lineIndex - (*ent)[beginTempJ].lineIndex == 2)
                runAgain = false;
    } while ((runAgain) && (beginTempJ > -1));
    if (beginTempJ == -1)
    {
        beginTempJ = tempJ;
        beginTempK = tempK;
    }
    else
        GetPostIndex(ent, &beginTempJ, &beginTempK);
    if ((*ent)[endTempJ].lineIndex == (*ent)[beginTempJ].lineIndex)
        return false;
    beginTempK = 0;
    do
    {
        if (((*ent)[beginTempJ].words[beginTempK].type != 0) && !(WordComment((*ent)[beginTempJ].words[beginTempK].comment)))
            return false;
        if (WordComment((*ent)[beginTempJ].words[beginTempK].comment))
            isComment = true;
        GetPostIndex(ent, &beginTempJ, &beginTempK);
    } while ((beginTempJ > -1) && ((beginTempJ != endTempJ) || (beginTempK != endTempK)));
    if (!isComment) return false;
    return true;
}

void SetPrevLine(vector<TypeLine>* ent, long* j, long* k)
{
    long tempJ = *j;
    long tempK = *k;
    GetPreIndex(ent, &tempJ, &tempK);
    if (tempJ < 0)
        return;
    *j = tempJ;
    *k = 0;
}

bool PostLineComment(vector<TypeLine>* ent, long* j, long* k)
{
    long endTempJ = *j;
    long endTempK = *k;
    long beginTempJ = *j;
    long beginTempK = *k;
    long tempJ = *j;
    long tempK = *k;
    bool runAgain = true;
    bool isComment = false;
    do
    {
        tempJ = endTempJ;
        tempK = endTempK;
        GetPostIndex(ent, &endTempJ, &endTempK);
        if (endTempJ > -1)
            if ((*ent)[endTempJ].lineIndex - (*ent)[beginTempJ].lineIndex == 2)
                runAgain = false;
    } while ((runAgain) && (endTempJ > -1));
    if (endTempJ == -1)
    {
        endTempJ = tempJ;
        endTempK = tempK;
    }
    else
        GetPreIndex(ent, &endTempJ, &endTempK);
    if ((*ent)[beginTempJ].lineIndex == (*ent)[endTempJ].lineIndex)
        return false;
    do
    {
        GetPostIndex(ent, &beginTempJ, &beginTempK);
        if (beginTempJ == -1)
            break;
        if (((*ent)[beginTempJ].words[beginTempK].type != 0) && !(WordComment((*ent)[beginTempJ].words[beginTempK].comment)))
            return false;
        if (WordComment((*ent)[beginTempJ].words[beginTempK].comment))
            isComment = true;
    } while ((beginTempJ > -1) && ((beginTempJ != endTempJ) || (beginTempK != endTempK)));
    if (!isComment) return false;
    return true;
}

bool PostLineEmpty(vector<TypeLine>* ent, long* j, long* k)
{
    long endTempJ = *j;
    long endTempK = *k;
    long beginTempJ = *j;
    long beginTempK = *k;
    long tempJ = *j;
    long tempK = *k;
    bool runAgain = true;
    //bool isComment = false;
    do
    {
        tempJ = endTempJ;
        tempK = endTempK;
        GetPostIndex(ent, &endTempJ, &endTempK);
        if (endTempJ > -1)
            if ((*ent)[endTempJ].lineIndex - (*ent)[beginTempJ].lineIndex == 2)
                runAgain = false;
    } while ((runAgain) && (endTempJ > -1));
    if (endTempJ == -1)
    {
        endTempJ = tempJ;
        endTempK = tempK;
    }
    else
        GetPreIndex(ent, &endTempJ, &endTempK);
    if ((*ent)[beginTempJ].lineIndex == (*ent)[endTempJ].lineIndex)
        return false;
    do
    {
        GetPostIndex(ent, &beginTempJ, &beginTempK);
        if (beginTempJ == -1)
            break;
        if (((*ent)[beginTempJ].words[beginTempK].type != 0) && !(WordComment((*ent)[beginTempJ].words[beginTempK].comment)))
            return false;
        //if (WordComment((*ent)[beginTempJ].words[beginTempK].comment))
        //    isComment = true;
        //GetPostIndex(ent, &beginTempJ, &beginTempK);
    } while ((beginTempJ > -1) && ((beginTempJ != endTempJ) || (beginTempK != endTempK)));
    //if (!isComment) return false;
    return true;
}

void SetPostLine(vector<TypeLine>* ent, long* j, long* k)
{
    long tempJ = *j;
    long tempK = *k;
    do
    {
        GetPostIndex(ent, &tempJ, &tempK);
        if (tempJ == -1)
            return;
    } while ((*ent)[*j].lineIndex == (*ent)[tempJ].lineIndex);
    *j = tempJ;
    *k = (*ent)[tempJ].words.size()-1;
}

long lastEndProcLine = -1;
long lastEndProcWord = -1;

void SetProcedureInfo(vector<TypeLine>* ent, long j, long k) {

    long preJ = j;
    long preK = k;
    long preJ2 = j;
    long preK2 = k;

    bool test;
    bool test2;
    bool setType = false;
    do
    {
        preJ2 = preJ;
        preK2 = preK;
        if ((*ent)[preJ].words[preK].entity == ")")
            return;
        if ((*ent)[preJ].words[preK].entity == "}")
            return;
        if ((*ent)[preJ].words[preK].entity == "#define")
            return;
        if (((*ent)[preJ].words[preK].type != 0) && ((preJ != j) || (preK != k)))
            setType = true;
        GetPreIndex(ent, &preJ, &preK);
    } while ((preJ > -1) && (((*ent)[preJ].words[preK].entity != "\n") && (((*ent)[preJ].words[preK].entity != ";") || WordComment((*ent)[preJ].words[preK].comment))));
    if (!setType)
        return;
    long postJ = j;
    long postK = k;
    long postJ2 = j;
    long postK2 = k;
    int procState = 0;
    bool isProc = false;
    bool isDec = false;
    bool runAgain = true;
    do
    {
        postJ2 = postJ;
        postK2 = postK;

        if (((postJ != j) || (postK != k)) && (procState == 0) && ((*ent)[postJ].words[postK].type == 1))
            break;

        if (((postJ != j) || (postK != k)) && (procState == 0) && ((*ent)[postJ].words[postK].type == 2))
            break;

        if (((postJ != j) || (postK != k)) && (procState == 0) && ((*ent)[postJ].words[postK].entity == "="))
            break;

        if ((procState == 0) && (((*ent)[postJ].words[postK].entity == "(") && !WordComment((*ent)[postJ].words[postK].comment)))
        {
            procState++;
        }
        if ((procState == 0) && (((*ent)[postJ].words[postK].entity == ")") && !WordComment((*ent)[postJ].words[postK].comment)))
        {
            break;
        }
        if ((procState == 1) && (((*ent)[postJ].words[postK].entity == ")") && !WordComment((*ent)[postJ].words[postK].comment)))
        {
            procState++;
        }
        if ((procState == 2) && (((*ent)[postJ].words[postK].entity == ";") && !WordComment((*ent)[postJ].words[postK].comment)))
        {
            isDec = true;
            runAgain = false;
            break;
        }
        if ((procState < 3) && (((*ent)[postJ].words[postK].entity == "}") && !WordComment((*ent)[postJ].words[postK].comment)))
        {
            break;
        }
        if ((procState < 2) && (((*ent)[postJ].words[postK].entity == "{") && !WordComment((*ent)[postJ].words[postK].comment)))
        {
            break;
        }
        if (procState >= 2)
        {
            if ((procState) && (((*ent)[postJ].words[postK].entity == "{") && !WordComment((*ent)[postJ].words[postK].comment)))
            {
                procState++;
                isProc = true;
            }
        }
        if (procState >= 3)
        {
            if ((procState) && (((*ent)[postJ].words[postK].entity == "}") && !WordComment((*ent)[postJ].words[postK].comment)))
            {
                procState--;
                if (procState == 2)
                {
                    runAgain = false;
                    break;
                }
            }
        }
        GetPostIndex(ent, &postJ, &postK);
    } while ((postJ > -1) && runAgain);

    long tempJ = postJ;
    long tempK = postK;
    long tempJ2 = postJ2;
    long tempK2 = postK2;

    if ((isDec)&&(!isProc))
    {
        runAgain = true;//get end line
        do {
            postJ2 = postJ;
            postK2 = postK;
            if ((*ent)[postJ].words[postK].entity == "\n")
            {
                runAgain = false;
            }
            if ((((*ent)[postJ].words[postK].type != 0) && !WordComment((*ent)[postJ].words[postK].comment)))
            {
                postJ = tempJ;
                postK = tempK;
                postJ2 = tempJ2;
                postK2 = tempK2;
                runAgain = false;
            }
            GetPostIndex(ent, &postJ, &postK);
        } while ((runAgain) && (postJ > -1));

        (*ent)[j].words[k].type = 10;
        (*ent)[j].words[k].beginLine = preJ2;
        (*ent)[j].words[k].beginWord = preK2;
        (*ent)[j].words[k].endLine = postJ2;
        (*ent)[j].words[k].endWord = postK2;
        lastEndProcLine = postJ2;
        lastEndProcWord = postK2;
    }

    if (isProc)
    {
        runAgain = true;//get ;
        do {
            postJ2 = postJ;
            postK2 = postK;
            if ((((*ent)[postJ].words[postK].entity == ";") && !WordComment((*ent)[postJ].words[postK].comment)))
            {
                runAgain = false;
            }
            if ((((*ent)[postJ].words[postK].type != 0) && !WordComment((*ent)[postJ].words[postK].comment)))
            {
                postJ = tempJ;
                postK = tempK;
                postJ2 = tempJ2;
                postK2 = tempK2;
                runAgain = false;
                break;
            }
            GetPostIndex(ent, &postJ, &postK);
        } while ((runAgain) && (postJ > -1));

        if (postJ == -1)
        {
            postJ = tempJ;
            postK = tempK;
            postJ2 = tempJ2;
            postK2 = tempK2;
        }

        runAgain = true;//get end line
        do {
            postJ2 = postJ;
            postK2 = postK;
            if ((*ent)[postJ].words[postK].entity == "\n")
            {
                runAgain = false;
                break;
            }
            if ((((*ent)[postJ].words[postK].type != 0) && !WordComment((*ent)[postJ].words[postK].comment)))
            {
                postJ = tempJ;
                postK = tempK;
                postJ2 = tempJ2;
                postK2 = tempK2;
                runAgain = false;
                break;
            }
            GetPostIndex(ent, &postJ, &postK);
        } while ((runAgain) && (postJ > -1));

        if ((*ent)[preJ2].lineIndex != (*ent)[postJ2].lineIndex)//not inline
        {
            while (PrevLineComment(ent, &preJ2, &preK2))
                SetPrevLine(ent, &preJ2, &preK2);
            while (PostLineComment(ent, &postJ2, &postK2))
                SetPostLine(ent, &postJ2, &postK2);
            if (PostLineEmpty(ent, &postJ2, &postK2))
                SetPostLine(ent, &postJ2, &postK2);
        }

        (*ent)[j].words[k].type = 20;
        (*ent)[j].words[k].beginLine = preJ2;
        (*ent)[j].words[k].beginWord = preK2;
        (*ent)[j].words[k].endLine = postJ2;
        (*ent)[j].words[k].endWord = postK2;
        lastEndProcLine = postJ2;
        lastEndProcWord = postK2;
    }

    /*if (isDec)
        PrintEntity(ent, (*ent)[j].words[k], "Dec: |", "|\n");
    if ((isProc) && (!isProc))
        PrintEntity(ent, (*ent)[j].words[k], "Proc: |", "|\n");*/
};

void AnalyzeProcedures(vector<TypeLine>* ent) {
    lastEndProcLine = -1;
    lastEndProcWord = -1;
    for (int j = 0; j < ent->size(); j++)
    {
        TypeLine* actLine = &(*ent)[j];
        for (int k = 0; k < actLine->words.size(); k++)
        {
            if (actLine->words[k].type == 1)
            {
                if(actLine->words[k].comment[0] == 'n')
                    if ((j > lastEndProcLine) || ((j == lastEndProcLine) && (k > lastEndProcWord)))
                    {
                        if ((actLine->words[k].entity == "or")&&(j==5))
                        {
                            int ss = 0;
                            ss++;
                        }
                        SetProcedureInfo(ent, j, k);
                    }
            }
        }
    }
}
/*
char getNext(string* tree, long* index) {
    while (true)
    {
        if (((*tree)[*index] >= 'A') || ((*tree)[*index] <= 'Z')) break;
        if (((*tree)[*index] >= 'a') || ((*tree)[*index] <= 'z')) break;
        if (((*tree)[*index] >= '0') || ((*tree)[*index] <= '9')) break;
        if ((*tree)[*index] == '(') break;
        if ((*tree)[*index] == ')') break;
        if ((*tree)[*index] == '-') break;
        if ((*tree)[*index] == ',') break;
        if ((*tree)[*index] == '[') break;
        if ((*tree)[*index] == ']') break;
        (*index)++;
    }
    return((*tree)[*index]);
}*/

long ReadNumber(string* tree, long* index) {
    long value=0;
    while (true)
    {
        if (((*tree)[*index] < '0') || ((*tree)[*index] > '9')) break;
        value *= 10;
        value += (*tree)[*index] - '0';
        (*index)++;
    }
    return value;
}

void AnalyzeBranch(string* source, string* tree, TypeTree* subTree, long* indexT) {    
    while (true)
    {
        if ((*tree)[*indexT] == '(') break;
        if (*indexT >= (*tree).size()) return;
        //if (((*tree)[*indexT] != '\n') && ((*tree)[*indexT] != ' ')) break;
        (*indexT)++;
    }
    if ((*tree)[*indexT] == ')') { (*indexT)++; return; }
    if ((*tree)[*indexT] == '(')
    {
        (*indexT)++;
        long beginWord = *indexT;
        while (true)
        {
            if ((((*tree)[*indexT] < 'A') || ((*tree)[*indexT] > 'Z')) &&
                (((*tree)[*indexT] < 'a') || ((*tree)[*indexT] > 'z')) &&
                ((*tree)[*indexT] != '_')) break;
            (*indexT)++;
        }
        string testStr = (*tree).substr(beginWord, *indexT - beginWord);
        subTree->type = -1;
        if (testStr == "translation_unit")
            subTree->type = 1;
        else if (testStr == "ERROR")
            subTree->type = 2;
        else if (testStr == "identifier")
            subTree->type = 3;
        else if (testStr == "preproc_include")
            subTree->type = 4;
        else if (testStr == "string_literal")
            subTree->type = 5;
        else if (testStr == "preproc_call")
            subTree->type = 6;
        else if (testStr == "preproc_directive")
            subTree->type = 7;
        else if (testStr == "declaration")
            subTree->type = 8;
        else if (testStr == "type_identifier")
            subTree->type = 9;
        else if (testStr == "init_declarator")
            subTree->type = 10;
        else if (testStr == "number_literal")
            subTree->type = 11;
        else if (testStr == "comment")
            subTree->type = 12;
        else if (testStr == "primitive_type")
            subTree->type = 13;
        else if (testStr == "pointer_declarator")
            subTree->type = 14;
        else if (testStr == "array_declarator")
            subTree->type = 15;
        else if (testStr == "function_definition")
            subTree->type = 16;
        else if (testStr == "function_declarator")
            subTree->type = 17;
        else if (testStr == "parameter_list")
            subTree->type = 18;
        else if (testStr == "parameter_declaration")
            subTree->type = 19;
        else if (testStr == "parameter_dcompound_statementeclaration")
            subTree->type = 20;
        else if (testStr == "expression_statement")
            subTree->type = 21;
        else if (testStr == "assignment_expression")
            subTree->type = 22;
        else if (testStr == "compound_statement")
            subTree->type = 23;
        else if (testStr == "cast_expression")
            subTree->type = 24;
        else if (testStr == "type_descriptor")
            subTree->type = 25;
        else if (testStr == "abstract_pointer_declarator")
            subTree->type = 26;
        else if (testStr == "call_expression")
            subTree->type = 27;
        else if (testStr == "qualified_identifier")
            subTree->type = 28;
        else if (testStr == "namespace_identifier")
            subTree->type = 29;
        else if (testStr == "argument_list")
            subTree->type = 30;
        else if (testStr == "if_statement")
            subTree->type = 31;
        else if (testStr == "condition_clause")
            subTree->type = 32;
        else if (testStr == "pointer_expression")
            subTree->type = 33;
        else if (testStr == "subscript_expression")
            subTree->type = 34;
        else if (testStr == "do_statement")
            subTree->type = 35;
        else if (testStr == "break_statement")
            subTree->type = 36;
        else if (testStr == "binary_expression")
            subTree->type = 37;
        else if (testStr == "update_expression")
            subTree->type = 38;
        else if (testStr == "parenthesized_expression")
            subTree->type = 39;
        else if (testStr == "while_statement")
            subTree->type = 40;
        else if (testStr == "field_expression")
            subTree->type = 41;
        else if (testStr == "field_identifier")
            subTree->type = 42;
        else if (testStr == "switch_statement")
            subTree->type = 43;
        else if (testStr == "case_statement")
            subTree->type = 44;
        else if (testStr == "char_literal")
            subTree->type = 45;
        else if (testStr == "escape_sequence")
            subTree->type = 46;
        else if (testStr == "false")
            subTree->type = 47;
        else if (testStr == "true")
            subTree->type = 48;
        else if (testStr == "type_definition")
            subTree->type = 49;
        else if (testStr == "struct_specifier")
            subTree->type = 50;
        else if (testStr == "field_declaration_list")
            subTree->type = 51;
        else if (testStr == "field_declaration")
            subTree->type = 52;
        else if (testStr == "return_statement")
            subTree->type = 53;
        else if (testStr == "for_statement")
            subTree->type = 54;
        else if (testStr == "sizeof_expression")
            subTree->type = 55;
        else if (testStr == "unary_expression")
            subTree->type = 56;
        else if (testStr == "optional_parameter_declaration")
            subTree->type = 57;
        else if (testStr == "template_function")
            subTree->type = 58;
        else if (testStr == "template_argument_list")
            subTree->type = 59;
        else if (testStr == "null")
            subTree->type = 60;
        else if (testStr == "continue_statement")
            subTree->type = 61;
        else if (testStr == "initializer_list")
            subTree->type = 62;
        else if (testStr == "type_qualifier")
            subTree->type = 63;
        else if (testStr == "compound_literal_expression")
            subTree->type = 64;
        else if (testStr == "preproc_if")
            subTree->type = 65;
        else if (testStr == "preproc_else")
            subTree->type = 66;
        else if (testStr == "preproc_arg")
            subTree->type = 67;
        else if (testStr == "preproc_ifdef")
            subTree->type = 68;
        else if (testStr == "preproc_def")
            subTree->type = 69;
        else if (testStr == "using_declaration")
            subTree->type = 70;
        else if (testStr == "system_lib_string")
            subTree->type = 71;
        else if (testStr == "parenthesized_declarator")
            subTree->type = 72;
        else if (testStr == "sized_type_specifier")
            subTree->type = 73;
        else if (testStr == "goto_statement")
            subTree->type = 74;
        else if (testStr == "statement_identifier")
            subTree->type = 75;
        else if (testStr == "labeled_statement")
            subTree->type = 76;
        else if (testStr == "comma_expression")
            subTree->type = 77;
        else if (testStr == "abstract_function_declarator")
            subTree->type = 78;
        else if (testStr == "abstract_parenthesized_declarator")
            subTree->type = 79;
        else if (testStr == "preproc_function_def")
            subTree->type = 80;
        else if (testStr == "preproc_params")
            subTree->type = 81;
        else if (testStr == "user_defined_literal")
            subTree->type = 82;
        else if (testStr == "literal_suffix")
            subTree->type = 83;
        else if (testStr == "storage_class_specifier")
            subTree->type = 84;
        else if (testStr == "template_type")
            subTree->type = 85;
        else if (testStr == "nullptr")
            subTree->type = 86;
        else if (testStr == "conditional_expression")
            subTree->type = 87;
        else if (testStr == "reference_declarator")
            subTree->type = 88;
        else if (testStr == "preproc_defined")
            subTree->type = 89;
        else if (testStr == "this")
            subTree->type = 90;
        else if (testStr == "auto")
            subTree->type = 91;
        else if (testStr == "lambda_expression")
            subTree->type = 92;
        else if (testStr == "lambda_capture_specifier")
            subTree->type = 93;
        else if (testStr == "class_specifier")
            subTree->type = 94;
        else if (testStr == "access_specifier")
            subTree->type = 95;
        else if (testStr == "default_method_clause")
            subTree->type = 96;
        else if (testStr == "preproc_elif")
            subTree->type = 97;
        else if (testStr == "template_declaration")
            subTree->type = 98;
        else if (testStr == "template_parameter_list")
            subTree->type = 99;
        else if (testStr == "type_parameter_declaration")
            subTree->type = 100;
        if(subTree->type == -1)
            {
                if (testStr == "throw_statement")
                    subTree->type = 101;
                else if (testStr == "new_expression")
                    subTree->type = 102;
                else if (testStr == "new_declarator")
                    subTree->type = 103;
                else if (testStr == "base_class_clause")
                    subTree->type = 104;
                else if (testStr == "union_specifier")
                    subTree->type = 105;
                else if (testStr == "enum_specifier")
                    subTree->type = 106;
                else if (testStr == "enumerator_list")
                    subTree->type = 107;
                else if (testStr == "enumerator")
                    subTree->type = 108;
                else if (testStr == "destructor_name")
                    subTree->type = 109;
                else if (testStr == "field_initializer_list")
                    subTree->type = 110;
                else if (testStr == "field_initializer")
                    subTree->type = 111;
                else if (testStr == "for_range_loop")
                    subTree->type = 112;
                else if (testStr == "delete_expression")
                    subTree->type = 113;
                else if (testStr == "virtual_function_specifier")
                    subTree->type = 114;
                else if (testStr == "template_method")
                    subTree->type = 115;
                else if (testStr == "ms_call_modifier")
                    subTree->type = 116;
                else if (testStr == "linkage_specification")
                    subTree->type = 117;
                else if (testStr == "declaration_list")
                    subTree->type = 118;
                else if (testStr == "concatenated_string")
                    subTree->type = 119;
                else if (testStr == "ms_declspec_modifier")
                    subTree->type = 120;
                else if (testStr == "attribute_specifier")
                    subTree->type = 121;
                else if (testStr == "subscript_designator")
                    subTree->type = 122;
                else if (testStr == "abstract_array_declarator")
                    subTree->type = 123;
                else if (testStr == "MISSING")
                    subTree->type = 124;
                else if (testStr == "namespace_definition")
                    subTree->type = 125;
                else if (testStr == "try_statement")
                    subTree->type = 126;
                else if (testStr == "catch_clause")
                    subTree->type = 127;
                else if (testStr == "operator_name")
                    subTree->type = 128;
                else
                {
                    printf("\nerror %s not found\n", testStr.c_str());
                    subTree->type = 200;
                }
            }
        (*indexT) += 2;
        subTree->beginLine = ReadNumber(tree, indexT);
        (*indexT) += 2;
        subTree->beginChar = ReadNumber(tree, indexT);
        (*indexT) += 5;
        subTree->endLine = ReadNumber(tree, indexT);
        (*indexT) += 2;
        subTree->endChar = ReadNumber(tree, indexT);
        (*indexT)++;
        if ((*tree)[*indexT] == ')')
        {
            (*indexT)++;
            return;
        }
        (*indexT) += 3;
        if ((*tree)[*indexT] != '(')
        {
            (*indexT) += 2;
            while (true)
            {
                if ((((*tree)[*indexT] < 'A') || ((*tree)[*indexT] > 'Z')) &&
                    (((*tree)[*indexT] < 'a') || ((*tree)[*indexT] > 'z')) &&
                    ((*tree)[*indexT] != '_') && ((*tree)[*indexT] != ':')) break;
                (*indexT)++;
            }
            (*indexT)++;
        }
        while (true)
        {
            if (*indexT >= (*tree).size()) return;
            TypeTree nextTree;
            AnalyzeBranch(source, tree, &nextTree, indexT);
            subTree->subTree.push_back(nextTree);
            if ((*tree)[*indexT] == ')')
            {
                break;
            }
        }
        (*indexT)++;
    }
};

void AnalyzeTree(TypeFile* typeFile) {
    long indexT = 0;
    AnalyzeBranch(&typeFile->source, &typeFile->tree, &typeFile->subTree, &indexT);
}

void AnalyzeFile(TypeFile* typeFile) {
    char* buffer;

    // Get the current working directory:
    if ((buffer = _getcwd(NULL, 0)) == NULL)
    {
        perror("_getcwd error");
    }
    else
    {
        //printf("%s \nLength: %zu\n", buffer, strlen(buffer));
        char buffer2[512];
        sprintf_s(buffer2, "%s\\tree-sitter.exe", buffer);
        FILE* testf;
        fopen_s(&testf, buffer2, "r");
        if (!testf)
        {
            //fclose(testf);
            sprintf_s(buffer2, "%s\\..\\DEBUG\\tree-sitter.exe", buffer);
            FILE* testf;
            fopen_s(&testf,buffer2, "r");
            if (!testf)exit(-1);
            fclose(testf);
        }
        char buffer3[512];
        //sprintf_s(buffer3, "%s parse c:\\prenos\\BATED\\Debug\\remc2.cpp > c:\\prenos\\BATED\\Debug\\remc2.cpp.tree", buffer2, typeFile->filename.c_str(), typeFile->filename.c_str());
        sprintf_s(buffer3, "%s parse %s > %s.tree", buffer2, typeFile->filename.c_str(), typeFile->filename.c_str());
        //BATED/BATED
        //BATED/DEBUG
        system(buffer3);
        //system("pause");
        free(buffer);

        //testcode = "  \n // \n/**/ /* */\n int main()\n {\n}\n //xx\n //yyy\n \n                ";
//string testcomment;
        FILE* fptr_file;
        fopen_s(&fptr_file, typeFile->filename.c_str(), "rb");
        fseek(fptr_file, 0L, SEEK_END);
        long sz_file = ftell(fptr_file);
        fseek(fptr_file, 0L, SEEK_SET);
        char* content_buffer = (char*)malloc(sz_file * sizeof(char*));
        fread(content_buffer, sz_file, 1, fptr_file);
        fclose(fptr_file);
        typeFile->source.assign(content_buffer, sz_file);
        free(content_buffer);

        sprintf_s(buffer3,"%s.tree", typeFile->filename.c_str());

        FILE* fptr_file_tree;
        fopen_s(&fptr_file_tree, buffer3, "rb");
        fseek(fptr_file_tree, 0L, SEEK_END);
        long sz_file_tree = ftell(fptr_file_tree);
        fseek(fptr_file_tree, 0L, SEEK_SET);
        char* content_buffer_tree = (char*)malloc(sz_file_tree * sizeof(char*));
        fread(content_buffer_tree, sz_file_tree, 1, fptr_file_tree);
        fclose(fptr_file_tree);
        typeFile->tree.assign(content_buffer_tree, sz_file_tree);
        free(content_buffer_tree);

        AnalyzeTree(typeFile);
        printf("File: %s imported\n", typeFile->filename.c_str());

        typeFile->comment = typeFile->source;
        AnalyzeLines(&typeFile->lines, &typeFile->source, &typeFile->comment);
    }    

    //AnalyzeComments(&typeFile->source, &typeFile->comment);
    //AnalyzeText(&typeFile->source, &typeFile->comment);
    //AnalyzeLines(&typeFile->lines, &typeFile->source, &typeFile->comment);
    //AnalyzeWords(&typeFile->lines, false);
    //AnalyzeProcedures(&typeFile->lines);
}

long FindNextConfigWord(TypeLine* line, long* index)
{
    while ((*line).words.size() > *index)
    {
        if ((*line).words[*index].type != 0)
            return *index;
        (*index)++;
    }
    return -1;
}

string inPath;
string outPath;

void GetReqDirs(const std::string& path, std::vector<string>& files, const bool showHiddenDirs = false) {
    DIR* dpdf;
    struct dirent* epdf;
    dpdf = opendir(path.c_str());
    if (dpdf != NULL) {
        while ((epdf = readdir(dpdf)) != NULL) {
            if (showHiddenDirs ? (epdf->d_type == DT_DIR && string(epdf->d_name) != ".." && string(epdf->d_name) != ".") : (epdf->d_type == DT_DIR && strstr(epdf->d_name, "..") == NULL && strstr(epdf->d_name, ".") == NULL)) {
                GetReqDirs(path + epdf->d_name + "/", files, showHiddenDirs);
            }
            if (epdf->d_type == DT_REG) {
                files.push_back(path + epdf->d_name);
            }
        }
    }
    closedir(dpdf);
}

void listFiles(const std::string& path, std::function<void(const std::string&)> cb) {
    if (auto dir = opendir(path.c_str())) {
        while (auto f = readdir(dir)) {
            if (!f->d_name || f->d_name[0] == '.') continue;
            if (f->d_type == DT_DIR)
                listFiles(path + f->d_name + "/", cb);

            if (f->d_type == DT_REG)
                cb(path + f->d_name);
        }
        closedir(dir);
    }
}

void AnalyzeDir(vector<TypeFile>* locDir, string inPath) {
    std::vector<string> files;
    GetReqDirs(inPath, files);
    for (string actFile : files)
    {
        if ((actFile.substr(actFile.length() - 2, 2) == ".h") ||
            (actFile.substr(actFile.length() - 4, 4) == ".cpp"))
        {
            TypeFile typefile;
            typefile.filename = actFile;
            AnalyzeFile(&typefile);
            locDir->push_back(typefile);
        }
    }
};

typedef struct {
    string filename;
    long lineIndex;
    string word;
} TypeDeclarations;

bool FindDecl(vector<TypeDeclarations>* declarations, string* actWord, string* tempFile, long* tempLineIndex)
{
    for (TypeDeclarations actDecl : *declarations)
    {
        if (actDecl.word == *actWord)
        {
            *tempFile = actDecl.filename;
            *tempLineIndex = actDecl.lineIndex;
            return true;
        }
    }
    return false;
}

const int Type_identifier = 3;
const int Type_declaration = 8;
const int Type_function_declarator = 17;
const int Type_storage_class_specifier = 84;

void FindDuplicity(vector<TypeDeclarations>* declarations, TypeTree* actTree, vector<TypeLine>* lines, string* filename) {
    if (actTree->type == Type_declaration)
    {
            for (TypeTree actTree2 : actTree->subTree)
            {
                if (actTree2.type == Type_function_declarator)
                {
                    for (TypeTree actTree3 : actTree2.subTree)
                        if (actTree3.type == Type_identifier)
                        {
                            long tempLineIndex;
                            string tempFile;
                            string entity = (*lines)[actTree3.beginLine].entity.substr(actTree3.beginChar, actTree3.endChar - actTree3.beginChar);
                            if (FindDecl(declarations, &entity, &tempFile, &tempLineIndex))
                            {
                                //printf("Duplicate declaration: %s", actFile.filename.c_str());
                                //printf("%s", actWord.entity);

                                printf("Duplicate declaration: %s %s : %d Prev: %s : %d\n", entity.c_str(), (*filename).c_str(), actTree2.beginLine, tempFile.c_str(), tempLineIndex);

                            }
                            else
                            {
                                TypeDeclarations locDeclaration;
                                locDeclaration.filename = *filename;
                                locDeclaration.lineIndex = actTree2.beginLine;
                                locDeclaration.word = entity;
                                (*declarations).push_back(locDeclaration);
                            }
                        }
                }
            }
    }
    for (TypeTree actTree : actTree->subTree)
    {
        FindDuplicity(declarations , &actTree, lines, filename);
    }
    
    /*
    for (TypeLine actLine : actFile.lines) {
        for (TypeWord actWord : actLine.words) {
            if (actWord.type == 10) {
                long tempLineIndex;
                string tempFile;
                if (FindDecl(&declarations, &actWord.entity, &tempFile, &tempLineIndex))
                {
                    //printf("Duplicate declaration: %s", actFile.filename.c_str());
                    //printf("%s", actWord.entity);

                    printf("Duplicate declaration: %s %s : %d Prev: %s : %d\n", actWord.entity.c_str(), actFile.filename.c_str(), actLine.lineIndex, tempFile.c_str(), tempLineIndex);

                }
                else
                {
                    TypeDeclarations locDeclaration;
                    locDeclaration.filename = actFile.filename;
                    locDeclaration.lineIndex = actLine.lineIndex;
                    locDeclaration.word = actWord;
                    declarations.push_back(locDeclaration);
                }
            }
        }
    }*/
};

void MakeConfigCommand(TypeLine* line, long* index)
{
    vector<TypeFile> locDir;
    if (!(*line).words[*index].entity.compare("IN"))
    {
        (*index)++;
        if (FindNextConfigWord(line, index) > -1)
            inPath = (*line).words[*index].entity;
    }
    else if (!(*line).words[*index].entity.compare("OUT"))
    {
        (*index)++;
        if (FindNextConfigWord(line, index) > -1)
            outPath = (*line).words[*index].entity;
    }
    else if (!(*line).words[*index].entity.compare("FDR"))
    {
        AnalyzeDir(&locDir, inPath);
        vector<TypeDeclarations> declarations;
        
        for (TypeFile actFile : locDir) {
            FindDuplicity(&declarations, &actFile.subTree, &actFile.lines, &actFile.filename);
        }
    }
}

int main(int argc, char** argv)
{
    std::string configStr;
    vector<TypeLine> config;

    if (argc < 2)
    {
        printf("using:\bBATED script.bated\n\n");
        printf("commands:\nIN=path\nOUT=path\nREN - rename\nMOV - move\n");
        printf("FDR - find references\nINS - insert\nDEL - delete\n");
    }
    else
    {
        FILE* fptr_script;
        fopen_s(&fptr_script, argv[1], "rb");
        fseek(fptr_script, 0L, SEEK_END);
        long sz_script = ftell(fptr_script);
        fseek(fptr_script, 0L, SEEK_SET);
        char* content_script = (char*)malloc(sz_script * sizeof(char*));
        fread(content_script, sz_script, 1, fptr_script);
        fclose(fptr_script);
        configStr.assign(content_script, sz_script);
        free(content_script);

        string configcomment;
        AnalyzeComments(&configStr, &configcomment);
        AnalyzeText(&configStr, &configcomment);
        AnalyzeLines(&config, &configStr, &configcomment);
        AnalyzeWords(&config, true);

        for (long lineIndex = 0; lineIndex < config.size(); lineIndex++)
        {
            long wordIndex = 0;
            if (FindNextConfigWord(&config[lineIndex], &wordIndex) > -1)
                MakeConfigCommand(&config[lineIndex], &wordIndex);
        }
        //AnalyzeFile();
    }
};


