#ifndef LOGICSYSTEM_H
#define LOGICSYSTEM_H

#include "Singleton.h"
#include "CSession.h"
#include <queue>
#include <mutex>
#include <functional>
#include <thread>

typedef std::function<
    void(std::shared_ptr<CSession>, short msg_id, std::string msg_data)> FuncCallback;

class LogicNode;
class LogicSystem : public Singleton<LogicSystem> {
    friend class Singleton<LogicSystem>;
private:
    LogicSystem();
    void DealMsg();
    void RegisterCallBacks();

    void HelloWorldCallback(std::shared_ptr<CSession> session,
         short msg_id, std::string msg_data);
public:
    ~LogicSystem();
    void PostMsg2Que(std::shared_ptr<LogicNode> msg);

private:
    std::queue<std::shared_ptr<LogicNode>> _msg_que;
    std::mutex _mutex;
    std::thread _worker;
    std::map<short, FuncCallback> _func_callbacks;

    std::condition_variable _consume;//
    bool _b_stop;                    //
};
#endif // LOGICSYSTEM_H