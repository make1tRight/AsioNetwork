#ifndef MSGNODE_H
#define MSGNODE_H


const int HEAD_TOTAL_LEN = 4;
const int HEAD_ID_LEN = 2;
const int HEAD_DATA_LEN = 2;
class LogicSystem;

class MsgNode {
    friend class Session;
public:
    MsgNode(int total_len);
    ~MsgNode();

    void Clear();
public:
    char* _data;
    int _total_len;
    int _cur_len;
};

class RecvNode : public MsgNode {
    friend class LogicSystem;
public:
    RecvNode(short max_len, short msg_id);
private:
    short _msg_id;
};

class SendNode : public MsgNode {
    // friend class LogicSystem;
public:
    SendNode(const char* msg, short max_len, short msg_id);
private:
    short _msg_id;
};
#endif // MSGNODE_H