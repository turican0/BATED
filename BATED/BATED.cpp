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

//void test    () {};

/*typedef struct {
    vector<TypeLine> lines;
    string entity;
    bool comment;
} TypeCommentEnt;*/

vector<TypeLine> ent;

void SetComment(string* testcomment, long begin, long size, bool comment) {
    for (int i = begin; i < begin + size; i++)
        if (comment)
            (*testcomment)[i] = 'y';
        else
            (*testcomment)[i] = 'n';
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
                SetComment(testcomment, oldindex, testcode->length() - oldindex, false);
                break;
            }
            else
            {
                if (testIndex1 < testIndex2)
                {
                    index = testIndex1;
                    SetComment(testcomment, oldindex, index - oldindex, false);
                    state = 1;
                }
                else
                {
                    index = testIndex2;
                    SetComment(testcomment, oldindex, index - oldindex, false);
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
            SetComment(testcomment, oldindex, index - oldindex, true);
            state = 0;
        }
        else
        {
            index = testcode->find("*/", index);
            index += 2;
            SetComment(testcomment, oldindex, index - oldindex, true);
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
            TypeLine locLine;
            index++;
            locLine.entity = (*testcode).substr(oldindex, index - oldindex);
            locLine.comment = (*testcomment).substr(oldindex, index - oldindex);
            locLine.lineIndex = lineIndex++;
            if (locLine.entity.length() > 0)
                ent->push_back(locLine);
        }
        else
        {
            TypeLine locLine;
            index = ent->size();
            locLine.entity = (*testcode).substr(oldindex, index - oldindex);
            locLine.comment = (*testcomment).substr(oldindex, index - oldindex);
            locLine.lineIndex = lineIndex;
            if (locLine.entity.length() > 0)
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

    if (((word[0] >= 'a') && (word[0] <= 'z'))|| ((word[0] >= 'A') && (word[0] <= 'Z')))
        return 1;

    return 0;
}

long FindNextWord(string* entity, string* comment, long index, int* state) {
    long actIndex = index;
    while (actIndex < entity->size())
    {
            if ((*comment)[actIndex]=='n')
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
            if (((((*entity)[actIndex] < '0') || ((*entity)[actIndex] > '9')) &&
                (((*entity)[actIndex] < 'a') || ((*entity)[actIndex] > 'z')) &&
                (((*entity)[actIndex] < 'A') || ((*entity)[actIndex] > 'Z')) &&
                ((*entity)[actIndex] != '_') && ((*entity)[actIndex] != '#')) && (*state != 0)&&
                ((*entity)[actIndex] != '(') && ((*entity)[actIndex] != ')') &&
                ((*entity)[actIndex] != '{') && ((*entity)[actIndex] != '}') &&
                ((*entity)[actIndex] != ';') && ((*entity)[actIndex] != '\n'))
            {
                *state = 0;
                return actIndex;
            }
            else
                actIndex++;
    }
    return -1;
}

void AnalyzeWords(vector<TypeLine>* ent) {
    for (int j = 0; j < ent->size(); j++)
    {
        TypeLine* actLine = &(*ent)[j];
        bool runAgain = true;
        long index = 0;
        int state = -1;
        while (runAgain) {
            long oldindex = index;
            index = FindNextWord(&actLine->entity, &actLine->comment, index, &state);
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
    } while ((runAgain) && (beginTempJ > -1));
    if (endTempJ == -1)
    {
        endTempJ = tempJ;
        endTempK = tempK;
    }
    else
        GetPreIndex(ent, &endTempJ, &endTempK);
    if ((*ent)[beginTempJ].lineIndex == (*ent)[endTempJ].lineIndex)
        return false;
    endTempK = 0;
    do
    {
        if (((*ent)[endTempJ].words[endTempK].type != 0) && !(WordComment((*ent)[endTempJ].words[endTempK].comment)))
            return false;
        if (WordComment((*ent)[beginTempJ].words[beginTempK].comment))
            isComment = true;
        GetPreIndex(ent, &endTempJ, &endTempK);
    } while ((endTempJ > -1) && ((beginTempJ != endTempJ) || (beginTempK != endTempK)));
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
    } while ((runAgain) && (beginTempJ > -1));
    if (endTempJ == -1)
    {
        endTempJ = tempJ;
        endTempK = tempK;
    }
    else
        GetPreIndex(ent, &endTempJ, &endTempK);
    if ((*ent)[beginTempJ].lineIndex == (*ent)[endTempJ].lineIndex)
        return false;
    endTempK = 0;
    do
    {
        if (((*ent)[endTempJ].words[endTempK].type != 0) && !(WordComment((*ent)[endTempJ].words[endTempK].comment)))
            return false;
        //if ((*ent)[endTempI].comment)
        //    isComment = true;
        GetPreIndex(ent, &endTempJ, &endTempK);
    } while ((endTempJ > -1) && ((beginTempJ != endTempJ) || (beginTempK != endTempK)));
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

void SetProcedureInfo(vector<TypeLine>* ent, long j, long k) {

    long preJ = j;
    long preK = k;
    long preJ2 = j;
    long preK2 = k;

    bool test;
    bool test2;
    do
    {
        preJ2 = preJ;
        preK2 = preK;

        GetPreIndex(ent, &preJ, &preK);
    } while ((preJ > -1) && (((*ent)[preJ].words[preK].entity != "\n") && (((*ent)[preJ].words[preK].entity != ";") || WordComment((*ent)[preJ].words[preK].comment))));

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

        if ((procState == 0) && (((*ent)[postJ].words[postK].entity == "(") && !WordComment((*ent)[postJ].words[postK].comment)))
        {
            procState++;
            isDec = true;
        }
        if ((procState == 1) && (((*ent)[postJ].words[postK].entity == ")") && !WordComment((*ent)[postJ].words[postK].comment)))
        {
            procState++;
        }
        if ((procState == 2) && (((*ent)[postJ].words[postK].entity == ";") && !WordComment((*ent)[postJ].words[postK].comment)))
        {
            runAgain = false;
        }
        if (procState >= 2)
        {
            if ((procState) && (((*ent)[postJ].words[postK].entity == "{") && !WordComment((*ent)[postJ].words[postK].comment)))
            {
                procState++;
                isProc = true;
            }
            if ((procState) && (((*ent)[postJ].words[postK].entity == "}") && !WordComment((*ent)[postJ].words[postK].comment)))
            {
                procState--;
                if(procState == 2)
                    runAgain = false;
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

        if ((*ent)[preJ2].lineIndex != (*ent)[postJ2].lineIndex)//not inline
        {
            while (PrevLineComment(ent, &preJ2, &preK2))
                SetPrevLine(ent, &preJ2, &preK2);
            while (PostLineComment(ent, &postJ2, &postK2))
                SetPostLine(ent, &postJ2, &postK2);
            while (PostLineEmpty(ent, &postJ2, &postK2))
                SetPostLine(ent, &postJ2, &postK2);
        }

        (*ent)[j].words[k].type = 20;
        (*ent)[j].words[k].beginLine = preJ2;
        (*ent)[j].words[k].beginWord = preK2;
        (*ent)[j].words[k].endLine = postJ2;
        (*ent)[j].words[k].endWord = postK2;
    }

    if (isDec)
        PrintEntity(ent, (*ent)[j].words[k], "Dec: |", "|\n");
    if ((isProc) && (!isProc))
        PrintEntity(ent, (*ent)[j].words[k], "Proc: |", "|\n");
};

void AnalyzeProcedures(vector<TypeLine>* ent) {
    for (int j = 0; j < ent->size(); j++)
    {
        TypeLine* actLine = &(*ent)[j];
        for (int k = 0; k < actLine->words.size(); k++)
        {
            if (actLine->words[k].type == 1)
            {
                SetProcedureInfo(ent, j, k);
            }
        }
    }
}

int main()
{
    testcode = "  \n // \n/**/ /* */\n int main()\n {\n}\n //xx\n //yyy\n111\n22\n";
    string testcomment;
    AnalyzeComments(&testcode, &testcomment);
    AnalyzeLines(&ent, &testcode, &testcomment);
    AnalyzeWords(&ent);
    AnalyzeProcedures(&ent);

    //REN - rename
    //MOV - move
    //FDR - find references
    //INS - insert
    //DEL - delete
} ;

