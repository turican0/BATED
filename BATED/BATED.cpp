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
    //long lineIndex;
    //long charIndex;
    string entity;
    bool comment;
} TypeCommentEnt;

vector<TypeCommentEnt> ent;

void FindComments(vector<TypeCommentEnt>* ent, string testcode) {
    int state = 0;
    long index = 0;
    bool runAgain = true;
    //index = testcode.find("//", index + 1);
    while (runAgain)
    {
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
            locEnt.entity = testcode.substr(oldindex, index - oldindex);
            locEnt.comment = true;
            if (locEnt.entity.length() > 0)
                ent->push_back(locEnt);
            state = 0;
        }
    }
};

int main()
{
    testcode = "#include <iostream>\n//my code /*\n    /*\n    aa //command\n    //\n    */\n    int main()\n    {\n    }\n";

    FindComments(&ent, testcode);

    std::cout << "Hello World!\n";
}

