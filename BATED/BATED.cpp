#include <iostream>
#include <vector>

using namespace std;
string testcode;

//my code /*
/*
aa //command
//
*/

typedef struct {
    int type;
    string entity;
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
} TypeLine;

//void test    () {};

typedef struct {
    vector<TypeLine> lines;
    string entity;
    bool comment;
} TypeCommentEnt;

vector<TypeCommentEnt> ent;

void AnalyzeComments(vector<TypeCommentEnt>* ent, string testcode) {
    int state = 0;
    long index = 0;
    bool runAgain = true;
    //index = testcode.find("//", index + 1);
    while (runAgain)
    {
        if (index == -1)
            break;
        long oldindex = index;
        if (state == 0)
        {
            long testIndex1 = index;
            long testIndex2 = index;
            testIndex1 = testcode.find("//", index);
            testIndex2 = testcode.find("/*", index);
            if (testIndex1 < 0) testIndex1 = 0xfffffff;
            if (testIndex2 < 0) testIndex2 = 0xfffffff;
            if ((testIndex1 == 0xfffffff) && (testIndex2 == 0xfffffff))
            {
                TypeCommentEnt locEnt;
                locEnt.entity = testcode.substr(oldindex, testcode.length() - oldindex);
                locEnt.comment = false;
                if (locEnt.entity.length() > 0)
                    ent->push_back(locEnt);
                break;
            }
            else
            {
                if (testIndex1 < testIndex2)
                {
                    index = testIndex1;
                    TypeCommentEnt locEnt;
                    locEnt.entity = testcode.substr(oldindex, index - oldindex);
                    locEnt.comment = false;
                    if (locEnt.entity.length() > 0)
                        ent->push_back(locEnt);
                    state = 1;
                }
                else
                {
                    index = testIndex2;
                    TypeCommentEnt locEnt;
                    locEnt.entity = testcode.substr(oldindex, index - oldindex);
                    locEnt.comment = false;
                    if (locEnt.entity.length() > 0)
                        ent->push_back(locEnt);
                    state = 2;
                }
            }
        }
        else if (state == 1)
        {
            index = testcode.find("\n", index);
            TypeCommentEnt locEnt;
            index++;
            locEnt.entity = testcode.substr(oldindex, index - oldindex);
            locEnt.comment = true;
            if (locEnt.entity.length() > 0)
                ent->push_back(locEnt);
            state = 0;
        }
        else
        {
            index = testcode.find("*/", index);
            TypeCommentEnt locEnt;
            index+=2;
            if(testcode[index]=='\n')
                index++;
            locEnt.entity = testcode.substr(oldindex, index - oldindex);
            locEnt.comment = true;
            if (locEnt.entity.length() > 0)
                ent->push_back(locEnt);
            state = 0;
        }
    }
};

void AnalyzeLines(vector<TypeCommentEnt>* ent) {
    long lineIndex = 0;
    for (int i = 0; i < ent->size(); i++)
    {
        TypeCommentEnt* actEnt = &(*ent)[i];
        bool runAgain = true;
        long index = 0;
        while (runAgain) {
            long oldindex = index;
            index = actEnt->entity.find("\n", index);
            if (index > -1)
            {
                TypeLine locLine;
                index++;
                locLine.entity = actEnt->entity.substr(oldindex, index - oldindex);
                locLine.lineIndex = lineIndex++;
                if (locLine.entity.length() > 0)
                    actEnt->lines.push_back(locLine);
            }
            else
            {
                TypeLine locLine;
                index = actEnt->entity.size();
                locLine.entity = actEnt->entity.substr(oldindex, index - oldindex);
                locLine.lineIndex = lineIndex;
                if (locLine.entity.length() > 0)
                    actEnt->lines.push_back(locLine);
                runAgain = false;
            }
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

    if (((word[0] >= 'a') && (word[0] <= 'z'))|| ((word[0] >= 'A') && (word[0] <= 'Z')))
        return 1;

    return 0;
}

long FindNextWord(string entity, long index, bool isComment, int* state) {
    long actIndex = index;
    while (actIndex < entity.size())
    {
            if (!isComment)
                if ((entity[actIndex] == '(') && (*state != '(')) {
                    *state = '(';
                    return actIndex;
                }
            if (!isComment)
                if ((entity[actIndex] == ')') && (*state != ')')) {
                    *state = ')';
                    return actIndex;
                }
            if (!isComment)
                if ((entity[actIndex] == '{') && (*state != '{')) {
                    *state = '{';
                    return actIndex;
                }
            if (!isComment)
                if ((entity[actIndex] == '}') && (*state != '}')) {
                    *state = '}';
                    return actIndex;
                }
            if (!isComment)
                if ((entity[actIndex] == ';') && (*state != ';')) {
                    *state = ';';
                    return actIndex;
                }
            if (!isComment)
                if ((entity[actIndex] == '\n') && (*state != '\n')) {
                    *state = '\n';
                    return actIndex;
                }
            if ((((entity[actIndex] >= '0') && (entity[actIndex] <= '9')) ||
                ((entity[actIndex] >= 'a') && (entity[actIndex] <= 'z')) ||
                ((entity[actIndex] >= 'A') && (entity[actIndex] <= 'Z')) ||
                (entity[actIndex] == '_') || (entity[actIndex] == '#')) && (*state != 1))
            {
                *state = 1;
                return actIndex;
            }
            if ((((entity[actIndex] < '0') || (entity[actIndex] > '9')) &&
                ((entity[actIndex] < 'a') || (entity[actIndex] > 'z')) &&
                ((entity[actIndex] < 'A') || (entity[actIndex] > 'Z')) &&
                (entity[actIndex] != '_') && (entity[actIndex] != '#')) && (*state != 0)&&
                (entity[actIndex] != '(') && (entity[actIndex] != ')') &&
                (entity[actIndex] != '{') && (entity[actIndex] != '}') &&
                (entity[actIndex] != ';') && (entity[actIndex] != '\n'))
            {
                *state = 0;
                return actIndex;
            }
        else
            actIndex++;
    }
    return -1;
}

void AnalyzeWords(vector<TypeCommentEnt>* ent) {
    bool single = false;
    for (int i = 0; i < ent->size(); i++)
    {
        TypeCommentEnt* actEnt = &(*ent)[i];
        for (int j = 0; j < actEnt->lines.size(); j++)
        {
            TypeLine* actLine = &(*actEnt).lines[j];
            bool runAgain = true;
            long index = 0;
            int state = -1;
            while (runAgain) {
                long oldindex = index;
                index = FindNextWord(actLine->entity, index, actEnt->comment, &state);
                if (index == -1)
                {
                    runAgain = false;
                    index = actLine->entity.size();
                }                
                TypeWord locWord;
                locWord.entity = actLine->entity.substr(oldindex, index - oldindex);
                locWord.type = GetStdType(locWord.entity);
                if (locWord.entity.length() > 0)
                    actLine->words.push_back(locWord);
            }
        }
    }
}


bool ExistWord(vector<TypeCommentEnt>* ent, long i, long j, long k)
{
    if ((i < 0) || (i >= ent->size()))
        return false;
    if ((j < 0) || (j >= (*ent)[i].lines.size()))
        return false;
    if ((k < 0) || (k >= (*ent)[i].lines[j].words.size()))
        return false;
    return true;
}

void GetPreIndex(vector<TypeCommentEnt>* ent, long* i, long* j, long* k) {
    (*k)--;
    if (ExistWord(ent, (*i), (*j), (*k)))
        return;
    else
    {
        (*j)--;
        (*k) = 0;
        if (ExistWord(ent, (*i), (*j), (*k)))
        {
            (*k) = (*ent)[(*i)].lines[(*j)].words.size() - 1;
            if (ExistWord(ent, (*i), (*j), (*k)))
                return;
        }
        else
        {
            (*i)--;
            (*j) = 0;
            if (ExistWord(ent, (*i), (*j), (*k)))
            {
                (*j) = (*ent)[(*i)].lines.size() - 1;
                if (ExistWord(ent, (*i), (*j), (*k)))
                    return;
            }
        }
    }
    (*i) = -1;
    (*j) = -1;
    (*k) = -1;
}

void GetPostIndex(vector<TypeCommentEnt>* ent, long* i, long* j, long* k) {
    (*k)++;
    if (ExistWord(ent, (*i), (*j), (*k)))
        return;
    else
    {
        (*j)++;
        (*k) = 0;
        if (ExistWord(ent, (*i), (*j), (*k)))
        {
            //(*k) = (*ent)[(*i)].lines[(*j)].words.size() - 1;
            if (ExistWord(ent, (*i), (*j), (*k)))
                return;
        }
        else
        {
            (*i)++;
            (*j) = 0;
            if (ExistWord(ent, (*i), (*j), (*k)))
            {
                //(*j) = (*ent)[(*i)].lines.size() - 1;
                if (ExistWord(ent, (*i), (*j), (*k)))
                    return;
            }
        }
    }
    (*i) = -1;
    (*j) = -1;
    (*k) = -1;
}

void SetProcedureInfo(vector<TypeCommentEnt>* ent, long i, long j, long k) {
    long preI = i;
    long preJ = j;
    long preK = k;
    long preI2 = i;
    long preJ2 = j;
    long preK2 = k;

    bool test;
    bool test2;
    do
    {
        preI2 = preI;
        preJ2 = preJ;
        preK2 = preK;

        GetPreIndex(ent, &preI, &preJ, &preK);
    } while ((preI > -1) && (((*ent)[preI].lines[preJ].words[preK].entity != "\n") && (((*ent)[preI].lines[preJ].words[preK].entity != ";") || (*ent)[preI].comment)));

    long postI = i;
    long postJ = j;
    long postK = k;
    long postI2 = i;
    long postJ2 = j;
    long postK2 = k;
    int procState = 0;
    bool isProc = false;
    bool isDec = false;
    bool runAgain = true;
    do
    {
        postI2 = postI;
        postJ2 = postJ;
        postK2 = postK;

        if ((procState == 0) && (((*ent)[postI].lines[postJ].words[postK].entity == "(") && !(*ent)[postI].comment))
        {
            procState++;
            isDec = true;
        }
        if ((procState == 1) && (((*ent)[postI].lines[postJ].words[postK].entity == ")") && !(*ent)[postI].comment))
        {
            procState++;
        }
        if ((procState == 2) && (((*ent)[postI].lines[postJ].words[postK].entity == ";") && !(*ent)[postI].comment))
        {
            runAgain = false;
        }
        if (procState >= 2)
        {
            if ((procState) && (((*ent)[postI].lines[postJ].words[postK].entity == "{") && !(*ent)[postI].comment))
            {
                procState++;
                isProc = true;
            }
            if ((procState) && (((*ent)[postI].lines[postJ].words[postK].entity == "}") && !(*ent)[postI].comment))
            {
                procState--;
                if(procState == 2)
                    runAgain = false;
            }
        }
        GetPostIndex(ent, &postI, &postJ, &postK);
    } while ((postI > -1) && runAgain);
    if (isDec)
    {
        (*ent)[postI].lines[postJ].words[postK].type = 10;
        (*ent)[postI].lines[postJ].words[postK].beginEnt = preI2;
        (*ent)[postI].lines[postJ].words[postK].beginLine = preJ2;
        (*ent)[postI].lines[postJ].words[postK].beginWord = preK2;
        (*ent)[postI].lines[postJ].words[postK].endEnt = postI2;
        (*ent)[postI].lines[postJ].words[postK].endLine = postJ2;
        (*ent)[postI].lines[postJ].words[postK].endWord = postK2;
    }
    if (isProc)
    {
        (*ent)[postI].lines[postJ].words[postK].type = 20;
        (*ent)[postI].lines[postJ].words[postK].beginEnt = preI2;
        (*ent)[postI].lines[postJ].words[postK].beginLine = preJ2;
        (*ent)[postI].lines[postJ].words[postK].beginWord = preK2;
        (*ent)[postI].lines[postJ].words[postK].endEnt = postI2;
        (*ent)[postI].lines[postJ].words[postK].endLine = postJ2;
        (*ent)[postI].lines[postJ].words[postK].endWord = postK2;
    }
};

void AnalyzeProcedures(vector<TypeCommentEnt>* ent) {
    for (int i = 0; i < ent->size(); i++)
    {
        TypeCommentEnt* actEnt = &(*ent)[i];
        for (int j = 0; j < actEnt->lines.size(); j++)
        {
            if (!actEnt->comment)
            {
                TypeLine* actLine = &(*actEnt).lines[j];
                for (int k = 0; k < actLine->words.size(); k++)
                {
                    if (actLine->words[k].type == 1)
                    {
                        SetProcedureInfo(ent, i, j, k);
                    }
                }
            }
        }
    }
}

int main()
{
    testcode = "int main() {}\n";

    AnalyzeComments(&ent, testcode);
    AnalyzeLines(&ent);
    AnalyzeWords(&ent);
    AnalyzeProcedures(&ent);

    //REN - rename
    //MOV - move
    //FDR - find references
    //INS - insert
    //DEL - delete
}

