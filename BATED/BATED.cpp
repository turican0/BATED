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
    long lineIndex;
    string entity;
} TypeLine;

typedef struct {
    //long lineIndex;
    //long charIndex;
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

void AnalyzeLines(vector<TypeCommentEnt>* ent, string testcode) {
    long lineIndex = 0;
    for (int i = 0; i < ent->size(); i++)
    {
        TypeCommentEnt* actEnt = &(*ent)[i];
        bool runAgain = true;
        //index = testcode.find("//", index + 1);
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

void AnalyzeWords(vector<TypeCommentEnt>* ent, string testcode) {
}

void AnalyzeProcedures(vector<TypeCommentEnt>* ent, string testcode) {
}

int main()
{
    testcode = "#include <iostream>\n//my code /*\n    /*\n    aa //command\n    //\n    */\n    int main()\n    {\n    }\n/*   */\n";

    AnalyzeComments(&ent, testcode);
    AnalyzeLines(&ent, testcode);
    AnalyzeWords(&ent, testcode);
    AnalyzeProcedures(&ent, testcode);

    //REN - rename
    //MOV - move
    //FDR - find references
    //INS - insert
    //DEL - delete 
}

