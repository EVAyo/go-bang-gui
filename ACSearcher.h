#pragma once

#include <string>
#include <vector>
#include <map>
using namespace std;

//trie���ڵ�
struct ACNode {
    ACNode(int p, char c)
        :parent(p),ch(c),fail(-1)
    {
    }

    ACNode() {}

    int parent;
    char ch;
    int fail;
    map<char, int> sons;
    vector<int> output;
};

//AC�㷨��
class ACSearcher
{
public:
    ACSearcher();
    ~ACSearcher();

    void LoadPatern(const vector<string>& paterns);
    void BuildGotoTable();
    void BuildFailTable();
    vector<int> ACSearch(const string& text);           //����ƥ�䵽��ģʽ������

private:
    int maxState;                                       //���״̬��
    vector<ACNode> nodes;                               //trie��
    vector<string> paterns;                             //��Ҫƥ���ģʽ

    void AddState(int parent, char ch);                                    //��ʼ����״̬
};

